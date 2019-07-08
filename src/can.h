/*
 * can.h
 *
 *  Created on: 08. 12. 2016
 *      Author: ibisek
 */

#ifdef STM32F0xx
#include <stm32f0xx_can.h>
#else
	error "Yet unsupported architecture"
#endif

#include "serial.h"

#ifndef CAN_H_
#define CAN_H_

// ---------------------- [ CAN configuration START ] ----------------------

#define CAN_GPIO GPIOA
#define CAN_RX_PIN GPIO_Pin_11
#define CAN_RX_PIN_SOURCE GPIO_PinSource11
#define CAN_TX_PIN GPIO_Pin_12
#define CAN_TX_PIN_SOURCE GPIO_PinSource12

#define CAN_ENABLE_RX_INT 0

#define CAN_DEBUG 0

// ---------------------- [ CAN configuration END ] ----------------------

#define CAN_BITRATE_125k	1
#define CAN_BITRATE_250k	2
#define CAN_BITRATE_500k	3
#define CAN_BITRATE_1M		4

typedef enum {
	UNKNOWN, OK, FAIL, MB_FULL, PENDING, NO_MB } CanStatus;

void can_init(uint8_t bitrate);
void can_setCanBitrate(CAN_InitTypeDef *CAN_InitStructure, uint8_t bitrate);
void can_printRxMessage(CanRxMsg *rxMsg);

uint8_t can_messageTxPossible();
CanStatus can_sendMessage(CanTxMsg *txMsg);

void can_setRxFilter(uint8_t filterNumber, uint8_t filterFifo, uint16_t canAddr);
void can_disableRxFilter(uint8_t filterNumber, uint8_t filterFifo);

#endif /* CAN_H_ */
