/***************************************************
 This is a library for the HTU21D-F Humidity & Temp Sensor
 Designed specifically to work with the HTU21D-F sensor from Adafruit
 ----> https://www.adafruit.com/products/1899

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries.
 BSD license, all text above must be included in any redistribution

 Modified for STM32 by ibisek.
 Version 2017-01-06
 ****************************************************/

#ifndef HTU21D_H_
#define HTU21D_H_

#include "i2c.h"
#include "ibitime.h"

#define HTU21DF_I2CADDR      0x40 << 1
#define HTU21DF_READTEMP     0xE3
#define HTU21DF_READHUM      0xE5
#define HTU21DF_WRITEREG     0xE6
#define HTU21DF_READREG      0xE7
#define HTU21DF_RESET    	   0xFE

/* ---- PUBLIC INTERFACE ---- */

uint8_t htu21d_init();
double htu21d_readTemperature();
double htu21d_readHumidity();
void htu21d_reset();

/* ---- PRIVATE STUFF ---- */

double humidity, temp;

#endif /* HTU21D_H_ */
