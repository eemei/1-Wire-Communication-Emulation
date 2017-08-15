/*
 * state_machine.c
 *
 *  Created on: Jul 3, 2017
 *      Author: Prince
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "state_machine.h"
#include "event.h"
#include "function.h"
#include "uart.h"

//extern TIM_HandleTypeDef htim2;
//extern UART_HandleTypeDef huart1;

BitSearchingInfo bsi = {IDLE_STATE};
//uint8_t reset = 0xE0;
uint8_t presencePulse;
uint8_t responsePulse[100] = {0};
char regisNum[64] = {0};
volatile int i, j=0;

void bitSearchingFSM(Event evt){
	switch(bsi.state){
	case IDLE_STATE:
		if(evt == START_EVT){
			HAL_HalfDuplex_EnableTxRx();
			timerStart();
			masterTransmitReceive(RECEIVE, &presencePulse, sizeof(presencePulse));
			resetPulse();
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
			bsi.state = FINISH_INIT_STATE;
		}
		else
			bsi.state = IDLE_STATE;
		break;
	case FINISH_INIT_STATE:
		if(evt == TIMEOUT_EVT){
			timerStop();
			masterTransmitReceive(RECEIVE, responsePulse, sizeof(responsePulse));
			//searchROM();
			readROM();
			bsi.state = COMMAND_STATE;
		}
    else
      bsi.state = IDLE_STATE;
		break;
	case COMMAND_STATE:
		if(evt == UART_TX_CPL_EVT){
			masterReadSlot();
			bsi.state = READ_SLOT_STATE;
		}
		else
			bsi.state = IDLE_STATE;
		break;
	case READ_SLOT_STATE:
		for(i=71; i>7; i--){
			if(responsePulse[i] == 0xf8){
				regisNum[j] = '0';
			}
			else if(responsePulse[i] == 0xff){
				regisNum[j] = '1';
			}
			else{
				break;
			}
			j++;
		}
		printf("ID number: %s\n", regisNum);
		break;
	default:
		bsi.state = RESET_STATE;
	}
}
