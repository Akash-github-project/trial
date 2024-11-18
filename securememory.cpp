#include "securememory.h"

SecureMemory::SecureMemory() {}

QString SecureMemory::getData() {
    unprotectMemory();  // Decrypt the data when accessed
    QString result = QString::fromUtf8(sensitiveData);
    protectMemory();  // Encrypt again after access
    return result;
}

void SecureMemory::secure() {
    protectMemory();  // Re-encrypt the data
}

bool SecureMemory::lockMemory() {
    if (VirtualLock(sensitiveData, dataSize)) {
        return true;
    }
    qDebug() << "Failed to lock memory.";
    return false;
}

void SecureMemory::unlockMemory() {
    VirtualUnlock(sensitiveData, dataSize);
}

void SecureMemory::protectMemory() {
    if (!CryptProtectMemory(sensitiveData, (DWORD)dataSize, CRYPTPROTECTMEMORY_SAME_PROCESS)) {
        qDebug() << "Failed to protect memory.";
    }
}

void SecureMemory::unprotectMemory() {
    if (!CryptUnprotectMemory(sensitiveData, (DWORD)dataSize, CRYPTPROTECTMEMORY_SAME_PROCESS)) {
        qDebug() << "Failed to unprotect memory.";
    }
}

void SecureMemory::secureEraseMemory() {
    SecureZeroMemory(sensitiveData, dataSize);
}

SecureMemory::SecureMemory(const QString &data) {
    QByteArray byteArray = data.toUtf8();
    dataSize = byteArray.size() + 1; // +1 for null terminator
    sensitiveData = (char*)malloc(dataSize);
    if (sensitiveData != nullptr) {
        memcpy(sensitiveData, byteArray.constData(), dataSize);
        if (lockMemory()) {
            protectMemory();  // Encrypt the data in memory
        }
    }
}

SecureMemory::~SecureMemory() {
    if (sensitiveData != nullptr) {
        unprotectMemory();  // Decrypt before erasing
        secureEraseMemory();
        unlockMemory();
        free(sensitiveData);  // Free the memory
    }
}
