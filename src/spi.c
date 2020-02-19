/*
 * spi.c
 *
 *  Created on: Jan 17, 2017
 *      Author: ibisek
 *
 *  ADDRESS byte:
 *  -> bit 7: 1=WR 0=RD	(normal situation)
 *  -> bit 7: 0=WR 1=RD	(MPU6500)
 */

#include "spi.h"

#include "serial.h"
#include "ibitime.h"

#define SPIx_TX_DMA_CHANNEL              DMA1_Channel3
#define SPIx_TX_DMA_FLAG_TC              DMA1_FLAG_TC3
#define SPIx_RX_DMA_CHANNEL              DMA1_Channel2
#define SPIx_RX_DMA_FLAG_TC              DMA1_FLAG_TC2

#define DMA_SPI_RX_BUF_SIZE 1
#define DMA_SPI_TX_BUF_SIZE 1
uint8_t dmaSpiRxBuf[DMA_SPI_RX_BUF_SIZE];
uint8_t dmaSpiTxBuf[DMA_SPI_TX_BUF_SIZE];

SPI_TypeDef* spix = SPI1;
GPIO_TypeDef* ssGpio = SS_GPIO;
uint16_t ssPin = SS_PIN;
uint8_t writeBit = 1;

#ifdef STM32F10x
/**
 * Initialises SPI1/2.
 * @param SPIx SPI1/SPI2
 * @param SS_GPIOx
 * @param SS_GPIO_Pin
 * @param writeBIT
 */
//void spi_initF103parametric(SPI_TypeDef* SPIx, GPIO_TypeDef* SS_GPIOx, uint16_t SS_GPIO_Pin, uint8_t writeBIT) {		// C
void spi_initF103parametric(SPI_TypeDef* SPIx, GPIO_TypeDef* SS_GPIOx, uint16_t SS_GPIO_Pin, uint8_t writeBIT = 1) {	// C++
	spix = SPIx;
	ssGpio = SS_GPIOx;
	ssPin = SS_GPIO_Pin;
	writeBit = writeBIT;

	SPI_I2S_DeInit(spix);

	GPIO_TypeDef* SPIx_GPIO = GPIOA;
	if(spix == SPI2) SPIx_GPIO = GPIOB;

	// GPIO configuration:
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // Enable GPIO A bank clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // Enable GPIO B bank clock

	GPIO_InitTypeDef GPIO_InitStructure;

	// MISO, MOSI, SCK pins:
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	if(spix == SPI1) GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	else GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(SPIx_GPIO, &GPIO_InitStructure);

	// SS pin:
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = ssPin;
	GPIO_Init(ssGpio, &GPIO_InitStructure);

	// Enable SPI clock:
	if(spix == SPI1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	else RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	SPI_InitTypeDef SPI_InitStructure;
	SPI_StructInit(&SPI_InitStructure);
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; // 32MHz/32 = 1MHz.
	SPI_Init(spix, &SPI_InitStructure);

	// enable SPI
	SPI_Cmd(spix, ENABLE);

	//SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
	SPI_CalculateCRC(spix, DISABLE);

	// DISABLE default NSS output:
	SPI_SSOutputCmd(spix, DISABLE); // enable NSS output for master mode
//	SPI_NSSInternalSoftwareConfig(spix, SPI_NSSInternalSoft_Reset);
}

/**
 * Initialises SPI1.
 */
void spi_initF103() {
	SPI_I2S_DeInit(SPI1);

	// GPIO configuration:
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // Enable GPIO A bank clock
	if(ssGpio == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	// MISO, MOSI, SCK pins:
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// SS pin:
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = ssPin;
	GPIO_Init(ssGpio, &GPIO_InitStructure);

	// Enable SPI clock:
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	SPI_InitTypeDef SPI_InitStructure;
	SPI_StructInit(&SPI_InitStructure);
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; // 32MHz/32 = 1MHz.
	SPI_Init(SPI1, &SPI_InitStructure);

	// enable SPI
	SPI_Cmd(SPI1, ENABLE);

	//SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
	SPI_CalculateCRC(SPI1, DISABLE);

	SPI_SSOutputCmd(SPI1, ENABLE); // enable NSS output for master mode
	SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
}
#else
void spi_initOtherArchitectures() {
	// GPIO configuration:
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);// Enable GPIO A bank clock
	if(SS_GPIO == GPIOB) RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	// MISO, MOSI, SCK pins:
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Alternate function mapping:
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_0);// alternate function SPI1_SCK
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_0);// alternate function SPI1_MISO
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_0);// alternate function SPI1_MOSI

	// SS pin:
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = SS_PIN;
	GPIO_Init(SS_GPIO, &GPIO_InitStructure);

	SPI_I2S_DeInit(SPI1);

	// Enable SPI clock:
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	SPI_InitTypeDef SPI_InitStructure;
	SPI_StructInit(&SPI_InitStructure);
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;// 32MHz/32 = 1MHz.
	SPI_Init(SPI1, &SPI_InitStructure);

	// enable SPI
	SPI_Cmd(SPI1, ENABLE);

	//SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
	SPI_CalculateCRC(SPI1, DISABLE);

	SPI_SSOutputCmd(SPI1, ENABLE);// enable NSS output for master mode
	SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
}
#endif

