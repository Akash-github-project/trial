#include "sharekeygenerator.h"

ShareKeyGenerator::ShareKeyGenerator(QObject *parent)
    : QObject{parent}
{}

void ShareKeyGenerator::handleOpenSSLErrors() {
    unsigned long errCode = ERR_get_error();
    if (errCode) {
        // Convert the error code to a human-readable string
        const char* errString = ERR_error_string(errCode, nullptr);
        qDebug() << "OpenSSL Error:" << errString;
    } else {
        qDebug() << "Unknown OpenSSL error occurred.";
    }
    abort();
}

EC_KEY* ShareKeyGenerator::generateECKeyPair() {
    EC_KEY *key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if (!key || EC_KEY_generate_key(key) != 1) {
        qCritical() << "Error generating EC key pair";
        handleOpenSSLErrors();
    }
    return key;
}

// Function to extract the public key in PEM format (client-side public key)
QByteArray ShareKeyGenerator::extractPublicKeyPEM(EC_KEY* key) {
    BIO *pub = BIO_new(BIO_s_mem());
    if (PEM_write_bio_EC_PUBKEY(pub, key) != 1) {
        qCritical() << "Error writing public key";
        handleOpenSSLErrors();
    }
    BUF_MEM *pub_mem;
    BIO_get_mem_ptr(pub, &pub_mem);
    QByteArray publicKey(pub_mem->data, pub_mem->length);
    BIO_free(pub);
    return publicKey;
}

EC_KEY* ShareKeyGenerator::rebuildPublicKeyFromHex(const QString& hexString) {
    // Step 1: Convert the hex string back to a byte array
    // Step 1: Convert the hex string back to a byte array
    QByteArray pubKeyBytes = QByteArray::fromHex(hexString.toUtf8());

    // Debugging: Print the hex string and its length
    qDebug() << "Hex string length:" << hexString.length();
    qDebug() << "Public key byte array length:" << pubKeyBytes.size();
    qDebug() << "Public key byte array (hex):" << pubKeyBytes.toHex();

    if (pubKeyBytes.size() != 65 || pubKeyBytes[0] != 0x04) {
        qCritical() << "Invalid public key format. Expected uncompressed public key (65 bytes, starts with 0x04).";
        return nullptr;
    }

    // Step 2: Get the EC_GROUP for the curve (assuming SECP256R1)
    EC_KEY *key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1); // SECP256R1
    if (!key) {
        qCritical() << "Error creating EC_KEY object";
        handleOpenSSLErrors();
    }

    // Step 3: Convert byte array into EC_POINT
    EC_POINT *pubKeyPoint = EC_POINT_new(EC_KEY_get0_group(key));
    if (EC_POINT_oct2point(EC_KEY_get0_group(key), pubKeyPoint,
                           reinterpret_cast<const unsigned char*>(pubKeyBytes.constData()),
                           pubKeyBytes.size(), nullptr) != 1) {
        qCritical() << "Error converting octets to EC_POINT";
        handleOpenSSLErrors();
    }

    // Step 4: Assign the EC_POINT to the EC_KEY object
    if (EC_KEY_set_public_key(key, pubKeyPoint) != 1) {
        qCritical() << "Error setting public key";
        handleOpenSSLErrors();
    }

    // Cleanup
    EC_POINT_free(pubKeyPoint);

    return key; // Now the EC_KEY has the server's public key
}
// Function to load the server's public key from a PEM string
EC_KEY* ShareKeyGenerator::loadServerPublicKey(const QByteArray& pemKey) {
    qDebug() << "PEM Key:" << pemKey;

    BIO *bio = BIO_new_mem_buf(pemKey.data(), pemKey.size());
    if (!bio) {
        qCritical() << "Failed to create BIO";
        handleOpenSSLErrors();
    }

    EC_KEY *ecKey = PEM_read_bio_EC_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);

    if (!ecKey) {
        qCritical() << "Failed to load public key";
        handleOpenSSLErrors();
    }

    return ecKey;
}

QByteArray ShareKeyGenerator::deriveSharedSecret(EC_KEY* client_key, EC_POINT* server_point) {
    const EC_GROUP* group = EC_KEY_get0_group(client_key);
    unsigned char shared_secret[32];  // Adjust size depending on curve (32 bytes for SECP256R1)

    // Compute the shared secret using the client's private key and the server's public key
    int secret_len = ECDH_compute_key(shared_secret, sizeof(shared_secret), server_point, client_key, nullptr);
    if (secret_len <= 0) {
        qCritical() << "Error deriving shared secret";
        handleOpenSSLErrors();
    }

    // Return the shared secret as a QByteArray
    return QByteArray(reinterpret_cast<char*>(shared_secret), secret_len);
}


