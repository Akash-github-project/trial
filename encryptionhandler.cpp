#include "encryptionhandler.h"

EncryptionHandler::EncryptionHandler() { }


namespace fs = std::filesystem;
using namespace CryptoPP;



std::vector<byte> EncryptionHandler::DecryptVideoToMemory(const QByteArray& inputData, const SecByteBlock& key,const SecByteBlock& iv) {

    // Initialize decryptor
    CBC_Mode<AES>::Decryption decryptor;
    decryptor.SetKeyWithIV(key, key.size(), iv);
    // Decrypt the input data (excluding the IV) and store the data in memory
    ByteQueue byteQueue;

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
    // QFile file("C:\\Users\\BharatCaller\\temp.mp4");

    // if (!file.open(QIODevice::ReadWrite)) {
    //     qWarning() << "Failed to open file for writing:" << file.errorString();
    //     return false;
    // }

    // qint64 bytesWritten = file.write(data);
    // if (bytesWritten == -1) {
    //     qWarning() << "Failed to write data to file:" << file.errorString();
    //     return false;
    // }

    // if (bytesWritten != data.size()) {
    //     qWarning() << "Not all data was written to the file.";
    //     return false;
    // }

    // file.close();
    return true;
}



QByteArray EncryptionHandler::decryptFile(QByteArray byteArray,VideoData vidItem) {
    qDebug()<<"entering dec block";
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
    QByteArray data = VectorToQByteArray( decryptedData );
    // qDebug()<<"writing file";
    /////
    //writeByteArrayToFile(*data,vidItem.fileName);
    ///

    qDebug()<<"exiting dec block";
    return data;
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
// GCM NEW

std::vector<byte> EncryptionHandler::DecryptTextToMemory(const QByteArray& inputData, const SecByteBlock& key, const SecByteBlock& iv) {

    // Initialize decryptor
    GCM<AES>::Decryption decryptor;
    decryptor.SetKeyWithIV(key, key.size(), iv);

    // Decrypt the input data and store the data in memory
    ByteQueue byteQueue;

    try {
        // Since there's no tag, treat the input data as raw ciphertext
        ArraySource(reinterpret_cast<const byte*>(inputData.constData()),
                    inputData.size(),
                    true,
                    new AuthenticatedDecryptionFilter(decryptor, new Redirector(byteQueue),AuthenticatedDecryptionFilter::DEFAULT_FLAGS, 0));
    } catch (const Exception& e) {
        qDebug() << e.GetWhat();
        return {};
    }

    // Convert ByteQueue to std::vector<byte>
    std::vector<byte> decryptedData(byteQueue.MaxRetrievable());
    byteQueue.Get(decryptedData.data(), decryptedData.size());

    return decryptedData;
}

QByteArray EncryptionHandler::decryptString(QByteArray byteArray,QString encIv, QString encKey) {

    std::vector<CryptoPP::byte> listOfKeys = hexStringToByteArray(encKey.toStdString());
    std::vector<CryptoPP::byte> listOfIv = hexStringToByteArray(encIv.toStdString());
    CryptoPP::byte key[32] = {};
    CryptoPP::byte iv[AES::BLOCKSIZE] = {};


    std::copy(listOfKeys.begin(),listOfKeys.end(),key);
    std::copy(listOfIv.begin(),listOfIv.end(),iv);

    qDebug()<<*key;
    qDebug()<<*iv;

    SecByteBlock keyBlock(key, sizeof(key));
    SecByteBlock ivBlock(iv,sizeof(iv));
    std::vector<byte> decryptedData = DecryptTextToMemory(byteArray, keyBlock,ivBlock);
    // wite this data to file
    QByteArray data = VectorToQByteArray( decryptedData );
    // qDebug()<<"final final " << data->toBase64();
    // qDebug()<<"writing file";
    /////
    //writeByteArrayToFile(*data,vidItem.fileName);
    ///

    return data;
}


///

QString EncryptionHandler::decrypt_data(
    const QByteArray& hexKey,
    const QByteArray& hexIV,
    const QString& hexCiphertext,
    const QByteArray& hexTag
) {

    std::string decryptedText;
    try {
        std::string key = hexKey.toStdString();
        std::string iv = hexIV.toStdString();
        std::string ciphertext = hexCiphertext.toStdString();
        std::string tag = hexTag.toStdString();
        // Check that the key and IV are of valid sizes
        if (key.size() != 32 || iv.size() != 12) {
            qDebug() << "Invalid key or IV size. AES-256 requires a 32-byte key and a 12-byte IV.";
            qDebug() <<"hex key"<<hexKey;
            qDebug() <<"hex key"<<hexKey.size();
            qDebug() <<"last key" <<key;
            qDebug() <<"last key size" <<key.size();

            qDebug() <<"hex iv"<<hexIV;
            qDebug() <<"hex key"<<hexIV.size();
            qDebug() <<"last iv" <<iv;
            qDebug() <<"last iv size" <<iv.size();
            return "ERROR";
        }

        // GCM Decryption object
        GCM<AES>::Decryption decryptor;
        decryptor.SetKeyWithIV(reinterpret_cast<const byte*>(key.data()), key.size(),
                               reinterpret_cast<const byte*>(iv.data()), iv.size());

        qDebug() << "Key and IV set successfully";

        // Decrypt and verify authentication tag
        AuthenticatedDecryptionFilter df(decryptor, new StringSink(decryptedText), AuthenticatedDecryptionFilter::DEFAULT_FLAGS, tag.size());

        qDebug() << "Starting to put ciphertext and tag";
        df.ChannelPut(DEFAULT_CHANNEL, reinterpret_cast<const byte*>(ciphertext.data()), ciphertext.size());
        df.ChannelPut(DEFAULT_CHANNEL, reinterpret_cast<const byte*>(tag.data()), tag.size());
        df.ChannelMessageEnd(DEFAULT_CHANNEL);

        // Check for authentication failure
        // qDebug()<<"text ----- " <<decryptedText;
        auto byteArray = convertStringToByteArray(decryptedText);
         QString result = QString::fromUtf8(reinterpret_cast<const char*>(byteArray.data()), byteArray.size());
        qDebug()<<"text----- result -- " << convertStringifiedHexToQString(result);
        if (!df.GetLastResult()) {
            qDebug() << "Decryption failed. Invalid authentication tag.";
            return "ERROR";
        }
    } catch (const Exception& e) {
        qDebug() << "Crypto++ error: " << e.what();
        return "ERROR";
    }

    return QString::fromStdString(decryptedText);
}

std::vector<unsigned char> EncryptionHandler::convertStringToByteArray(const std::string& str) {
    std::vector<unsigned char> byteArray;

    std::istringstream ss(str);
    std::string byteString;

    while (ss >> byteString) {
        if (byteString[0] == '\\' && byteString[1] == 'x' && byteString.size() == 4) {
            // Convert \xHH to a byte
            unsigned int byteValue;
            std::istringstream(byteString.substr(2)) >> std::hex >> byteValue;
            byteArray.push_back(static_cast<unsigned char>(byteValue));
        } else {
            // Handle regular characters
            byteArray.push_back(static_cast<unsigned char>(byteString[0]));
        }
    }

    return byteArray;
}

QString EncryptionHandler::convertStringifiedHexToQString(const QString& hexString) {
    QByteArray byteArray;

    // Iterate over the string to find all occurrences of the hex representation
    for (int i = 0; i < hexString.length(); ++i) {
        if (hexString[i] == '\\' && (i + 1) < hexString.length() && hexString[i + 1] == 'x') {
            // Ensure there are at least two more characters for the hex value
            if ((i + 3) < hexString.length()) {
                // Extract the hex value
                QString hexValue = hexString.mid(i + 2, 2);
                bool ok;
                // Convert the hex value to an unsigned char
                char byte = static_cast<char>(hexValue.toUInt(&ok, 16));
                if (ok) {
                    byteArray.append(byte);
                    // Move index forward by 3 to skip over the processed hex
                    i += 3;
                } else {
                    // Handle invalid hex conversion if necessary
                    qDebug() << "Invalid hex value:" << hexValue;
                }
            }
        } else {
            // Handle case for non-hex characters (if needed)
            // e.g., byteArray.append(hexString[i].toLatin1());
        }
    }

    // Convert QByteArray to QString
    return QString::fromUtf8(byteArray);
}