void spi_init() {
#ifdef STM32F10x
	spi_initF103();
#else
	spi_initOtherArchitectures();
#endif
}

//uint8_t intEnabled;
void spi_select() {
	// Read PRIMASK register, check interrupt status before disabling them.
	// Returns 0 if they are enabled, or non-zero if disabled.
//	intEnabled = __get_PRIMASK() != 0;
//	__disable_irq();	// ~ cli

	GPIO_WriteBit(ssGpio, ssPin, Bit_RESET);
}

void spi_unselect() {
	delay_1us();
	GPIO_WriteBit(ssGpio, ssPin, Bit_SET);

//	__enable_irq();	// ~ sei
}

uint8_t spi_transfer(uint8_t data) {
//	while (SPI_I2S_GetFlagStatus(spix, SPI_I2S_FLAG_TXE) == RESET);
//	while ((spix->SR & SPI_I2S_FLAG_TXE) == 0); // wait for TX buffer empty before pushing in next byte

#ifdef STM32F10x
	SPI_I2S_SendData(spix, data);
#else
	SPI_SendData8(spix, data);
#endif

	while ((spix->SR & SPI_I2S_FLAG_BSY) != 0) {
		delay_1us(); // wait for TX done
	}

	delay_1us();

	uint8_t b;
#ifdef STM32F10x
	b = SPI_I2S_ReceiveData(spix);
#else
	b = SPI_ReceiveData8(spix);
#endif

	return b;
}

void spi_transferBytes(uint8_t *data, uint8_t dataLen) {
	for (uint8_t i = 0; i < dataLen; i++) {
		spi_transfer(data[i]);
	}
}

//uint16_t spi_transfer16(uint16_t data) {
//	SPI_I2S_SendData16(spix, data);
//
//	while (SPI_I2S_GetFlagStatus(spix, SPI_I2S_FLAG_BSY) == SET)
//		;	// wait for TX done
//	while (SPI_I2S_GetFlagStatus(spix, SPI_I2S_FLAG_RXNE) == SET)
//		;	// wait for RX
//
//	return SPI_I2S_ReceiveData16(spix);
//}

uint8_t spi_readByte(uint8_t addr) {
	spi_select();
	//serial_print_char('A');
	if(writeBit)
		spi_transfer(addr & 0x7F);	// ensure upper bit (write) set to 0
	else
		spi_transfer(addr | 0x80);	// ensure upper bit (read) set to 1

	//serial_print_char('B');
	uint8_t byte = spi_transfer(0x00);
	//serial_print_char('C');
	spi_unselect();

	return byte;
}

void spi_writeByte(uint8_t addr, uint8_t byte) {
	spi_select();
	//serial_print_char('X');
	if(writeBit)
		spi_transfer(addr | 0x80);	// 0x80 = set upper bit (write) to 1
	else
		spi_transfer(addr & 0x7F);	// 0x7F = set upper bit (write) to 0

	//serial_print_char('Y');
	spi_transfer(byte);
	//serial_print_char('Z');
	spi_unselect();
}

void spi_readBytes(uint8_t addr, uint8_t *buffer, uint8_t dataLen) {
	spi_select();

	if(writeBit)
		spi_transfer(addr & 0x7F);	// ensure upper bit (write) set to 0
	else
		spi_transfer(addr | 0x80);	// ensure upper bit (read) set to 1

	for(uint8_t i=0; i< dataLen; i++) {
		buffer[i] = spi_transfer(0x00);
	}

	spi_unselect();
}

void spi_writeBytes(uint8_t addr, uint8_t *data, uint8_t dataLen) {
	spi_select();
	if(writeBit)
		spi_transfer(addr | 0x80);	// 0x80 = set upper bit (write) to 1
	else
		spi_transfer(addr & 0x7F);	// 0x7F = set upper bit (write) to 0

	spi_transferBytes(data, dataLen);
	spi_unselect();
}

uint16_t spi_readWord(uint8_t addr) {
	spi_select();

	if(writeBit)
		spi_transfer(addr & 0x7FFF);	// ensure upper bit (write) set to 0
	else
		spi_transfer(addr | 0x8000);	// ensure upper bit (read) set to 1

	//	uint16_t word = spi_transfer16(0);	//XXX NEFUNGUJE!!
	uint16_t word = spi_transfer(0x00);
	word |= spi_transfer(0x00) << 8;
	spi_unselect();

	return word;
}

void spi_writeWord(uint8_t addr, uint16_t word) {
	spi_select();
	if(writeBit)
		spi_transfer(addr | 0x80);	// 0x80 = set upper bit (write) to 1
	else
		spi_transfer(addr & 0x7F);	// 0x7F = set upper bit (write) to 0

	//spi_transfer16(word);		//XXX NEFUNGUJE!!
	spi_transfer(word >> 8);		// upper byte first
	spi_transfer(word & 0xFF);	// lower byte
	spi_unselect();
}

