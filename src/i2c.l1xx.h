/*
 * i2c.l1xx.h
 *
 *  Created on: Dec. 25, 2020
 *      Author: ibisek
 */

#ifndef I2C_L1xx_H_
#define I2C_L1xx_H_

#include <stm32l1xx.h>
#include <stm32l1xx_i2c.h>
#include <stm32l1xx_rcc.h>

#include <sys/_stdint.h>

#include "serial.h"

//--------------------------------------------------
//								CONFIGURATION
//--------------------------------------------------

#define I2C_DEBUG 1
#define I2C_TIMEOUT 12345

//--------------------------------------------------
//								PRIVATE STUFF
//--------------------------------------------------

void i2c__sendStartCondition(I2C_TypeDef* I2Cx, uint8_t deviceAddress);
void i2c__sendStopCondition(I2C_TypeDef* I2Cx);

//--------------------------------------------------
//								PUBLIC INTERFACE
//--------------------------------------------------

void i2c_init(I2C_TypeDef* I2Cx, uint8_t ownAddress, uint8_t enableRxInterrupt);
uint8_t i2c_readByte(I2C_TypeDef* I2Cx, uint8_t i2cAddress7, uint8_t registerAddress);
ErrorStatus i2c_readBytes(I2C_TypeDef* I2Cx, uint8_t deviceAddress, uint8_t registerAddress, uint8_t count, uint8_t *dest);
ErrorStatus i2c_writeByte(I2C_TypeDef* I2Cx, uint8_t deviceAddress, uint8_t registerAddress, uint8_t data);
//ErrorStatus i2c_writeCommand(I2C_TypeDef* I2Cx, uint8_t deviceAddress, uint8_t command);
//uint8_t i2c_readCommandResponse(I2C_TypeDef* I2Cx, uint8_t deviceAddress);
void i2c_printFlags(I2C_TypeDef* I2Cx);

#endif /* I2C_L1xx_H_ */
