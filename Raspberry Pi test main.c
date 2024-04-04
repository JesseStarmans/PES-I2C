#include <stdio.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <cstdint>
#include <malloc.h>

#define SlaveAdd 0x10
#define SlaveAdd2 0x20

int fd = open("/dev/i2c-1", O_RDWR);			//Maakt een I2C verbinding aan
int fd2 = open("/dev/i2c-1", O_RDWR);			//Maakt een I2C verbinding aan

void delaySec(int seconde) {
	usleep(seconde * 1000000);
}

/*Deze functie regelt het sturen van een array naar een specifieke positie in het I2C_REGISTER op de microcontrollers*/
void writeTo(uint8_t startPos, uint8_t array[], int sizeArray, int fdSlave) {
	uint8_t* toSend = (uint8_t*) malloc((sizeof(uint8_t)*sizeArray)+sizeof(uint8_t));
												//Maakt een array aan (afhankelijk van de grootte van de meegegeven array)
												//"(sizeof(uint8_t)*sizeArray)" om te zorgen dat de hele array er in opgeslagen kan worden
												//"+sizeof(uint8_t)" zodat de startPositie ook nog meegegeven kan worden
												
	toSend[0] = startPos;						//Zet het eerste te sturen getal gelijk aan de startPositie
	for (int i = 1; i < sizeArray; i++) {		//Loop door de volledige meegegeven array
		toSend[i] = array[i-1];					//Vul de te verzenden array met het getal in de meegegeven array - 1
		if (i == sizeArray-1) {					//Check of de teller bij de een naar laatste plek is 
			toSend[i+1] = array[i];				//Vul de laatste te verzenden array met het meegegeven array getal
		}
	}
	write(fdSlave, toSend, sizeArray+sizeof(uint8_t)); 
												//Stuur de toSend array over I2C naar de meegegeven slave
}

/*Deze functie regelt het inlezen van 6 bytes, beginnend vanaf de startPositie, uit de I2C_REGISTERS op de microcontrollers*/
void read6From(uint8_t startPos, uint8_t* ingelezen, int fdSlave) {
	write(fdSlave, &startPos, 1);				//Stuur naar de slave op welke plek in het register gekeken moet worden
	usleep(1000);								//Wacht 1 miliseconde (1000 microsec = 1 milisec)
	read(fdSlave, ingelezen, 6);				//Lees 6 bytes van de meegegeven slave in
}

/*Deze functie bind de slave adressen aan de slave variabelen*/
void initI2C(){
	ioctl(fd, I2C_SLAVE, SlaveAdd);				//Initialiseer de eerste slave op het eerste slaveAdres
	ioctl(fd2, I2C_SLAVE, SlaveAdd2);			//Initialiseer de tweede slave op het tweede slaveAdres
}

int main (void) {
	initI2C();									//Roep de functie initI2C() aan
	
	uint8_t sendBuf[3] = {65, 97, 110};			//Maak een te versturen array aan						//Ascii voor: "Aan"			
	uint8_t sendBuf2[4] = {85, 105, 116};		//Maak een te versturen array aan						//Ascii voor: "Uit"
	uint8_t recBuf[6] = {0};					//Maak een array aan waarin de ontvangen bytes komen van slave 1
	uint8_t recBuf2[6] = {0};					//Maak een array aan waarin de ontvangen bytes komen van slave 2
	
	for (int i = 0; i < 10; i++) {
		if (i % 2 == 0) {						//Alterneer tussen aan en uitzetten op basis van of i een even getal is of niet
			writeTo(5, sendBuf, sizeof(sendBuf)/sizeof(sendBuf[0]), fd);		//Roep de functie writeTo() aan en schrijf "Aan" naar de eerste slave
			
			read6From(5, recBuf, fd);											//Roep de functie read6From() aan en lees 6 bytes van slave 1 in
			for (int j = 0; j < 6; j++) {										//Loop door de volledige ontvangen bytes array
				//Slave 1
				if (recBuf[j] != 0) {											//Check of de ontvangen byte van slave 1 niet 0 is
					printf("%de ingelezen waarde uit het REGISTER van slave 1: %d = %c\n\r", j+1 ,recBuf[j], (char)recBuf[j]);
				}
			}
			printf("\n");
			
			delaySec(1);														//Wacht 1 seconde
			
			writeTo(5, sendBuf, sizeof(sendBuf)/sizeof(sendBuf[0]), fd2);		//Roep de functie writeTo() aan en schrijf "Aan" naar de tweede slave
		
			read6From(5, recBuf2, fd2);											//Roep de functie read6From() aan en lees 6 bytes van slave 2 in

			for (int j = 0; j < 6; j++) {										//Loop door de volledige ontvangen bytes array
				//Slave 2
				if (recBuf2[j] != 0) {											//Check of de ontvangen byte van slave 2 niet 0 is
					printf("%de ingelezen waarde uit het REGISTER van slave 2: %d = %c\n\r", j+1 ,recBuf2[j], (char)recBuf2[j]);
				}
			}
			printf("\n");
		}
		else {
			writeTo(5, sendBuf2, sizeof(sendBuf2)/sizeof(sendBuf2[0]), fd);		//Roep de functie writeTo() aan en schrijf "Aan" naar de eerste slave
			
			read6From(5, recBuf, fd);											//Roep de functie read6From() aan en lees 6 bytes van slave 1 in
			for (int j = 0; j < 6; j++) {										//Loop door de volledige ontvangen bytes array
				//Slave 1
				if (recBuf[j] != 0) {											//Check of de ontvangen byte van slave 1 niet 0 is
					printf("%de ingelezen waarde uit het REGISTER van slave 1: %d = %c\n\r", j+1 ,recBuf[j], (char)recBuf[j]);
				}
			}
			printf("\n");
			
			delaySec(1);														//Wacht 1 seconde
			
			writeTo(5, sendBuf2, sizeof(sendBuf2)/sizeof(sendBuf2[0]), fd2);	//Roep de functie writeTo() aan en schrijf "Aan" naar de tweede slave
		
			read6From(5, recBuf2, fd2);											//Roep de functie read6From() aan en lees 6 bytes van slave 2 in

			for (int j = 0; j < 6; j++) {										//Loop door de volledige ontvangen bytes array
				//Slave 2
				if (recBuf2[j] != 0) {											//Check of de ontvangen byte van slave 2 niet 0 is
					printf("%de ingelezen waarde uit het REGISTER van slave 2: %d = %c\n\r", j+1 ,recBuf2[j], (char)recBuf2[j]);
				}
			}
			printf("\n");
		}
		delaySec(1);															//Wacht 1 seconde
	}
}
