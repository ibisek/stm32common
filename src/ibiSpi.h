/*
 * ibiSpi.h
 *
 *  Created on: Mar 21, 2019
 *      Author: ibisek
 */

#ifndef IBISPI_H_
#define IBISPI_H_

#include "ibitime.h"

#ifdef STM32F10x
#include <stm32f10x_spi.h>
#include <stm32f10x_rcc.h>
#else
#ifdef STM32L1
#include <stm32l1xx_spi.h>
#include <stm32l1xx_rcc.h>
#else
#error "Yet unsupported architecture"
#endif
#endif

class IbiSpi {

private:
	SPI_TypeDef* spix;
	GPIO_TypeDef* ssGpio;
	uint16_t ssPin;
	uint8_t writeBit = 1;

	void select();
	void unselect();
	uint8_t transfer(uint8_t data);
	void transferBytes(uint8_t *data, uint8_t dataLen);

public:
	IbiSpi(SPI_TypeDef* SPIx, GPIO_TypeDef* SS_GPIOx, uint16_t SS_GPIO_Pin, uint8_t writeBIT);
//	virtual ~IbiSpi2();

	void init();

	uint8_t readByte(uint8_t addr);
	void writeByte(uint8_t addr, uint8_t byte);
	void readBytes(uint8_t addr, uint8_t *buffer, uint8_t dataLen);
	void writeBytes(uint8_t addr, uint8_t *data, uint8_t dataLen);
	uint16_t readWord(uint8_t addr);
	void writeWord(uint8_t addr, uint16_t word);
};

#endif /* IBISPI_H_ */
