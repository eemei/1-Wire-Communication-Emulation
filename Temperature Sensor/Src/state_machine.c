/*
 * state_machine.c
 *
 *  Created on: Jul 3, 2017
 *      Author: Prince
 */
#include <stdlib.h>
#include "stm32f1xx_hal.h"
#include "state_machine.h"
#include "function.h"

//UART_HandleTypeDef huart1;

int fsm(State state){
	switch(state){
	case INIT:
		oneWireReset();
		state = SEARCH;
		break;
	case SEARCH:
		readCommand();
		state = SEARCH;
		break;
	default:
		state = INIT;
	}
	return state;
}
