#ifndef BSONOBJECTID_H
#define BSONOBJECTID_H

#include <QObject>
#include <ctime>
#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <mutex>
#include <unistd.h>

class BSONObjectID : public QObject
{
public:
    explicit BSONObjectID(QObject *parent = nullptr);

public:
    // Generates and returns a BSON ObjectID as a hexadecimal string
    static std::string generate();

private:
    static std::mutex mutex;
    static uint32_t counter;

    // Generates a random machine ID (3 bytes)
    static std::array<uint8_t, 3> generateMachineID();
};

#endif // BSONOBJECTID_H
