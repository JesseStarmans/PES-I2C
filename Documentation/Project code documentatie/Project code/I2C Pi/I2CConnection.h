/*!
    @file I2CConnection.h
    @brief Header file voor de I2CConnection klasse.

    Deze header file definieert de klasse die het mogelijk maakt om over I2C tussen een Raspberry Pi 4B en een NUCLEO-L432KC microcontroler te communiceren.
*/

#include <string>
#include <vector>
#include <cstdint>

/*!
    @class I2CConnection
    @brief Een klasse voor I2C communicatie.
*/
class I2CConnection {
public:
    I2CConnection(uint8_t);
    ~I2CConnection();
    void sendI2CTo(const uint8_t, std::vector<uint8_t>, uint8_t);
    std::vector<uint8_t> receive6OverI2C(uint8_t);
private:
    uint8_t slaveAddress;       /*!< Het slave adres.*/
    int slaveOpen;              /*!< File descriptor voor het openen van de I2C verbinding.*/
};
