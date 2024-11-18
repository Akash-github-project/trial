#ifndef TEXTDECRYPTOR_H
#define TEXTDECRYPTOR_H

#include <QObject>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iostream>
#include <vector>
#include <cstring>

class TextDecryptor : public QObject
{
    Q_OBJECT
public:
    explicit TextDecryptor(QObject *parent = nullptr);

    QString decryptText(QString keyHex, QString ivHex, QString chipper);
    std::vector<unsigned char> hexStringToByteArray(const std::string &hexStr);
    bool decrypt_data(const std::vector<unsigned char> &key, const std::vector<unsigned char> &iv, const std::vector<unsigned char> &ciphertext, const std::vector<unsigned char> &tag, std::vector<unsigned char> &plaintext);
signals:
};

#endif // TEXTDECRYPTOR_H
