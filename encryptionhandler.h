#ifndef ENCRYPTIONHANDLER_H
#define ENCRYPTIONHANDLER_H

#include <qbytearray.h>
#include <qfile.h>
#include <QtCore>
#include <aes.h>
#include <filters.h>
#include <modes.h>
#include <osrng.h>
#include <files.h>

class EncryptionHandler
{

public:
    EncryptionHandler();
    CryptoPP::SecByteBlock key;
    QByteArray decryptFile(QByteArray byteArray);
    QByteArray* VectorToQByteArray(const std::vector<CryptoPP::byte> &vec) {
        return new QByteArray(reinterpret_cast<const char*>(vec.data()), vec.size());
    }
    std::vector<CryptoPP::byte> DecryptVideoFromQByteArray(const QByteArray &byteArray, const CryptoPP::SecByteBlock &key);
    std::vector<CryptoPP::byte> DecryptVideoToMemory(const QByteArray &inputData, const CryptoPP::SecByteBlock &key);
};

#endif // ENCRYPTIONHANDLER_H
