/*
 * ibiSpi.cpp
 *
 *  Created on: Mar 21, 2019
 *      Author: ibisek
 */

#include <ibiSpi.h>

//uint8_t intEnabled;
void IbiSpi::select() {
	// Read PRIMASK register, check interrupt status before disabling them.
	// Returns 0 if they are enabled, or non-zero if disabled.
//	intEnabled = __get_PRIMASK() != 0;
	__disable_irq();	// ~ cli

	GPIO_WriteBit(ssGpio, ssPin, Bit_RESET);
}

void IbiSpi::unselect() {
	delay_1us();
	GPIO_WriteBit(ssGpio, ssPin, Bit_SET);

	__enable_irq();	// ~ sei
}

uint8_t IbiSpi::transfer(uint8_t data) {
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

void IbiSpi::transferBytes(uint8_t *data, uint8_t dataLen) {
	for (uint8_t i = 0; i < dataLen; i++) {
		transfer(data[i]);
	}
}

// ------------------------- [ PUBLIC INTERFACE ] -------------------------

IbiSpi::IbiSpi(SPI_TypeDef* SPIx, GPIO_TypeDef* SS_GPIOx, uint16_t SS_GPIO_Pin, uint8_t writeBIT = 1) {
	spix = SPIx;
	ssGpio = SS_GPIOx;
	ssPin = SS_GPIO_Pin;
	writeBit = writeBIT;
}

void IbiSpi::init() {
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

uint8_t IbiSpi::readByte(uint8_t addr) {
	select();
	//serial_print_char('A');
	if(writeBit)
		transfer(addr & 0x7F);	// ensure upper bit (write) set to 0
	else
		transfer(addr | 0x80);	// ensure upper bit (read) set to 1

	//serial_print_char('B');
	uint8_t byte = transfer(0x00);
	//serial_print_char('C');
	unselect();

	return byte;
}

void IbiSpi::writeByte(uint8_t addr, uint8_t byte) {
	select();
	//serial_print_char('X');
	if(writeBit)
		transfer(addr | 0x80);	// 0x80 = set upper bit (write) to 1
	else
		transfer(addr & 0x7F);	// 0x7F = set upper bit (write) to 0

	//serial_print_char('Y');
	transfer(byte);
	//serial_print_char('Z');
	unselect();
}

void IbiSpi::readBytes(uint8_t addr, uint8_t *buffer, uint8_t dataLen) {
	select();

	if(writeBit)
		transfer(addr & 0x7F);	// ensure upper bit (write) set to 0
	else
		transfer(addr | 0x80);	// ensure upper bit (read) set to 1

	for(uint8_t i=0; i< dataLen; i++) {
		buffer[i] = transfer(0x00);
	}

	unselect();
}

void IbiSpi::writeBytes(uint8_t addr, uint8_t *data, uint8_t dataLen) {
	select();
	if(writeBit)
		transfer(addr | 0x80);	// 0x80 = set upper bit (write) to 1
	else
		transfer(addr & 0x7F);	// 0x7F = set upper bit (write) to 0

	transferBytes(data, dataLen);
	unselect();
}

uint16_t IbiSpi::readWord(uint8_t addr) {
	select();

	if(writeBit)
		transfer(addr & 0x7FFF);	// ensure upper bit (write) set to 0
	else
		transfer(addr | 0x8000);	// ensure upper bit (read) set to 1

	//	uint16_t word = transfer16(0);	//XXX NEFUNGUJE!!
	uint16_t word = transfer(0x00);
	word |= transfer(0x00) << 8;
	unselect();

	return word;
}

void IbiSpi::writeWord(uint8_t addr, uint16_t word) {
	select();
	if(writeBit)
		transfer(addr | 0x80);	// 0x80 = set upper bit (write) to 1
	else
		transfer(addr & 0x7F);	// 0x7F = set upper bit (write) to 0

	//transfer16(word);		//XXX NEFUNGUJE!!
	transfer(word >> 8);		// upper byte first
	transfer(word & 0xFF);	// lower byte
	unselect();
}
