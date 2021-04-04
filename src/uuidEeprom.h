/*
 * eepromUniqueId.h
 *
 *  Created on: Apr 4, 2021
 *      Author: ibisek
 *
 * This workaround exists due to duplicate "unique" ids for L152 devices.
 * The first 4 bytes of the EEPROM is taken by programmed (hopefully also) unique OGN ID.
 */

#ifndef UUIDEEPROM_H_
#define UUIDEEPROM_H_

#include <stm32l1xx.h>

#define DATA_EEPROM_START_ADDR     0x08080000	// ref. manual 'Table 9. NVM module organization (Cat.3 devices)'

uint32_t eepromUniqueId_read(void) {
	uint32_t id;
	memcpy(&id, (uint32_t*) DATA_EEPROM_START_ADDR, sizeof(id));

	return id;
}

uint32_t eepromUniqueId_write(uint32_t id) {
	DATA_EEPROM_Unlock();
	FLASH_Status s = DATA_EEPROM_ProgramWord(DATA_EEPROM_START_ADDR, id);
	if (s != FLASH_COMPLETE) return FLASH->SR;
	DATA_EEPROM_Lock();

	return 0;
}

#endif /* UUIDEEPROM_H_ */
