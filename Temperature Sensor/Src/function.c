/*
 * function.c
 *
 *  Created on: Jul 3, 2017
 *      Author: Prince
 */
#include <stdlib.h>
#include "stm32f1xx_hal.h"
#include "function.h"

extern UART_HandleTypeDef huart1;

void timerStart(TIM_HandleTypeDef *htim){
	HAL_GPIO_WritePin(amberLed_GPIO_Port, amberLed_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(amberLed_GPIO_Port, amberLed_Pin, GPIO_PIN_RESET);
	htim->Instance->CNT = 0;
	__HAL_TIM_CLEAR_FLAG(htim, TIM_FLAG_UPDATE);
	HAL_TIM_Base_Start_IT(htim);
}

void timerStop(TIM_HandleTypeDef *htim){
	HAL_GPIO_WritePin(amberLed_GPIO_Port, amberLed_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(amberLed_GPIO_Port, amberLed_Pin, GPIO_PIN_RESET);
	HAL_TIM_Base_Stop_IT(htim);
}

void masterWriteByteWithInterrupt(uint8_t *txData, int dataSize){
	huart1.Instance->BRR = 480;

	HAL_UART_Transmit_IT(&huart1, txData, dataSize);
}

void masterReadSlot(){
	/*uint8_t txData[] = {BYTE1, BYTE1, BYTE0, \
						BYTE1, BYTE1, BYTE1, \
						BYTE1, BYTE1, BYTE0, \
						BYTE1, BYTE1, BYTE1, \
						BYTE1, BYTE1, BYTE0, \
						BYTE1, BYTE1, BYTE1, \
						BYTE1, BYTE1, BYTE0, \
						BYTE1, BYTE1, BYTE0};*/
	int i=0;
	uint8_t txData[64] = {0};

	for(i=0; i<64; i++){
		txData[i] = BYTE1;
	}

	huart1.Instance->BRR = 480;

	HAL_UART_Transmit_IT(&huart1, txData, sizeof(txData));
}

void resetPulse(){
	uint8_t reset = BYTE0;

	huart1.Instance->BRR = 6240;

	HAL_UART_Transmit_IT(&huart1, &reset, sizeof(reset));
}

void presencePulseDetect(){
	uint8_t presencePulse[2] = {0};

	huart1.Instance->BRR = 6240;

	HAL_UART_Receive_IT(&huart1, presencePulse, sizeof(presencePulse));
}

/*
 * @brief	This function is to send a data (F0h) to the
 * 			sensor to search the registration number of
 * 			the sensor bit by bit, till the 64th bit of
 * 			the registration number.
 */
void searchROM(){
	uint8_t searchCommand[] = {BYTE0, BYTE0, BYTE0, BYTE0, BYTE1, BYTE1, BYTE1, BYTE1};

	masterWriteByteWithInterrupt(searchCommand, sizeof(searchCommand));
}

/*
 * @brief	This function is to send a data (33h) to the
 * 			sensor to obtain the registration number of the
 * 			sensor, if and only if one device is connected
 * 			to the master. Otherwise, error will occur.
 */
void readROM(){
	uint8_t readCommand[] = {BYTE1, BYTE1, BYTE0, BYTE0, BYTE1, BYTE1, BYTE0, BYTE0};

	masterWriteByteWithInterrupt(readCommand, sizeof(readCommand));
}

void skipROM(){
	uint8_t skipCommand[] = {BYTE0, BYTE0, BYTE1, BYTE1, BYTE0, BYTE0, BYTE1, BYTE1};

	masterWriteByteWithInterrupt(skipCommand, sizeof(skipCommand));
}

void readPowerSupply(){
	uint8_t readPowSupCommand[] = {BYTE0, BYTE0, BYTE1, BYTE0, BYTE1, BYTE1, BYTE0, BYTE1};

	masterWriteByteWithInterrupt(readPowSupCommand, sizeof(readPowSupCommand));
}
