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

#include "htu21d.h"

uint8_t htu21d_init() {

	//i2c_init(0x01, 0);

	htu21d_reset();

	ErrorStatus res = i2c_writeCommand(HTU21DF_I2CADDR, HTU21DF_READREG);
	if (res == ERROR) return 0;

	uint8_t val = i2c_readCommandResponse(HTU21DF_I2CADDR);
	if (val == 0x02) // after reset should be 0x02
		return 1;

	return 0;
}

void htu21d_reset(void) {
	i2c_writeCommand(HTU21DF_I2CADDR, HTU21DF_RESET);
	delay_ms(15);
}

double htu21d_readTemperature() {

	i2c_writeCommand(HTU21DF_I2CADDR, HTU21DF_READTEMP);
	delay_ms(50);

	uint8_t bufik[3];
	ErrorStatus res = i2c_readBytes(HTU21DF_I2CADDR, HTU21DF_READTEMP, 3, bufik);
	if (res == ERROR) {
		serial_print("ERROR: htu21d_readTemperature()");
		return 0;
	}

	double t = (bufik[0] << 8 | (bufik[1] & 0xFC));	// LSB bits 0+1 are status - chop them off; byte[3] is CRC

	t = t * 175.72 / 65536.0 - 46.85;

	return t;
}

double htu21d_readHumidity() {

	i2c_writeCommand(HTU21DF_I2CADDR, HTU21DF_READHUM);
	delay_ms(50);

	uint8_t bufik[3];
	ErrorStatus res = i2c_readBytes(HTU21DF_I2CADDR, HTU21DF_READHUM, 3, bufik);
	if (res == ERROR) {
		serial_print("ERROR: htu21d_readHumidity()");
		return 0;
	}

	double h = (bufik[0] << 8 | (bufik[1] & 0xFC));	// LSB bits 0+1 are status - chop them off; byte[3] is CRC

	h = h * 125 / 65536 -6;

	return h;
}

