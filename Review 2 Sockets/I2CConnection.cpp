#include "I2CConnection.h"

#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

I2CConnection::I2CConnection(uint8_t slaveAddress) : slaveAddress(slaveAddress) {
    //std::cout<<slaveOpen<<std::endl;
    slaveOpen = open("/dev/i2c-1", O_RDWR);
    //std::cout<<slaveOpen<<std::endl;
    ioctl(slaveOpen, I2C_SLAVE, slaveAddress);
}

I2CConnection::~I2CConnection() {

}

void I2CConnection::sendI2CTo(const uint8_t pos, std::vector<uint8_t> tekst, uint8_t vecSize) {
    tekst.insert(tekst.begin(), pos);
    write(slaveOpen, tekst.data(), static_cast<int>(tekst.size()));
}

std::vector<uint8_t> I2CConnection::receive6OverI2C(uint8_t pos) {
    write(slaveOpen, &pos, 1);
    usleep(1000);
    uint8_t ingelezen[6] = {0};
    read(slaveOpen, ingelezen, 6);
    std::vector<uint8_t> tekst;
    for (int i = 0; i < (int)sizeof(ingelezen); i++) {
        tekst.push_back(ingelezen[i]);
    }
    return tekst;
}

