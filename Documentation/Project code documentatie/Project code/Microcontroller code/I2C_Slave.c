/*
 * I2C_Slave.c
 *
 *  Created on: Mar 26, 2024
 *      Author: Jesse
 */

#include "main.h"										///Include de main
#include "I2C_Slave.h"									///Include de header van dit bestand
#include "string.h"										///Include string.h voor het vergelijken van strings

uint8_t I2C_REGISTERS[10] = {0,0,0,0,0,0,0,0,0,0};		///Register om data in te zetten
extern I2C_HandleTypeDef hi2c1;							///Om te zorgen dat de code de I2C kan gebruiken

#define RXSIZE 11										///Grootte definieren van de inlees array
uint8_t RxData[RXSIZE];									///Inlees array waar data in ontvangen wordt
uint8_t rxcount = 0;									///Variabele met het aantal ingelezen bytes

int countAddr = 0;										///Variabele die telt hoe vaak het slave-adres aangeroepen is
int counterror = 0;										///Variabale die telt hoevaak er een error is geweest

//Transmit variabelen
uint8_t txcount = 0;									///Variabele met het aantal verstuurde bytes
uint8_t startPosition = 0;								///Variabele vanaf waar er in het register gelezen moet worden

/*!Deze functie wordt aangeroepen als de master klaar is met data verzenden*/
void process_data(void) {
	//Doe iets met de ingekomen data.
	/*if (strcmp(RxData, "Aan") == 0) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 1);
	}
	else {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0);
	}*/

	int startREG = RxData[0];							//Maak een variabele met het eerste getal uit de inlees array
	int numREG = rxcount-1;								//Maak een variabele met de hoeveelheid ingelezen bytes - 1 (startREG)
	int endREG = startREG+numREG-1;						//Maak een variabele met de positie van de laatste byte in het register
	if (endREG>9){										//Check of de positie van het eindregister groter is dan de grootte van het register
		Error_Handler();								//Roep een error handler aan
	}
	int indx = 1;										//Maak een variabele voor de index
	for (int i = 0; i < numREG; i++) {					//Loop door de hoeveelheid registers die ingelezen zijn
		I2C_REGISTERS[startREG++] = RxData[indx++];		//Schrijf de data uit de inlees array, in de I2C_REGISTERS (op de juiste plek)
	}

	if (RxData[1] == 'A' && RxData[2] == 'a' && RxData[3] == 'n') {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 1);
	}
	else if (RxData[1] == 'U' && RxData[2] == 'i' && RxData[3] == 't'){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0);
	}

}

/*!Deze Callback wordt aangeroepen als de enable lister IT functie is afgemaakt*/
void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c) {
	HAL_I2C_EnableListen_IT(hi2c);						//Zet enable listen weer aan zodat I2C blijft werken
}

/*!Deze Callback wordt aangeroepen als het verstuurde adres het adres van de slave is*/
void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode) {
	if (TransferDirection == I2C_DIRECTION_TRANSMIT) {	//Check of er data vanaf de master naar de slave gestuurd wordt
		rxcount = 0;									//Reset hoeveel data er ontvangen is
		countAddr++;									//Verhoog de variabele voor het aanroepen van de slave
		HAL_I2C_Slave_Seq_Receive_IT(hi2c, RxData + rxcount, 1, I2C_FIRST_FRAME);
														//Lees achtereenvolgend data in en maak van deze eerste byte
														//de eerste frame (I2C_FIRST_FRAME)
	}
	else {												//Als er data vanaf de slave naar de master verwacht wordt
		txcount = 0;									//Reset hoeveel data er verstuurd is
		startPosition = RxData[0];						//Zet de startpositie gelijk aan het eerst onvangen getal
		HAL_I2C_Slave_Seq_Transmit_IT(hi2c, I2C_REGISTERS+startPosition+txcount, 1, I2C_FIRST_FRAME);
														//Verstuur achtereenvolgend data naar de master en maak van
														//de eerste byte de eerste frame (I2C_FIRST_FRAME)
	}
}

/*!Deze Callback wordt aangeroepen zodra een receive IT functie afgemaakt is*/
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	rxcount++;											//Verhoog de ingelezen waarde teller met 1
	if (rxcount < RXSIZE) {								//Check of de ingelezen waarde teller nog kleiner is dan de max
														//grootte van de inlees array
		if (rxcount == RXSIZE-1) {						//Check of de ingelezen waarde teller precies 1 kleiner is dan de
														//max grootte van de inlees array
			HAL_I2C_Slave_Seq_Receive_IT(hi2c, RxData + rxcount, 1, I2C_LAST_FRAME);
														//Lees de volgende frame als de laaste frame in
		}
		else {											//Als rxcount kleiner is dan RXSIZE - 1
			HAL_I2C_Slave_Seq_Receive_IT(hi2c, RxData + rxcount, 1, I2C_NEXT_FRAME);
														//Lees de volgende verstuurde frame als de volgende frame in de rij frames in
		}
	}

	if (rxcount == RXSIZE) {							//Check of de ingelezen waarde teller gelijk is aan de grootte
														//van de inlees array
		process_data();									//Roep de functie process_data() aan
	}
}

/*!Deze functie wordt aangeroepen zodra een transmit IT functie afgemaakt is*/
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c) {
	txcount++;											//Verhoog de verstuurd waarde teller met 1
	HAL_I2C_Slave_Seq_Transmit_IT(hi2c, I2C_REGISTERS+startPosition+txcount, 1, I2C_NEXT_FRAME);
														//Verstuur de volgende frame als de volgende frame in de rij frames
}

/*!Deze functie wordt aangeroepen als er een error optreed in het I2C gedeelte*/
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
	counterror++;										//Verhoog de hoeveelheid errors met 1
	uint32_t errorcode = HAL_I2C_GetError(hi2c);		//Maak een variabele en vul deze met de I2C error code die is afgegeven
	if (errorcode == 4) {								//Check of de error code gelijk is aan 4 (ingelezen hoeveelheid bytes is kleiner
														//dan de maximale grootte die ingelezen kan worden = master is eerder gestopt met zenden)
		process_data();									//Roep de functie process_data() aan
	}
	HAL_I2C_EnableListen_IT(hi2c);						//Laat de I2C weer luisteren naar de lijn
}
