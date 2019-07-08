/*
 * gpsBuf.h
 *
 *  Created on: Jul 17, 2017
 *      Author: ibisek
 *
 * Stores incoming characters from the GPS module for later processing.
 *
 * !! NOT PROPERLY TESTED, DO NOT USE !!
 */

#ifndef GPSBUF_H_
#define GPSBUF_H_

#include "serial.h"
#include "serialDma.h"
#include "gps.h"
#include "stringUtils.h"

namespace gpsBuf {

#define ENABLE_GPS_NMEA_OUT 0

const uint8_t rxBufferSize = 80;	// shall be enough ;)
char rxBuffer[rxBufferSize];
char processingBuffer[rxBufferSize];

uint8_t rxBufferPointer = 0;
uint8_t processingBufferDataLen = 0;
uint8_t completeSentenceInBuffer = 0;

void _clearRxBuffer() {
	memset(rxBuffer, 0, rxBufferSize);
	rxBufferPointer = 0;
}

void _addToBuffer(char c) {
	rxBuffer[rxBufferPointer] = c;
	rxBufferPointer = (rxBufferPointer < rxBufferSize - 1 ? rxBufferPointer + 1 : 0);
}

void add(char c) {
	processSerialInterruptChar(c);	// call to the the single-character GPS processing logic
	return;

//	if (c == '\n') {
//		_addToBuffer(c);
//
//		memset(processingBuffer, 0, rxBufferSize);	// zero out the processing buffer
//		memcpy(processingBuffer, rxBuffer, rxBufferPointer); // move the data to the other buffer
//		processingBufferDataLen = rxBufferPointer;
//
//		completeSentenceInBuffer = 1;
//
//	} else if (c == '$') {
//		_clearRxBuffer();	// clean immediately for next incoming characters
//		return;
//	}
//
//	_addToBuffer(c);
}

void loop() {
	if (!completeSentenceInBuffer || processingBufferDataLen == 0)
		return;	//no data to process

//	processGpsBuffer(processingBuffer, processingBufferDataLen);

#if ENABLE_GPS_NMEA_OUT
	// print out required GPS sentence(s):
	if (startsWith(processingBuffer, "GPGGA") || startsWith(processingBuffer, "GPRMC") || startsWith(processingBuffer, "GPGSA")) {
//		serial_print_arr_limited("", processingBuffer, processingBufferDataLen);
//		serial_print_char('\n');
		serial_printDma(processingBuffer, processingBufferDataLen);
	}
#endif

	completeSentenceInBuffer = 0;
}

} // ~namespace

#endif /* GPSBUF_H_ */
