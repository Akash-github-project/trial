#include "textdecryptor.h"
#include <QDebug>
const int AES_GCM_TAG_LENGTH = 16;
const int AES_GCM_IV_LENGTH = 12;

TextDecryptor::TextDecryptor(QObject *parent)
    : QObject{parent}
{}

bool TextDecryptor::decrypt_data(const std::vector<unsigned char>& key,
                                 const std::vector<unsigned char>& iv,
                                 const std::vector<unsigned char>& ciphertext,
                                 const std::vector<unsigned char>& tag,
                                 std::vector<unsigned char>& plaintext) {
    EVP_CIPHER_CTX* ctx;
    int len;
    int plaintext_len;
    int ret;

    // Resize the passed-in plaintext vector to hold the decrypted data
    plaintext.resize(ciphertext.size() - AES_GCM_TAG_LENGTH);

    // Create and initialize the context
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        throw std::runtime_error("Failed to create context");
    }

    // Initialize the decryption operation.
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize decryption");
    }

    // Set IV length
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, AES_GCM_IV_LENGTH, NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to set IV length");
    }

    // Initialize key and IV
    if (EVP_DecryptInit_ex(ctx, NULL, NULL, key.data(), iv.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize key and IV");
    }

    // Provide the ciphertext
    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size() - AES_GCM_TAG_LENGTH) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to decrypt ciphertext");
    }
    plaintext_len = len;

    // Set expected tag value
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, AES_GCM_TAG_LENGTH, (void*)(ciphertext.data() + ciphertext.size() - AES_GCM_TAG_LENGTH)) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to set tag");
    }

    // Finalize decryption
    ret = EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len);

    // Clean up
    EVP_CIPHER_CTX_free(ctx);

    if (ret > 0) {
        plaintext_len += len;
        plaintext.resize(plaintext_len);
        QString finalOutput = QString::fromUtf8(reinterpret_cast<const char*>(plaintext.data()), plaintext.size());
        qDebug()<<finalOutput;
        return true;
    } else {
        throw std::runtime_error("Decryption failed");
    }
    return false;
}


std::vector<unsigned char> TextDecryptor::hexStringToByteArray(const std::string &hexStr) {
    if (hexStr.length() % 2 != 0) {
        throw std::invalid_argument("Hex string must have an even number of characters");
    }

    std::vector<unsigned char> result;
    result.reserve(hexStr.length() / 2);

    for (size_t i = 0; i < hexStr.length(); i += 2) {
        std::string byteString = hexStr.substr(i, 2);
        unsigned char value = static_cast<unsigned char>(std::stoul(byteString, nullptr, 16));
        result.push_back(value);
    }
    return result;
}
// GCM NEW

QString TextDecryptor::decryptText(QString keyHex,QString ivHex,QString chipper) {
    // Example key (32 bytes for AES-256)
    std::vector<unsigned char> key = hexStringToByteArray(keyHex.toStdString());

    // Example IV (12 bytes for AES-GCM)
    std::vector<unsigned char> iv = hexStringToByteArray(ivHex.toStdString());

    // Example ciphertext
    std::vector<unsigned char> ciphertext = hexStringToByteArray(chipper.toStdString());

    // Example authentication tag (16 bytes for AES-GCM)
    std::vector<unsigned char> tag = {};

    // Output buffer for decrypted plaintext
    std::vector<unsigned char> plaintext;

    // Decrypt the data
    if (decrypt_data(key, iv, ciphertext, tag, plaintext)) {
        qDebug() << "Decryption successful!" ;
        QString finalOutput = QString::fromUtf8(reinterpret_cast<const char*>(plaintext.data()), plaintext.size());
        return finalOutput;
    } else {
        std::cerr << "Decryption failed!" << std::endl;
        return "NONE";
    }

}
