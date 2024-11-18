#ifndef SHAREKEYGENERATOR_H
#define SHAREKEYGENERATOR_H

#include <QObject>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/hmac.h>

#include <QDebug>
#include <QByteArray>

// cryptopp imports

#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>
#include <cryptopp/hkdf.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>

class ShareKeyGenerator : public QObject
{
    Q_OBJECT
public:
    explicit ShareKeyGenerator(QObject *parent = nullptr);

    void handleOpenSSLErrors();
    EC_KEY *generateECKeyPair();
    QByteArray extractPublicKeyPEM(EC_KEY *key);
    EC_KEY *loadServerPublicKey(const QByteArray& pemKey);
    EC_KEY *rebuildPublicKeyFromHex(const QString &hexString);
    QByteArray publicKeyToUncompressedHex(EC_KEY *ecKey);
    EC_POINT *hexToECPoint(const QByteArray &hex, EC_GROUP *group);
    QByteArray deriveSharedSecret(EC_KEY *client_key, EC_POINT *server_point);
    QByteArray deriveKeyFromSharedSecret(const QByteArray &sharedSecret, const QByteArray &info, size_t keyLength);
    void hkdf(const QByteArray &salt, const QByteArray &ikm, const QByteArray &info, QByteArray &okm, int okm_len);
signals:
};

#endif // SHAREKEYGENERATOR_H
