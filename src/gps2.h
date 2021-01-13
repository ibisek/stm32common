/*
 * gps2.h
 *
 * Alternative to gps.h
 *
 *  Created on: Apr 21, 2018
 *      Author: ibisek
 */

#ifndef GPS2_H_
#define GPS2_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "stringUtils.h"
#include "conversionUtils.h"
#include "serial.h"
#include "nmea.h"

struct GpsRecord {
	uint8_t hasFix;	// 0 = no fix; 1 - has fix
	char date[6];	// DDMMYY
	char time[6];	// HHMMSS
	char latitude[1 + 9];	//N4913.4336
	char longitude[1 + 10];	//E01634.6103
	char groundSpeed[6];	//655.35 [knots] ~ 1 214 km/h (1knot = 0.5144m/s); MAX 655.35 knot (!) - more will not fit into 2 bytes
	char trackCourse[6];	//359.89
	char numSatellites[2];	// 00 - 12
	char altitude[7]; 	// 12345.6 [m]; MAX 79999.9m (3B in BCD [saaa|bbb][cd][ef]). Space starts at 100km :)
	uint8_t dim; // ?=1, 2=2D, 3=3D
	uint8_t pdop; // 0-50.0
	uint8_t hdop; // 0-50.0
	uint8_t vdop; // 0-50.0
} gpsRecord;

