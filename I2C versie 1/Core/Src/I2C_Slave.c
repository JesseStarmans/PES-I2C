/*
 * I2C_Slave.c
 *
 *  Created on: Mar 26, 2024
 *      Author: Jesse
 */

#include "main.h"
#include "I2C_Slave.h"
#include "string.h"

extern I2C_HandleTypeDef hi2c1;

uint8_t TxData[6] = {0x10, 1,2,3,4,5};

#define RXSIZE 6
uint8_t RxData[RXSIZE];
uint8_t rxcount = 0;

int countAddr = 0;
int countrxcplt = 0;
int counterror = 0;

//Transmit toevoegingen
uint8_t dataToSend = 0;

uint8_t bytesTransd = 0;
uint8_t txcount = 0;
uint8_t startPosition = 0;

void process_data(void) {
	//Doe iets met de ingekomen data.
	/*for (int i = 0; i < RXSIZE; i++) {
		RxData[i] = 0;
	}*/
	if (strcmp(RxData, "Aan") == 0) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 1);
	}
	else {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0);
	}
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c) {
	HAL_I2C_EnableListen_IT(hi2c);
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode) {
	if (TransferDirection == I2C_DIRECTION_TRANSMIT) {
		rxcount = 0;
		countAddr++;
		HAL_I2C_Slave_Seq_Receive_IT(hi2c, RxData + rxcount, 1, I2C_FIRST_FRAME);
	}
	else {
		txcount = 0;
		HAL_I2C_Slave_Seq_Transmit_IT(hi2c, TxData+txcount, 1, I2C_FIRST_FRAME);
	}
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c) {
	txcount++;
	HAL_I2C_Slave_Seq_Transmit_IT(hi2c, TxData+txcount, 1, I2C_NEXT_FRAME);
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	rxcount++;
	if (rxcount < RXSIZE) {
		if (rxcount == RXSIZE-1) {
			HAL_I2C_Slave_Seq_Receive_IT(hi2c, RxData + rxcount, 1, I2C_LAST_FRAME);
		}
		else {
			HAL_I2C_Slave_Seq_Receive_IT(hi2c, RxData + rxcount, 1, I2C_NEXT_FRAME);
		}
	}

	if (rxcount == RXSIZE) {
		process_data();
	}
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
	counterror++;
	uint32_t errorcode = HAL_I2C_GetError(hi2c);
	if (errorcode == 4) {
		process_data();
	}
	HAL_I2C_EnableListen_IT(hi2c);
}






