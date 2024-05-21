#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

class I2CConnection {
public:
    I2CConnection(uint8_t);
    ~I2CConnection();
    void sendI2CTo(const uint8_t, std::vector<uint8_t>, uint8_t);
    std::vector<uint8_t> receive6OverI2C(uint8_t);
private:
    uint8_t slaveAddress;
    int slaveOpen = 0;
};
