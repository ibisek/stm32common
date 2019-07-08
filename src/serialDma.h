/*
 * serialDma.h
 *
 *  Created on: Jul 25, 2017
 *      Author: ibisek
 *
 * USART2 DMA transfer implementation for F103.
 *
 * DMA channel mapping:
 * 	USART2_RX: channel 6
 * 	USART2_TX: channel 7
 *  @see stm32f10x-reference-manual.pdf -> p. 282, Table 78. Summary of DMA1 requests for each channel
 *
 */

#ifndef SERIALDMA_H_
#define SERIALDMA_H_

#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_exti.h"

#include <string.h>

namespace serialDma {

void init_serial2DmaRx(char* buffer, uint8_t bufferSize);
void init_serial2DmaTx();
void init_serial2DmaInterrupt(uint8_t enableRx, uint8_t enableTx);
void serial2StartDmaTx(char* buffer, uint8_t dataLen);

}

#endif /* SERIALDMA_H_ */


