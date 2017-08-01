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
#include "event.h"
#include "uart.h"
#include "callback.h"

extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;

BitSearchingInfo bsi = {IDLE_STATE};
uint8_t reset = 0xE0;
uint8_t presencePulse[] = {0};
uint8_t responsePulse[100] = {0};
volatile int i;

void bitSearchingFSM(Event evt){
	switch(bsi.state){
	case IDLE_STATE:
		if(evt == START_EVT){
			//HAL_HalfDuplex_EnableTxRx(&huart1);
			timerStart(&htim2);
			huart1.Instance->BRR = 6240;
			HAL_UART_Receive_IT(&huart1, presencePulse, sizeof(presencePulse));
		    HAL_UART_Transmit_IT(&huart1, &reset, sizeof(reset));
			bsi.state = RESET_STATE;
		}
		break;
	case RESET_STATE:
		if(evt == UART_TX_CPL_EVT){
			bsi.state = RESPONSE_STATE;
		}
		else{
			bsi.state = IDLE_STATE;
		}
		break;
	case RESPONSE_STATE:
		if(evt == UART_RX_CPL_EVT){
			bsi.state = WAIT_STATE;
		}
		else
			bsi.state = IDLE_STATE;
		break;
	case WAIT_STATE:
		if(evt == TIMEOUT_EVT){
			timerStop(&htim2);
			HAL_UART_Receive_IT(&huart1, responsePulse, sizeof(responsePulse));
			//searchROM();
			readROM();
			bsi.state = COMMAND_STATE;
		}
		break;
	case COMMAND_STATE:
		if(evt == UART_TX_CPL_EVT){
			masterReadSlot();
		}
		for(i=71; i>7; i--){
			if(responsePulse[i] == 0xf8){
				printf("0");
			}
			else{
				printf("1");
			}
		}
		bsi.state = IDLE_STATE;
		break;
	default:
		bsi.state = RESET_STATE;
	}
}
