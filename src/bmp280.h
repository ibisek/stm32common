/***************************************************************************
 This is a library for the BMP280 pressure sensor.

 Written by Kevin Townsend for Adafruit Industries.
 BSD license, all text above must be included in any redistribution

 Modified for STM32 by ibisek.
 Version 2017-01-06
 ***************************************************************************/

#ifndef BMP280_H_
#define BMP280_H_

#include <math.h>

#include "serial.h"

#ifdef STM32F10x
#include "i2c.f103.h"
#else
#include "i2c.h"
#endif


#define BMP280_BUS I2C2

/*=========================================================================
 I2C ADDRESS/BITS/SETTINGS
 -----------------------------------------------------------------------*/
#define BMP280_ADDRESS	0x77 << 1	// the 7-bit address is 0x76 (SDO to GND) or 0x77 (SDO to UCC)
#define BMP280_CHIPID   0x58
/*=========================================================================*/

/*=========================================================================
 REGISTERS
 -----------------------------------------------------------------------*/
enum {
	BMP280_REGISTER_DIG_T1 = 0x88,
	BMP280_REGISTER_DIG_T2 = 0x8A,
	BMP280_REGISTER_DIG_T3 = 0x8C,

	BMP280_REGISTER_DIG_P1 = 0x8E,
	BMP280_REGISTER_DIG_P2 = 0x90,
	BMP280_REGISTER_DIG_P3 = 0x92,
	BMP280_REGISTER_DIG_P4 = 0x94,
	BMP280_REGISTER_DIG_P5 = 0x96,
	BMP280_REGISTER_DIG_P6 = 0x98,
	BMP280_REGISTER_DIG_P7 = 0x9A,
	BMP280_REGISTER_DIG_P8 = 0x9C,
	BMP280_REGISTER_DIG_P9 = 0x9E,

	BMP280_REGISTER_CHIPID = 0xD0,
	BMP280_REGISTER_VERSION = 0xD1,
	BMP280_REGISTER_SOFTRESET = 0xE0,

	BMP280_REGISTER_CAL26 = 0xE1,  // R calibration stored in 0xE1-0xF0

	BMP280_REGISTER_CONTROL = 0xF4,
	BMP280_REGISTER_CONFIG = 0xF5,
	BMP280_REGISTER_PRESSUREDATA = 0xF7,
	BMP280_REGISTER_TEMPDATA = 0xFA,
};

/*=========================================================================*/

/*=========================================================================
 CALIBRATION DATA
 -----------------------------------------------------------------------*/
typedef struct {
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;

	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;

	uint8_t dig_H1;
	int16_t dig_H2;
	uint8_t dig_H3;
	int16_t dig_H4;
	int16_t dig_H5;
	int8_t dig_H6;
} bmp280_calib_data;
/*=========================================================================*/

/* ---- PUBLIC INTERFACE ---- */

uint8_t bmp280_init();
double bmp280_readTemperature(void);
double bmp280_readPressure(void);
double bmp280_readAltitude(double seaLevelhPa);

/* ---- PRIVATE METHODS ---- */

void bmp280_readCoefficients(void);

void bmp280_write8(uint8_t reg, uint8_t value);
uint8_t bmp280_read8(uint8_t reg);
uint16_t bmp280_read16(uint8_t reg);
int16_t bmp280_readS16(uint8_t reg);
uint16_t bmp280_read16_LE(uint8_t reg); // little endian
int16_t bmp280_readS16_LE(uint8_t reg); // little endian
uint32_t bmp280_read24(uint8_t reg);

#endif // BMP280_H_

