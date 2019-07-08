/*
 * flashStorage.h
 *
 *  Created on: Dec 18, 2016
 *      Author: ibisek
 */

#ifndef FLASHSTORAGE_H_
#define FLASHSTORAGE_H_

#ifdef STM32F0xx
#include "stm32f0xx.h"
#else
#ifdef STM32F10x
#include "stm32f10x.h"
#else
	error "Yet unsupported architecture"
#endif
#endif

#define FLASH_STORAGE_LEN	0x00000400	// 1024 bytes
#define FLASH_STORAGE_START_ADDR (0x08008000 - FLASH_STORAGE_LEN)

void flashStorage_init();
uint8_t flashStorage_write(uint16_t addr, uint32_t data);
void flashStorage_read(uint32_t addr, uint32_t *data);

#endif /* FLASHSTORAGE_H_ */
