#ifndef SECUREMEMORY_H
#define SECUREMEMORY_H

#include <windows.h>
#include <wincrypt.h>
#include <QDebug>
#include <QString>
#include <QByteArray>

class SecureMemory {
public:
    // Constructor: Takes QString, converts it to QByteArray, and handles encryption
    SecureMemory(const QString& data);
    // Destructor: Clean up memory and securely erase
    ~SecureMemory();
    SecureMemory();
    // Access the decrypted sensitive data as a QString
    QString getData();
    // Manually re-encrypt the data in memory
    void secure();

private:
    char* sensitiveData = nullptr;
    size_t dataSize = 0;
    // Lock memory to prevent paging
    bool lockMemory();
    // Unlock memory
    void unlockMemory();
    // Encrypt memory in place
    void protectMemory();
    // Decrypt memory in place
    void unprotectMemory();
    // Securely erase memory by overwriting it with zeros
    void secureEraseMemory();
};


#endif // SECUREMEMORY_H
