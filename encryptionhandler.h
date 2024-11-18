#ifndef ENCRYPTIONHANDLER_H
#define ENCRYPTIONHANDLER_H

#include <QFile>
#include <QtCore>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QString>
#include <QElapsedTimer>
#include <QString>
#include <QByteArray>

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/modes.h>
#include <cryptopp/files.h>
#include <cryptopp/osrng.h>
#include <cryptopp/filters.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/gcm.h>
#include <cryptopp/hex.h>

#include <iostream>
#include <vector>
//
#include "apimanager.h"

class EncryptionHandler
{

public:
    EncryptionHandler();
    CryptoPP::SecByteBlock key;
    QByteArray decryptFile(QByteArray byteArray,VideoData vidItem);
    QByteArray* VectorToQByteArray(const std::vector<CryptoPP::byte> &vec) {
        return new QByteArray(reinterpret_cast<const char*>(vec.data()), vec.size());
    }
    std::vector<CryptoPP::byte> DecryptVideoFromQByteArray(const QByteArray &byteArray, const CryptoPP::SecByteBlock &key,const CryptoPP::SecByteBlock &iv);
    std::vector<CryptoPP::byte> DecryptVideoToMemory(const QByteArray &inputData, const CryptoPP::SecByteBlock &key,const CryptoPP::SecByteBlock &iv);
    std::vector<CryptoPP::byte> hexStringToByteArray(const std::string &hexStr);
    bool writeByteArrayToFile(const QByteArray data, const QString &filePath);
    QString decrypt_data(const QByteArray &hexKey, const QByteArray &hexIV, const QString &hexCiphertext, const QByteArray &hexTag);
    std::vector<unsigned char> convertStringToByteArray(const std::string &str);
    QString convertStringifiedHexToQString(const QString &hexString);
    QByteArray decryptString(QByteArray byteArray, QString encIv, QString encKey);
    std::vector<CryptoPP::byte> DecryptTextToMemory(const QByteArray &inputData, const CryptoPP::SecByteBlock &key, const CryptoPP::SecByteBlock &iv);
};

#endif // ENCRYPTIONHANDLER_H