// Function to convert hex string to EC_POINT
EC_POINT* ShareKeyGenerator::hexToECPoint(const QByteArray& hex, EC_GROUP* group) {
    QByteArray bytes = QByteArray::fromHex(hex);
    EC_POINT* point = EC_POINT_new(group);
    if (EC_POINT_oct2point(group, point, reinterpret_cast<const unsigned char*>(bytes.constData()), bytes.size(), nullptr) != 1) {
        qCritical() << "Error converting hex to EC_POINT";
        handleOpenSSLErrors();
    }
    return point;
}

QByteArray ShareKeyGenerator::publicKeyToUncompressedHex(EC_KEY* ecKey) {
    const EC_GROUP *group = EC_KEY_get0_group(ecKey);
    const EC_POINT *point = EC_KEY_get0_public_key(ecKey);

    if (!group || !point) {
        qCritical() << "Failed to get EC group or point" ;
        handleOpenSSLErrors();
    }

    // Get size of uncompressed key
    size_t key_len = EC_POINT_point2oct(group, point, POINT_CONVERSION_UNCOMPRESSED, nullptr, 0, nullptr);
    QByteArray uncompressedKey(key_len, '\0');

    // Convert to uncompressed format (starts with 0x04)
    EC_POINT_point2oct(group, point, POINT_CONVERSION_UNCOMPRESSED, (unsigned char*)uncompressedKey.data(), key_len, nullptr);

    return uncompressedKey.toHex();
}

QByteArray ShareKeyGenerator::deriveKeyFromSharedSecret(const QByteArray& sharedSecret, const QByteArray& info, size_t keyLength) {
    //ECDH_compute_key()

    // using namespace CryptoPP;

    // CryptoPP::byte* sharedSecretBytes = const_cast<CryptoPP::byte*>(reinterpret_cast<const CryptoPP::byte*>(sharedSecret.data()));
    // CryptoPP::byte* infoBytes = const_cast<CryptoPP::byte*>(reinterpret_cast<const CryptoPP::byte*>(info.data()));

    // // Create a byte array to hold the derived key
    // std::vector<CryptoPP::byte> derivedKey(keyLength);
    // // Create an HKDF object using SHA256
    // CryptoPP::HKDF<CryptoPP::SHA256> hkdf;
    // // Derive the key using HKDF
    // hkdf.DeriveKey(
    //     derivedKey.data(),          // Output key buffer
    //     derivedKey.size(),          // Length of the output key
    //     sharedSecretBytes,          // Input keying material (shared secret)
    //     sharedSecret.size(),        // Length of the shared secret
    //     nullptr,                    // No salt (optional, can be nullptr)
    //     0,                          // Length of salt (0 if no salt)
    //     infoBytes,                  // Context info (optional, can be nullptr)
    //     info.size()                 // Length of the context info
    //     );
    // // Convert derived key to QByteArray and return
    // return QByteArray(reinterpret_cast<const char*>(derivedKey.data()), derivedKey.size());
    return QByteArray();
}

// HKDF Extract + Expand
void ShareKeyGenerator::hkdf(const QByteArray &salt, const QByteArray &ikm, const QByteArray &info, QByteArray &okm, int okm_len) {
    unsigned char prk[EVP_MAX_MD_SIZE];  // Pseudo-random key (PRK)
    unsigned int prk_len;

    // Step 1: HKDF-Extract
    // HMAC-SHA256 with salt as key and IKM (shared secret) as data
    HMAC(EVP_sha256(), salt.constData(), salt.size(), reinterpret_cast<const unsigned char *>(ikm.constData()), ikm.size(), prk, &prk_len);

    // Step 2: HKDF-Expand
    // We need to generate okm_len bytes of key material, which requires repeated HMAC computations
    int hash_len = EVP_MD_size(EVP_sha256());  // Output length of HMAC-SHA256
    unsigned char t[EVP_MAX_MD_SIZE];  // Buffer for the result of each round
    unsigned int t_len = 0;
    unsigned char counter = 1;
    int bytes_remaining = okm_len;

    // Initialize okm (output key material)
    okm.resize(okm_len);

    while (bytes_remaining > 0) {
        // HMAC-SHA256 with PRK as key, and previous T (if any) + info + counter as data
        HMAC_CTX *ctx = HMAC_CTX_new();
        HMAC_Init_ex(ctx, prk, prk_len, EVP_sha256(), nullptr);

        if (t_len > 0) {
            HMAC_Update(ctx, t, t_len);  // Previous T (if any)
        }

        HMAC_Update(ctx, reinterpret_cast<const unsigned char *>(info.constData()), info.size());  // Optional context information
        HMAC_Update(ctx, &counter, 1);  // Counter byte

        HMAC_Final(ctx, t, &t_len);
        HMAC_CTX_free(ctx);

        int bytes_to_copy = (bytes_remaining > hash_len) ? hash_len : bytes_remaining;
        std::memcpy(okm.data() + (okm_len - bytes_remaining), t, bytes_to_copy);
        bytes_remaining -= bytes_to_copy;
        counter++;
    }
}