namespace gps2 {

char rxBuffer[250];
uint8_t rxBufferPointer = 0;
const uint8_t rxBufferSize = sizeof(rxBuffer) / sizeof(rxBuffer[0]); // + termination char

char processingBuffer[250];
uint8_t processingBufferDataLen = 0;
uint8_t processingBufferDataSet = 0;

volatile uint8_t newGpsPositionAvailable = 0;

void _clearRxBuffer() {
	memset(rxBuffer, 0, rxBufferSize);
	rxBufferPointer = 0;
}

void _clearProcessingBuffer() {
	memset(processingBuffer, 0, sizeof(processingBuffer));
	processingBufferDataLen = 0;
	processingBufferDataSet = 0;
}

void printGpsRecord() {
	serial_print("GPS fix:");
	serial_print_char((char) 0x30 + gpsRecord.hasFix);
	serial_print_arr_limited(" d:", gpsRecord.date, sizeof(gpsRecord.date));
	serial_print_arr_limited(" t:", gpsRecord.time, sizeof(gpsRecord.time));
	serial_print_arr_limited(" nSat:", gpsRecord.numSatellites, sizeof(gpsRecord.numSatellites));

	if (gpsRecord.hasFix == 1) {
		serial_print_arr_limited(" lat:", gpsRecord.latitude, sizeof(gpsRecord.latitude));
		serial_print_arr_limited(" lon:", gpsRecord.longitude, sizeof(gpsRecord.longitude));
		serial_print_arr_limited(" alt:", gpsRecord.altitude, sizeof(gpsRecord.altitude));
		serial_print_arr_limited(" trk:", gpsRecord.trackCourse, sizeof(gpsRecord.trackCourse));
		serial_print_arr_limited(" gs: ", gpsRecord.groundSpeed, sizeof(gpsRecord.groundSpeed));

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

void _processRMC(char* buf, uint8_t bufLen) {
	uint8_t dataLen = 11;	// no item is longer that that (with trailing 0)
	char data[dataLen];

	dataLen = extractItem(1, buf, bufLen, data);	//TIME
	if(dataLen > 0) {
		memset(gpsRecord.time, 0, sizeof(gpsRecord.time));
		memcpy(gpsRecord.time, data, dataLen);
	}

//	dataLen = extractItem(2, buf, bufLen, data);	// VALIDITY: A=OK V=INVALID
//	if(dataLen > 0)
//		gpsRecord.hasFix = (data[0] == 'A' ? 1 : 0);

	dataLen = extractItem(3, buf, bufLen, data);	// LATITUDE
	if(dataLen > 0) {
		memset(gpsRecord.latitude, 0, sizeof(gpsRecord.latitude));
		memcpy(gpsRecord.latitude + 1, data, (dataLen < sizeof(gpsRecord.latitude) - 1 ? dataLen : sizeof(gpsRecord.latitude) - 1));
	}

	dataLen = extractItem(4, buf, bufLen, data);	// LATITUDE LETTER
	if(dataLen > 0)
		gpsRecord.latitude[0] = data[0];

	dataLen = extractItem(5, buf, bufLen, data);	// LONGITUDE
	if(dataLen > 0) {
		memset(gpsRecord.longitude, 0, sizeof(gpsRecord.longitude));
		memcpy(gpsRecord.longitude + 1, data, (dataLen < sizeof(gpsRecord.longitude) - 1 ? dataLen : sizeof(gpsRecord.longitude) - 1));
	}

	dataLen = extractItem(6, buf, bufLen, data);	// LONGITUDE LETTER
	if(dataLen > 0)
		gpsRecord.longitude[0] = data[0];

	dataLen = extractItem(7, buf, bufLen, data);	// GROUND SPEED
	if(dataLen > 0) {
		memset(gpsRecord.groundSpeed, 0, sizeof(gpsRecord.groundSpeed));
		memcpy(gpsRecord.groundSpeed, data, (dataLen < sizeof(gpsRecord.groundSpeed) ? dataLen : sizeof(gpsRecord.groundSpeed)));
	}

	dataLen = extractItem(8, buf, bufLen, data);	// TRACK COURSE
	if(dataLen > 0) {
		memset(gpsRecord.trackCourse, 0, sizeof(gpsRecord.trackCourse));
		memcpy(gpsRecord.trackCourse, data, (dataLen < sizeof(gpsRecord.trackCourse) ? dataLen : sizeof(gpsRecord.trackCourse)));
	}

	dataLen = extractItem(9, buf, bufLen, data);	// DATE
	if(dataLen > 0) {
		memset(gpsRecord.date, 0, sizeof(gpsRecord.date));
		memcpy(gpsRecord.date, data, (dataLen < sizeof(gpsRecord.date) ? dataLen : sizeof(gpsRecord.date)));
	}
}

void _processGSA(char* buf, uint8_t bufLen) {
	uint8_t dataLen = 11;	// no item is longer that that (with trailing 0)
	char data[dataLen];

	dataLen = extractItem(2, buf, bufLen, data);	// DIM
	if(dataLen > 0)
		gpsRecord.dim = 0x0F & data[0];

	dataLen = extractItem(15, buf, bufLen, data);	// PDOP
	if(dataLen > 0)
		gpsRecord.pdop = strNumToInt(data, dataLen);

	dataLen = extractItem(16, buf, bufLen, data);	// HDOP
	if(dataLen > 0)
		gpsRecord.hdop = strNumToInt(data, dataLen);

	dataLen = extractItem(17, buf, bufLen, data);	// VDOP
	if(dataLen > 0)
		gpsRecord.vdop = strNumToInt(data, dataLen);
}


void _processGGA(char* buf, uint8_t bufLen) {
	uint8_t dataLen = 11;	// no item is longer that that (with trailing 0)
	char data[dataLen];

// ## THIS IS ALREADY EXTRACTED FROM RMC ##
//	dataLen = extractItem(1, buf, bufLen, data);	//TIME
//	if(dataLen > 0) {
//		memset(gpsRecord.time, 0, sizeof(gpsRecord.time));
//		memcpy(gpsRecord.time, data, (dataLen < sizeof(gpsRecord.time) ? dataLen : sizeof(gpsRecord.time)));
//	}
//
//	dataLen = extractItem(2, buf, bufLen, data);	// LATITUDE
//	if(dataLen > 0) {
//		memset(gpsRecord.latitude, 0, sizeof(gpsRecord.latitude));
//		memcpy(gpsRecord.latitude + 1, data, (dataLen < sizeof(gpsRecord.latitude) - 1 ? dataLen : sizeof(gpsRecord.latitude) - 1));
//	}
//
//	dataLen = extractItem(3, buf, bufLen, data);	// LATITUDE LETTER
//	if(dataLen > 0)
//		gpsRecord.latitude[0] = data[0];
//
//	dataLen = extractItem(4, buf, bufLen, data);	// LONGITUDE
//	if(dataLen > 0) {
//		memset(gpsRecord.longitude, 0, sizeof(gpsRecord.longitude));
//		memcpy(gpsRecord.longitude + 1, data, (dataLen < sizeof(gpsRecord.longitude) - 1 ? dataLen : sizeof(gpsRecord.longitude) - 1));
//	}
//
//	dataLen = extractItem(5, buf, bufLen, data);	// LONGITUDE LETTER
//	if(dataLen > 0)
//		gpsRecord.longitude[0] = data[0];

	dataLen = extractItem(6, buf, bufLen, data);	// SIGNAL QUALITY - 0 = no signal, 1 = HAS FIX, 2 = DGPS
	if(dataLen > 0) {
		gpsRecord.hasFix = (data[0] != '0' ? 1 : 0);
	}

	dataLen = extractItem(7, buf, bufLen, data);	// NUM SATELLITES IN VIEW
	if(dataLen > 0) {
		memset(gpsRecord.numSatellites, 0, sizeof(gpsRecord.numSatellites));
		memcpy(gpsRecord.numSatellites, data, (dataLen < sizeof(gpsRecord.numSatellites) ? dataLen : sizeof(gpsRecord.numSatellites)));
	}

	dataLen = extractItem(9, buf, bufLen, data);	// GEOID ALTITUDE
	if(dataLen > 0) {
		memset(gpsRecord.altitude, 0, sizeof(gpsRecord.altitude));
		memcpy(gpsRecord.altitude, data, (dataLen < sizeof(gpsRecord.altitude) ? dataLen : sizeof(gpsRecord.altitude)));
	}
}

/**
 * Expects NMEA without leading '$'
 */
void _processGpsBuffer() {
	if (startsWith(rxBuffer+2, "GGA")) {
		_processGGA(processingBuffer, processingBufferDataLen); // GPGGA is last sentence in the batch -> we have new position
		newGpsPositionAvailable = 1;
	} else if (startsWith(rxBuffer+2, "RMC")) {
		_processRMC(processingBuffer, processingBufferDataLen);
	} else if (startsWith(rxBuffer+2, "GSA")) {
		_processGSA(processingBuffer, processingBufferDataLen);
	}
}

void loop() {
	if (processingBufferDataSet) {
		if(startsWith(processingBuffer+2, "GGA") || startsWith(processingBuffer+2, "RMC") || startsWith(processingBuffer+2, "GSA")) {
			if(nmea_checkSumOk(processingBuffer, processingBufferDataLen))
				_processGpsBuffer();
		}

		_clearProcessingBuffer();
	}
}

void processSerialInterruptChar(uint8_t c) {
	if (c == '$') {
		_clearRxBuffer();

	} else if (c == '\n') {
		// move to processing buffer and set flag:
		if(!processingBufferDataSet) {
			memcpy(processingBuffer, rxBuffer, rxBufferPointer);
			processingBufferDataLen = rxBufferPointer;
			processingBufferDataSet = 1;
		}

	} else if (c == '\r') {	// throw it away
		return;

	} else {
		rxBuffer[rxBufferPointer] = c;
		rxBufferPointer = (rxBufferPointer < rxBufferSize - 1 ? rxBufferPointer + 1 : 0);
	}
}

} // ~namespace

#endif /* GPS2_H_ */
