/*
 * gps.h
 *
 *  Created on: Mar 14, 2016
 *      Author: ibisek
 */

#ifndef GPS_H_
#define GPS_H_

#include <stdint.h>
#include <string.h>

#include "conversionUtils.h"

typedef enum READING_MODE {
	NONE, HEADER, GGA, RMC, GSA
} readingModes;

struct GpsRecord {
	uint8_t hasFix;	// 0 = no fix; 1 - has fix
	char date[6];
	char time[6];
	char latitude[1 + 9];	//N4916.1234
	char longitude[1 + 10];	//E0123.1234
	char groundSpeed[6];	//655.35 [knots] ~ 1 214 km/h (1knot = 0.5144m/s); MAX 655.35 knot (!) - more will not fit into 2 bytes
	char trackCourse[6];	//359.89
	char numSatellites[2];	// 00 - 12
	char altitude[7]; 	// 12345.6 [m]; MAX 79999.9m (3B in BCD [saaa|bbb][cd][ef]). Space starts at 100km :)
	uint8_t dim; // ?=1, 2=2D, 3=3D
	uint8_t pdop; // 0-50.0
	uint8_t hdop; // 0-50.0
	uint8_t vdop; // 0-50.0
} gpsRecord;

char rxBuffer[16]; // no single item is longer than 10 bytes
uint8_t rxBufferInUse = 0;
uint8_t rxBufferPointer = 0;
const uint8_t rxBufferSize = sizeof(rxBuffer) / sizeof(rxBuffer[0]); // + termination char

volatile uint8_t newGpsPositionAvailable = 0;

volatile enum READING_MODE readingMode = NONE;
volatile uint8_t sentenceItemIndex = 0;

void printGpsRecord() {
	serial_print("fix:");
	serial_print_char((char) 0x30 + gpsRecord.hasFix);
	serial_print_arr_limited(" d:", gpsRecord.date, sizeof(gpsRecord.date));
	serial_print_arr_limited(" t:", gpsRecord.time, sizeof(gpsRecord.time));
	serial_print_arr_limited(" sat:", gpsRecord.numSatellites, sizeof(gpsRecord.numSatellites));

	if (gpsRecord.hasFix == 1) {
		serial_print_arr_limited(" lat:", gpsRecord.latitude, sizeof(gpsRecord.latitude));
		serial_print_arr_limited(" lon:", gpsRecord.longitude, sizeof(gpsRecord.longitude));
		serial_print_arr_limited(" gs:", gpsRecord.groundSpeed, sizeof(gpsRecord.groundSpeed));
		serial_print_arr_limited(" co:", gpsRecord.trackCourse, sizeof(gpsRecord.trackCourse));
		serial_print_arr_limited(" alt:", gpsRecord.altitude, sizeof(gpsRecord.altitude));

		serial_print(" p:");
		serial_print_int(gpsRecord.pdop);
		serial_print(" h:");
		serial_print_int(gpsRecord.hdop);
		serial_print(" v:");
		serial_print_int(gpsRecord.vdop);
		serial_print(" dim:");
		serial_print_int(gpsRecord.dim);
	}

	serial_print("\n");
}

void gps_readRMC(uint8_t itemIndex, char *rxBuffer, uint8_t dataLen) {
	if (itemIndex == 1) { // TIME
		memset(gpsRecord.time, 0, sizeof(gpsRecord.time));
		memcpy(gpsRecord.time, rxBuffer, (dataLen < sizeof(gpsRecord.time) ? dataLen : sizeof(gpsRecord.time)));

	} else if (itemIndex == 2) { // VALIDITY: A=OK V=INVALID
		//gpsRecord.hasFix = (rxBuffer[0] == 'A' ? 1 : 0);

	} else if (itemIndex == 3) { // LATITUDE
		memset(gpsRecord.latitude, 0, sizeof(gpsRecord.latitude));
		memcpy(gpsRecord.latitude + 1, rxBuffer, (dataLen < sizeof(gpsRecord.latitude) - 1 ? dataLen : sizeof(gpsRecord.latitude) - 1));

	} else if (itemIndex == 4) { // LATITUDE LETTER
		gpsRecord.latitude[0] = rxBuffer[0];

	} else if (itemIndex == 5) { // LONGITUDE
		memset(gpsRecord.longitude, 0, sizeof(gpsRecord.longitude));
		memcpy(gpsRecord.longitude + 1, rxBuffer, (dataLen < sizeof(gpsRecord.longitude) - 1 ? dataLen : sizeof(gpsRecord.longitude) - 1));

	} else if (itemIndex == 6) { // LONGITUDE LETTER
		gpsRecord.longitude[0] = rxBuffer[0];

	} else if (itemIndex == 7) { // GROUND SPEED
		memset(gpsRecord.groundSpeed, 0, sizeof(gpsRecord.groundSpeed));
		memcpy(gpsRecord.groundSpeed, rxBuffer, (dataLen < sizeof(gpsRecord.groundSpeed) ? dataLen : sizeof(gpsRecord.groundSpeed)));

	} else if (itemIndex == 8) { // TRACK COURSE
		memset(gpsRecord.trackCourse, 0, sizeof(gpsRecord.trackCourse));
		memcpy(gpsRecord.trackCourse, rxBuffer, (dataLen < sizeof(gpsRecord.trackCourse) ? dataLen : sizeof(gpsRecord.trackCourse)));

	} else if (itemIndex == 9) { // DATE
		memset(gpsRecord.date, 0, sizeof(gpsRecord.date));
		memcpy(gpsRecord.date, rxBuffer, (dataLen < sizeof(gpsRecord.date) ? dataLen : sizeof(gpsRecord.date)));
	}
}

