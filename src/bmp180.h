/*
BMP180.h
 Bosch BMP180 pressure sensor library STM32
 Mike Grusin, SparkFun Electronics

 Uses floating-point equations from the Weather Station Data Logger project
 http://wmrx00.sourceforge.net/
 http://wmrx00.sourceforge.net/Arduino/BMP085-Calcs.pdf

 Forked from BMP085 library by M.Grusin

 version 1.0 2013/09/20 initial version
 Version 1.1.2 - Updated for Arduino 1.6.4 5/2015

 Our example code uses the "beerware" license. You can do anything
 you like with this code. No really, anything. If you find it useful,
 buy me a (root) beer someday.

 @author Ibisek, Airborne Instruments
 @version 2016-12-06
 */

#ifndef BMP180_H
#define BMP180_H

#ifdef STM32F0xx
#include "stm32f0xx.h"
#else
	error "Yet unsupported architecture"
#endif

#include <inttypes.h>


#define BMP180_ADDR 0x77 << 1 // 0x77 is a 7-bit address. 0x77 << 1 = 0xEE!!
#define BMP180_I2C_INTERFACE I2C2

#define	BMP180_REG_CONTROL 0xF4
#define	BMP180_REG_RESULT 0xF6

#define	BMP180_COMMAND_TEMPERATURE 0x2E
#define	BMP180_COMMAND_PRESSURE0 0x34
#define	BMP180_COMMAND_PRESSURE1 0x74
#define	BMP180_COMMAND_PRESSURE2 0xB4
#define	BMP180_COMMAND_PRESSURE3 0xF4

uint8_t bmp180_init();
// call pressure.begin() to initialize BMP180 before use
// returns 1 if success, 0 if failure (bad component or I2C bus shorted?)

uint8_t bmp180_startTemperature(void);
// command BMP180 to start a temperature measurement
// returns (number of ms to wait) for success, 0 for fail

uint8_t bmp180_getTemperature(double *T);
// return temperature measurement from previous startTemperature command
// places returned value in T variable (deg C)
// returns 1 for success, 0 for fail

uint8_t bmp180_startPressure(char oversampling);
// command BMP180 to start a pressure measurement
// oversampling: 0 - 3 for oversampling value
// returns (number of ms to wait) for success, 0 for fail

uint8_t bmp180_getPressure(double *P, double *T);
// return absolute pressure measurement from previous startPressure command
// note: requires previous temperature measurement in variable T
// places returned value in P variable (mbar)
// returns 1 for success, 0 for fail

double bmp180_sealevel(double P, double A);
// convert absolute pressure to sea-level pressure (as used in weather data)
// P: absolute pressure (mbar)
// A: current altitude (meters)
// returns sealevel pressure in mbar

double bmp180_altitude(double P, double P0);
// convert absolute pressure to altitude (given baseline pressure; sea-level, runway, etc.)
// P: absolute pressure (mbar)
// P0: fixed baseline pressure (mbar)
// returns signed altitude in meters

uint8_t bmp180_getError(void);
// If any library command fails, you can retrieve an extended
// error code using this command. Errors are from the wire library:
// 0 = Success
// 1 = Data too long to fit in transmit buffer
// 2 = Received NACK on transmit of address
// 3 = Received NACK on transmit of data
// 4 = Other error

// private:

uint8_t _bmp180_readInt(uint8_t address, int16_t *value);
// read an signed int (16 bits) from a BMP180 register
// address: BMP180 register address
// value: external signed int for returned value (16 bits)
// returns 1 for success, 0 for fail, with result in value

uint8_t _bmp180_readUInt(uint8_t address, uint16_t *value);
// read an unsigned int (16 bits) from a BMP180 register
// address: BMP180 register address
// value: external unsigned int for returned value (16 bits)
// returns 1 for success, 0 for fail, with result in value

uint8_t _bmp180_readBytes(unsigned char *values, char length);
// read a number of bytes from a BMP180 register
// values: array of char with register address in first location [0]
// length: number of bytes to read back
// returns 1 for success, 0 for fail, with read bytes in values[] array

uint8_t _bmp180_writeBytes(unsigned char *values, char length);
// write a number of bytes to a BMP180 register (and consecutive subsequent registers)
// values: array of char with register address in first location [0]
// length: number of bytes to write
// returns 1 for success, 0 for fail

#endif
