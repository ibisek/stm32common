/*
 * i2cf3.h
 *
 *  Created on: Dec 6, 2016
 *      Author: ibisek
 */

#ifndef I2C_H_
#define I2C_H_

#ifdef STM32F042
#include <stm32f0xx.h>
#include <stm32f0xx_i2c.h>
#else
	error "Yet unsupported architecture"
#endif

#include <sys/_stdint.h>

#include "serial.h"

//--------------------------------------------------
//								CONFIGURATION
//--------------------------------------------------

#define I2C_DEBUG 1
#define I2C_TIMEOUT 12345

/**
 * The following values are calculated by "I2C timing configuration tool" downloaded from st.com
 * for specified clock source and I2C clock 100 kHz.
 */

// 8 MHz:
//#define I2C_TIMING 0x00201D2B

// 16 MHz:
//#define I2C_TIMING 0x00503D5A

// 24 MHz:
//#define I2C_TIMING 0x00805C89

// 32 MHz:
#define I2C_TIMING  0x20302E37

// 48 MHz:
//#define I2C_TIMING 0x10805E89 // 100kHz
//#define I2C_TIMING 0x1080D8FF // 50kHz

//--------------------------------------------------
//								PUBLIC INTERFACE
//--------------------------------------------------

void i2c_init(I2C_TypeDef* I2Cx, uint8_t ownAddress, uint8_t enableRxInterrupt);
uint8_t i2c_readByte(I2C_TypeDef* I2Cx, uint8_t i2cAddress, uint8_t registerAddress);
ErrorStatus i2c_readBytes(I2C_TypeDef* I2Cx, uint8_t deviceAddress, uint8_t registerAddress, uint8_t count, uint8_t *dest);
ErrorStatus i2c_writeByte(I2C_TypeDef* I2Cx, uint8_t deviceAddress, uint8_t registerAddress, uint8_t data);
ErrorStatus i2c_writeCommand(I2C_TypeDef* I2Cx, uint8_t deviceAddress, uint8_t command);
uint8_t i2c_readCommandResponse(I2C_TypeDef* I2Cx, uint8_t deviceAddress);
void i2c_printFlags(I2C_TypeDef* I2Cx);

#endif /* I2C_H_ */
