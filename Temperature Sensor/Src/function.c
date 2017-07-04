/*
 * function.c
 *
 *  Created on: Jul 3, 2017
 *      Author: Prince
 */
#include <stdlib.h>
#include "stm32f1xx_hal.h"
#include "function.h"

UART_HandleTypeDef huart1;

void masterWriteByteWithInterrupt(uint8_t *byte, int dataSize){
	if(byte == 0x0){  // pointer problem
		huart1.Instance->BRR = 480;
	}
	else{
		huart1.Instance->BRR = 180;
	}

	HAL_UART_Transmit_IT(&huart1, byte, dataSize);
}

void oneWireReset(){
	uint8_t reset = 0x00;

	huart1.Instance->BRR = 4000;

	HAL_UART_Transmit(&huart1, &reset, sizeof(reset), 0);
}
/*
void searchCommand(){
	int i;

	for(i=0; i<9; i++){
		if(i<5){
			masterWriteByte(BYTE0);
		}
		else{
			masterWriteByte(BYTE1);
		}
	}
}*/

/*
 * @brief	This function is to send a data (33h) to the
 * 			sensor to obtain the registration number of the
 * 			sensor, if and only if one device is connected
 * 			to the master. Otherwise, error will occur.
 */
void readCommand(){
	uint8_t txData[] = {BYTE0, BYTE0, BYTE1, BYTE1, BYTE0, BYTE0, BYTE1, BYTE1};

	masterWriteByteWithInterrupt(txData, sizeof(txData));
}
