/*
 * byteUtils.h
 *
 *  Created on: Apr 30, 2019
 *      Author: ibisek
 */

#ifndef BYTEUTILS_H_
#define BYTEUTILS_H_

/**
 * Compares two arrays byte by byte.
 */
bool arraysEqual(uint8_t* data1, uint8_t* data2, uint8_t len) {
	for(uint8_t i=0; i < len; i++) {
		uint8_t d1 = *((uint8_t*)data1+i) & 0xFF;
		uint8_t d2 = *((uint8_t*)data2+i) & 0xFF;

		if (d1 != d2) return false;
	}

	return true;
}

#endif /* BYTEUTILS_H_ */
