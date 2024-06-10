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

EncryptionHandler::EncryptionHandler() {
    CryptoPP::byte keyData[] = { 0xba, 0xb2, 0xb9, 0xa7, 0xf4, 0x00, 0x83, 0x3d, 0x48, 0x94, 0x5f, 0xc3, 0x85, 0xf8, 0x45, 0x92,
                      0x07, 0xe3, 0x97, 0x70, 0x44, 0xed, 0x72, 0x24, 0xb8, 0x29, 0xc5, 0x4c, 0x94, 0xeb, 0x91, 0x0f };
    key.Assign(keyData, sizeof(keyData));
}


namespace fs = std::filesystem;
using namespace CryptoPP;



std::vector<byte> EncryptionHandler::DecryptVideoToMemory(const QByteArray& inputData, const SecByteBlock& key) {
    // Read IV from the beginning of the input data
    SecByteBlock iv(AES::BLOCKSIZE);
    memcpy(iv.BytePtr(), inputData.constData(), iv.size());

    // Initialize decryptor
    CBC_Mode<AES>::Decryption decryptor;
    decryptor.SetKeyWithIV(key, key.size(), iv);

    // Decrypt the input data (excluding the IV) and store the data in memory
    ByteQueue byteQueue;
    ArraySource(reinterpret_cast<const byte*>(inputData.constData() + iv.size()), inputData.size() - iv.size(), true,
                new StreamTransformationFilter(decryptor,
                                               new Redirector(byteQueue)
                                               )
                );

    // Convert ByteQueue to std::vector<byte>
    std::vector<byte> decryptedData(byteQueue.MaxRetrievable());
    byteQueue.Get(decryptedData.data(), decryptedData.size());

    return decryptedData;
}

std::vector<byte> EncryptionHandler::DecryptVideoFromQByteArray(const QByteArray& byteArray, const SecByteBlock& key) {
    return DecryptVideoToMemory(byteArray, key);
}


QByteArray EncryptionHandler::decryptFile(QByteArray byteArray) {

    byte key[32] = {0xba, 0xb2, 0xb9, 0xa7, 0xf4, 0x00, 0x83, 0x3d, 0x48, 0x94, 0x5f, 0xc3, 0x85, 0xf8, 0x45, 0x92,
                    0x07, 0xe3, 0x97, 0x70, 0x44, 0xed, 0x72, 0x24, 0xb8, 0x29, 0xc5, 0x4c, 0x94, 0xeb, 0x91, 0x0f};
    SecByteBlock keyBlock(key, sizeof(key));
    std::vector<byte> decryptedData = DecryptVideoFromQByteArray(byteArray, keyBlock);
    // wite this data to file
    QByteArray* data = VectorToQByteArray( decryptedData );
    // qDebug()<<"writing file";
    // QFile file("some_me_video.mp4");
    // file.open(QIODevice::WriteOnly);
    // file.write(*data);
    // file.close();
    // qDebug()<<"wrote the file";
    /////
    return *data;
}










