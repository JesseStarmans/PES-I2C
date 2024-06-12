/*!
    @file I2CConnection.cpp
    @date 21 May 2024
    @author Jesse Starmans & Glenn de Jonge
    @brief CPP file voor de I2CConnection klasse.

    De implementatie van de I2CConnection klasse. 
    
    Deze klasse maakt het mogelijk om over I2C tussen een Raspberry Pi 4B en een NUCLEO-L432KC microcontroler te communiceren. \n
    Voor de I2C connectie worden op de Raspberry Pi 4B pinnen 3 (SDA), 5 (SCL) en 6 (Ground) gebruikt. \n
    Voor de I2C connectie worden op de NUCLEO-L432KC pinnen D4/PB7 (I2C1_SDA), D5/PB6 (I2C1_SCL) en GND (Ground) gebruikt.

    Created on 21 May 2024
    by Jesse Starmans. \n
    Modified on 24 may 2024
    by Jesse Starmans. \n
*/

#include "I2CConnection.h"

#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

/*!
    @brief Constructor voor de I2CConnection klasse.

    Deze constructor maakt een I2CConnection object aan, opent een I2C verbinding en stelt het slave-adres in.

    @param slaveAddress Het adres van de I2C slave. 
*/
I2CConnection::I2CConnection(uint8_t slaveAddress) : slaveAddress(slaveAddress) {
    slaveOpen = open("/dev/i2c-1", O_RDWR);
    ioctl(slaveOpen, I2C_SLAVE, slaveAddress);
}

/*!
    @brief Destructor voor de I2CConnection klasse.

    Deze destructor sluit de I2C verbinding met de slave.
*/
I2CConnection::~I2CConnection() {
    close(slaveOpen);
}

/*!
    @brief Verstuurt uint8_t data naar de slave over I2C.

    Deze functie verstuurt eem vector van uint8_t bytes naar een specifieke plek in het ontvangstregister van de ontvangende slave.

    @param pos De positie in het ontvangstregister waar de data moet worden opgeslagen.
    @param tekst De uint8_t data die verstuurd moet worden in de vorm van een vector<uint8_t>.
    @param vecSize De grootte van de data vector.
*/
void I2CConnection::sendI2CTo(const uint8_t pos, std::vector<uint8_t> tekst, uint8_t vecSize) {
    tekst.insert(tekst.begin(), pos);
    write(slaveOpen, tekst.data(), static_cast<int>(tekst.size()));
}

/*!
    @brief Ontvangt 6 bytes uint8_t data van de slave over I2C.

    Deze functie leest 6 bytes (uint8_t) data vanaf een specifieke plek in het register in van de slave.

    @param pos De registerindex vanaf waar ingelezen moet worden van de slave.
    @return Een vector<uint8_t> van 6 bytes data die gelezen zijn van de slave. 
*/
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

