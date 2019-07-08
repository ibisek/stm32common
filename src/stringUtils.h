/*
 * stringUtils.h
 *
 *  Created on: Apr 23, 2018
 *      Author: ibisek
 */

#include <string.h>

#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

/**
 * Parses NMEA sentence and fills specified itemNo into a buffer.
 * (this a variance to StringTokenizer)
 * @param itemNo
 * @param str to parse - NMEA string
 * @param buf to store the item
 * @return 0 if there is no such item
 */
uint8_t extractItem(uint8_t itemNo, char* str, uint8_t strLen, char* buf) {
	uint8_t bufIndex = 0;

	uint8_t currItemNo = 0;
	for(uint8_t i = 0; i < strLen; i++) {
		if(str[i] == ',' || str[i] == '*') {
			if(currItemNo == itemNo) {
				return bufIndex;
			}

			currItemNo++;
			continue;
		}

		if(currItemNo == itemNo)
			buf[bufIndex++] = str[i];
	}

	return 0;
}

/**
 * @param str(ing)
 * @param pre(fix)
 */
bool startsWith(const char *str, const char *pre) {
    return strncmp(pre, str, strlen(pre)) == 0;
}

#endif /* STRINGUTILS_H_ */
