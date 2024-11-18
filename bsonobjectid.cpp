#include "bsonobjectid.h"

std::mutex BSONObjectID::mutex;
uint32_t BSONObjectID::counter = static_cast<uint32_t>(std::chrono::steady_clock::now().time_since_epoch().count() % 0xFFFFFF);

BSONObjectID::BSONObjectID(QObject *parent)
    : QObject{parent}
{}

// Initialize static members
std::string BSONObjectID::generate() {
    std::lock_guard<std::mutex> lock(mutex);  // Ensure thread-safety
    std::array<uint8_t, 12> id;

    // Timestamp (4 bytes)
    uint32_t timestamp = static_cast<uint32_t>(std::time(nullptr));
    id[0] = (timestamp >> 24) & 0xFF;
    id[1] = (timestamp >> 16) & 0xFF;
    id[2] = (timestamp >> 8) & 0xFF;
    id[3] = timestamp & 0xFF;

    // Machine ID (3 bytes)
    auto machineID = generateMachineID();
    for (size_t i = 0; i < 3; ++i) {
        id[4 + i] = machineID[i];
    }

    // Process ID (2 bytes)
    uint16_t pid = static_cast<uint16_t>(::getpid());
    id[7] = (pid >> 8) & 0xFF;
    id[8] = pid & 0xFF;

    // Counter (3 bytes)
    counter++;
    id[9] = (counter >> 16) & 0xFF;
    id[10] = (counter >> 8) & 0xFF;
    id[11] = counter & 0xFF;

    // Convert id array to a hexadecimal string
    std::ostringstream oss;
    for (auto byte : id) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return oss.str();
}

std::array<uint8_t, 3> BSONObjectID::generateMachineID() {
    std::array<uint8_t, 3> machineID;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 255);
    for (size_t i = 0; i < 3; ++i) {
        machineID[i] = static_cast<uint8_t>(distrib(gen));
    }
    return machineID;
}