void gps_readGGA(uint8_t itemIndex, char *rxBuffer, uint8_t dataLen) {
	if (itemIndex == 1) { // TIME
		memset(gpsRecord.time, 0, sizeof(gpsRecord.time));
		memcpy(gpsRecord.time, rxBuffer, (dataLen < sizeof(gpsRecord.time) ? dataLen : sizeof(gpsRecord.time)));

	} else if (itemIndex == 2) { // LATITUDE
		memset(gpsRecord.latitude, 0, sizeof(gpsRecord.latitude));
		memcpy(gpsRecord.latitude + 1, rxBuffer, (dataLen < sizeof(gpsRecord.latitude) - 1 ? dataLen : sizeof(gpsRecord.latitude) - 1));

	} else if (itemIndex == 3) { // LATITUDE LETTER
		gpsRecord.latitude[0] = rxBuffer[0];

	} else if (itemIndex == 4) { // LONGITUDE
		memset(gpsRecord.longitude, 0, sizeof(gpsRecord.longitude));
		memcpy(gpsRecord.longitude + 1, rxBuffer, (dataLen < sizeof(gpsRecord.longitude) - 1 ? dataLen : sizeof(gpsRecord.longitude) - 1));

	} else if (itemIndex == 5) { // LONGITUDE LETTER
		gpsRecord.longitude[0] = rxBuffer[0];

	} else if (itemIndex == 6) { // SIGNAL QUALITY - 0 = no signal, 1 = HAS FIX, 2 = DGPS
		gpsRecord.hasFix = (rxBuffer[0] != '0' ? 1 : 0);

	} else if (itemIndex == 7) { // NUM SATELLITES IN VIEW
		memset(gpsRecord.numSatellites, 0, sizeof(gpsRecord.numSatellites));
		memcpy(gpsRecord.numSatellites, rxBuffer, (dataLen < sizeof(gpsRecord.numSatellites) ? dataLen : sizeof(gpsRecord.numSatellites)));

	} else if (itemIndex == 9) { // GEOID ALTITUDE
		memset(gpsRecord.altitude, 0, sizeof(gpsRecord.altitude));
		memcpy(gpsRecord.altitude, rxBuffer, (dataLen < sizeof(gpsRecord.altitude) ? dataLen : sizeof(gpsRecord.altitude)));
	}
}

void gps_readGSA(uint8_t itemIndex, char *rxBuffer, uint8_t dataLen) {
	if (itemIndex == 2) {
		gpsRecord.dim = 0x0F & rxBuffer[0];
	} else if (itemIndex == 15) { // PDOP
		gpsRecord.pdop = strNumToInt(rxBuffer, (dataLen < 4 ? dataLen : 4)); // 55.0 -> maxLen = 4
	} else if (itemIndex == 16) { // HDOP
		gpsRecord.hdop = strNumToInt(rxBuffer, (dataLen < 4 ? dataLen : 4)); // 55.0 -> maxLen = 4
	} else if (itemIndex == 17) { // VDOP
		gpsRecord.vdop = strNumToInt(rxBuffer, (dataLen < 4 ? dataLen : 4)); // 55.0 -> maxLen = 4
	}
}

void processGpsData(enum READING_MODE readingMode, uint8_t sentenceItemIndex, char *rxBuffer, uint8_t rxBufferDataLen) {
	if (readingMode == RMC) {
		gps_readRMC(sentenceItemIndex, rxBuffer, rxBufferDataLen);
	} else if (readingMode == GGA) {
		gps_readGGA(sentenceItemIndex, rxBuffer, rxBufferDataLen);
	} else if (readingMode == GSA) {
		gps_readGSA(sentenceItemIndex, rxBuffer, rxBufferDataLen);
	}
}

void _clearRxBuffer() {
	memset(rxBuffer, 0, rxBufferSize);
	rxBufferPointer = 0;
}

/**
 * @param c incoming character from the serial line
 */
void processSerialInterruptChar(uint8_t c) {
	if (c == '$') {
		if (gpsRecord.hasFix && readingMode == GGA) { // last sentence was GPGGA, we have new position
			newGpsPositionAvailable = 1;
		}

		readingMode = HEADER;
		sentenceItemIndex = 0;
		_clearRxBuffer();

	} else if (c == ',' || c == '*') {
		if (readingMode == HEADER) {
			if (strcmp("GNGGA", rxBuffer) == 0 || strcmp("GPGGA", rxBuffer) == 0) {
				readingMode = GGA;
			} else if (strcmp("GNRMC", rxBuffer) == 0 || strcmp("GPRMC", rxBuffer) == 0) {
				readingMode = RMC;
			} else if (strcmp("GPGSA", rxBuffer) == 0 || strcmp("GPGSA", rxBuffer) == 0) {
				readingMode = GSA;
			} else {
				readingMode = NONE;
			}

		} else if (rxBufferPointer > 0) {	// data length > 0
			processGpsData(readingMode, sentenceItemIndex, rxBuffer, rxBufferPointer);
		}

		_clearRxBuffer();
		sentenceItemIndex++;

	} else if (readingMode != NONE) {
		rxBuffer[rxBufferPointer] = c;
		rxBufferPointer = (rxBufferPointer < rxBufferSize - 1 ? rxBufferPointer + 1 : 0);
	}
}

#endif /* GPS_H_ */
