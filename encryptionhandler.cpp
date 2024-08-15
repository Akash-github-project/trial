#include "encryptionhandler.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QString>
#include <QElapsedTimer>
#include <aes.h>
#include <modes.h>
#include <files.h>
#include <osrng.h>
#include <filters.h>

EncryptionHandler::EncryptionHandler() { }


namespace fs = std::filesystem;
using namespace CryptoPP;



std::vector<byte> EncryptionHandler::DecryptVideoToMemory(const QByteArray& inputData, const SecByteBlock& key,const SecByteBlock& iv) {
    // Initialize decryptor
    CBC_Mode<AES>::Decryption decryptor;
    decryptor.SetKeyWithIV(key, key.size(), iv);
    // Decrypt the input data (excluding the IV) and store the data in memory
    ByteQueue byteQueue;
    /////
    try {
        ArraySource(reinterpret_cast<const byte*>(inputData.constData()),
            inputData.size(),
            true,
            new StreamTransformationFilter(decryptor, new Redirector(byteQueue)));
    }catch (Exception e){
        qDebug()<<e.GetWhat();

    }
    // Convert ByteQueue to std::vector<byte>
    std::vector<byte> decryptedData(byteQueue.MaxRetrievable());
    byteQueue.Get(decryptedData.data(), decryptedData.size());

    return decryptedData;
}

std::vector<byte> EncryptionHandler::DecryptVideoFromQByteArray(const QByteArray& byteArray, const SecByteBlock& key,const SecByteBlock& iv) {
    return DecryptVideoToMemory(byteArray, key,iv);
}


bool EncryptionHandler::writeByteArrayToFile(const QByteArray data, const QString &filePath) {
    QFile file("C:\\Users\\BharatCaller\\temp.mp4");

    if (!file.open(QIODevice::ReadWrite)) {
        qWarning() << "Failed to open file for writing:" << file.errorString();
        return false;
    }

    qint64 bytesWritten = file.write(data);
    if (bytesWritten == -1) {
        qWarning() << "Failed to write data to file:" << file.errorString();
        return false;
    }

    if (bytesWritten != data.size()) {
        qWarning() << "Not all data was written to the file.";
        return false;
    }

    file.close();
    return true;
}



QByteArray EncryptionHandler::decryptFile(QByteArray byteArray,VideoData vidItem) {

    std::vector<CryptoPP::byte> listOfKeys = hexStringToByteArray(vidItem.key.toStdString());
    std::vector<CryptoPP::byte> listOfIv = hexStringToByteArray(vidItem.iv.toStdString());
    CryptoPP::byte key[32] = {};
    CryptoPP::byte iv[AES::BLOCKSIZE] = {};


    std::copy(listOfKeys.begin(),listOfKeys.end(),key);
    std::copy(listOfIv.begin(),listOfIv.end(),iv);

    qDebug()<<*key;
    qDebug()<<*iv;

    SecByteBlock keyBlock(key, sizeof(key));
    SecByteBlock ivBlock(iv,sizeof(iv));
    std::vector<byte> decryptedData = DecryptVideoFromQByteArray(byteArray, keyBlock,ivBlock);
    // wite this data to file
    QByteArray* data = VectorToQByteArray( decryptedData );
    // qDebug()<<"writing file";
    /////
    writeByteArrayToFile(*data,vidItem.fileName);
    ///

    return *data;
}

// Function to convert a hex string to an unsigned char array
std::vector<unsigned char> EncryptionHandler::hexStringToByteArray(const std::string &hexStr) {
    if (hexStr.length() % 2 != 0) {
        throw std::invalid_argument("Hex string must have an even number of characters");
    }

    std::vector<CryptoPP::byte> result;
    result.reserve(hexStr.length() / 2);

    for (size_t i = 0; i < hexStr.length(); i += 2) {
        std::string byteString = hexStr.substr(i, 2);
        unsigned char value = static_cast<CryptoPP::byte>(std::stoul(byteString, nullptr, 16));
        result.push_back(value);
    }
    return result;
}



