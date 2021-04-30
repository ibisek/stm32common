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

#include "serialDma.h"
namespace serialDma {

//-----------------------------------------------------
//							LOCAL CONSTANTS & VARIABLES
//-----------------------------------------------------

const uint16_t DMA_TX_BUFFER_SIZE = 1024;
char dmaTxBuffer[DMA_TX_BUFFER_SIZE];
volatile uint8_t dmaTxBusy = 0;

uint16_t dmaTxBufferIdx = 0;
uint16_t dmaStartIdx = 0;
uint16_t dmaEndIdx = 0;


//-----------------------------------------------------
//											SERIAL IRQ
//-----------------------------------------------------

/* USART2 TX DMA IRQ */
extern "C" void DMA1_Channel7_IRQHandler(void);	// C++ stuff necessity
void DMA1_Channel7_IRQHandler() {
	//Test on DMA1 Channel7 Transfer Complete interrupt
	if (DMA_GetITStatus(DMA1_IT_TC7)) {
		//Clear DMA1 Channel7 Half Transfer, Transfer Complete and Global interrupt pending bits
		DMA_ClearITPendingBit(DMA1_IT_GL7);
		DMA_Cmd(DMA1_Channel7, DISABLE);// disable IRQ so it can be enabled at the next transfer

		dmaTxBusy = 0;
	}
}

//-----------------------------------------------------
//							STUFF
//-----------------------------------------------------

void init_serial2DmaRx(char* rxBuffer, uint8_t rxBufferSize) {
	//enable DMA1 clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Channel6);

	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // Receive
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) rxBuffer;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_BufferSize = rxBufferSize;

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &USART2->DR;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;

	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel6, &DMA_InitStructure);

	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);// Enable the USART Rx DMA request
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);	// Enable DMA Stream Half Transfer and Transfer Complete interrupt
	DMA_Cmd(DMA1_Channel6, ENABLE);	// Enable the DMA RX Stream
}

void init_serial2DmaTx() {
	//enable DMA1 clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Channel7);

	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; // Transmit
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) dmaTxBuffer;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_BufferSize = DMA_TX_BUFFER_SIZE;

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &USART2->DR;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;

	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel7, &DMA_InitStructure);

	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);// Enable the USART Tx DMA request
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);	// Enable DMA Stream Half Transfer and Transfer Complete interrupt
	//DMA_Cmd(DMA1_Channel7, ENABLE);	// Enable the DMA TX Stream
}

/**
 * @param enableRx 1 to enable RX
 * @param enableTx 1 to enable TX
 */
void init_serial2DmaInterrupt(uint8_t enableRx, uint8_t enableTx) {
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	if (enableRx) {
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 14;
		NVIC_Init(&NVIC_InitStructure);
	}

	if (enableTx) {
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 15;
		NVIC_Init(&NVIC_InitStructure);
	}
}

void serial2StartDmaTx(char* buffer, uint16_t dataLen) {
	if (!dmaTxBusy && dmaEndIdx > 0) {	// if the previous DMA transfer has finished and the buffer has not been shifted yet
		uint16_t len = dmaEndIdx - dmaTxBufferIdx;
		memmove(dmaTxBuffer, dmaTxBuffer+dmaEndIdx, len);
		dmaTxBufferIdx = len;
		dmaEndIdx = 0;
	}

	uint16_t remaining = DMA_TX_BUFFER_SIZE - dmaTxBufferIdx;
	if (dataLen > remaining) dataLen = remaining;	// do not overflow the buffer

	memcpy(dmaTxBuffer+dmaTxBufferIdx, buffer, dataLen);	// append the new buffer data into the TX buffer
	dmaTxBufferIdx += dataLen;

	if (!dmaTxBusy) {
		dmaEndIdx = dmaTxBufferIdx;

		DMA1_Channel7->CMAR = (uint32_t) dmaTxBuffer;
		DMA1_Channel7->CNDTR = dmaEndIdx;
		DMA_Cmd(DMA1_Channel7, ENABLE);	// Enable the DMA TX Stream
		dmaTxBusy = 1;
	}
}

} // ~ namespace

