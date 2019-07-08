/*
 * flashStorage.c
 *
 *  Created on: Dec 18, 2016
 *      Author: ibisek
 */

#include "flashStorage.h"

/**
 * Initializes last 1kB of the internal flash to be used as 'eeprom' storage.
 * Last byte of the storage ('0xFF') is used to check the block has been already formatted and initialized for storage.
 */
void flashStorage_init() {
	// check if the page has been initialized:
	uint8_t d;
	uint8_t *dd = &d;
	*dd = (*(__IO uint8_t*) (FLASH_STORAGE_START_ADDR + FLASH_STORAGE_LEN -1 ));
	if (d != 0xFF) FLASH_ErasePage(FLASH_STORAGE_START_ADDR);
}

/**
 * @param addr <0; 1020>
 * @oaram data 4 bytes of data to be stored in the flash
 */
uint8_t flashStorage_write(uint16_t addr, uint32_t data) {
	if (addr >= (FLASH_STORAGE_LEN - 4 - 1))	// max addr limit = 1020
		return 0;

	FLASH_Unlock();
	FLASH_ErasePage(FLASH_STORAGE_START_ADDR);
	FLASH_Status res = FLASH_ProgramWord(FLASH_STORAGE_START_ADDR + addr, data);
	FLASH_Lock();

	if (res == FLASH_COMPLETE)
		return 1;
//	else if (res == FLASH_ERROR_PROGRAM)
//		serial_println("WR: FLASH_ERROR_PROGRAM");
//	else if (res == FLASH_ERROR_WRP)
//		serial_println("WR: FLASH_ERROR_WRP");
//	else if (res == FLASH_COMPLETE)
//		serial_println("WR: FLASH_COMPLETE");
//	else if (res == FLASH_TIMEOUT) serial_println("WR: FLASH_TIMEOUT");

	return 0;
}

/**
 * @param addr <0; 1020>
 * @param data
 */
void flashStorage_read(uint32_t addr, uint32_t *data) {
*data = (*(__IO uint32_t*) (FLASH_STORAGE_START_ADDR + addr));
}

