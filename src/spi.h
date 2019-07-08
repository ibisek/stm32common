/*
 * spi.h
 *
 *  Created on: Jan 13, 2017
 *      Author: ibisek
 */

#ifndef SPI_H_
#define SPI_H_

#ifdef STM32F042
#include <stm32f0xx_spi.h>
#include <stm32f0xx_rcc.h>
#else
#ifdef STM32F10x
#include <stm32f10x_spi.h>
#include <stm32f10x_rcc.h>
#else
#error "Yet unsupported architecture"
#endif
#endif

//#define USE_HW_SS 0	// @see http://stm32info.com/en/spi-general-description/

/* ---- CONFIGURATION ---- */

// Slave Select pin (default PA4):
#ifdef STM32F042
#define SS_GPIO GPIOB
#define SS_PIN	GPIO_Pin_1
#endif

#ifdef STM32F10x
// RADIO Slave Select pin for OGN CUBE 1+2 (PB0):
#define SS_GPIO GPIOB
#define SS_PIN	GPIO_Pin_0

// SD CARD Slave Select pin for OGN CUBE 2 (PB3):
//#define SS_GPIO GPIOB
//#define SS_PIN	GPIO_Pin_3
#endif

/* ---- PUBLIC INTERFACE ---- */

void spi_init();

#ifdef STM32F10x
void spi_initF103parametric(SPI_TypeDef* SPIx, GPIO_TypeDef* SS_GPIOx, uint16_t SS_GPIO_Pin, uint8_t writeBIT);
#endif

void spi_select();
void spi_unselect();

uint8_t spi_readByte(uint8_t addr);
void spi_readBytes(uint8_t addr, uint8_t *buffer, uint8_t dataLen);

void spi_writeByte(uint8_t addr, uint8_t byte);
void spi_writeBytes(uint8_t addr, uint8_t *data, uint8_t dataLen);

uint16_t spi_readWord(uint8_t addr);
void spi_writeWord(uint8_t addr, uint16_t word);


/* ---- PRIVATE METHOTS ---- */

uint8_t spi_transfer(uint8_t data) ;
void spi_transferBytes(uint8_t *data, uint8_t dataLen);

uint16_t spi_transfer16(uint16_t data);


#endif /* SPI_H_ */
