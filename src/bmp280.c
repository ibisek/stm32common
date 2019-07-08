/***************************************************************************
 This is a library for the BMP280 pressure sensor.

 Written by Kevin Townsend for Adafruit Industries.
 BSD license, all text above must be included in any redistribution

 Modified for STM32 by ibisek.
 Version 2017-01-06
 ***************************************************************************/

#include "bmp280.h"

/***************************************************************************
 PRIVATE VARIABLES
 ***************************************************************************/

int32_t t_fine;
bmp280_calib_data _bmp280_calib;

/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/

/**
 * Writes an 8 bit value over I2C
 */
void bmp280_write8(uint8_t reg, uint8_t value) {
	i2c_writeByte(BMP280_BUS, BMP280_ADDRESS, reg, value);
}

/**
 * Reads an 8 bit value over I2C
 */
uint8_t bmp280_read8(uint8_t reg) {
	return i2c_readByte(BMP280_BUS, BMP280_ADDRESS, reg);
}

/**
 * Reads a 16 bit value over I2C
 */
uint16_t bmp280_read16(uint8_t reg) {

	uint8_t bufik[2];
	ErrorStatus res = i2c_readBytes(BMP280_BUS, BMP280_ADDRESS, reg, 2, bufik);

	if (res == ERROR) {
		serial_print("ERROR: bmp280_read16()");
		return 0;
	}

	return (bufik[0] << 8) | bufik[1];
}

/**
 *
 */
uint16_t bmp280_read16_LE(uint8_t reg) {
	uint16_t temp = bmp280_read16(reg);
	return (temp >> 8) | (temp << 8);
}

/**
 * Reads a signed 16 bit value over I2C
 */
int16_t bmp280_readS16(uint8_t reg) {
	return (int16_t) bmp280_read16(reg);

}

int16_t bmp280_readS16_LE(uint8_t reg) {
	return (int16_t) bmp280_read16_LE(reg);

}

/**
 * Reads a 24 bit value over I2C
 */
uint32_t bmp280_read24(uint8_t reg) {

	uint8_t bufik[3];
	ErrorStatus res = i2c_readBytes(BMP280_BUS, BMP280_ADDRESS, reg, 3, bufik);

	if (res == ERROR) {
		serial_print("ERROR: bmp280_read24()");
		return 0;
	}

	return (bufik[0] << 16) | (bufik[1] << 8) | bufik[2];
}

/**
 * Reads the factory-set coefficients.
 */
void bmp280_readCoefficients(void) {
	_bmp280_calib.dig_T1 = bmp280_read16_LE(BMP280_REGISTER_DIG_T1);
	_bmp280_calib.dig_T2 = bmp280_readS16_LE(BMP280_REGISTER_DIG_T2);
	_bmp280_calib.dig_T3 = bmp280_readS16_LE(BMP280_REGISTER_DIG_T3);

	_bmp280_calib.dig_P1 = bmp280_read16_LE(BMP280_REGISTER_DIG_P1);
	_bmp280_calib.dig_P2 = bmp280_readS16_LE(BMP280_REGISTER_DIG_P2);
	_bmp280_calib.dig_P3 = bmp280_readS16_LE(BMP280_REGISTER_DIG_P3);
	_bmp280_calib.dig_P4 = bmp280_readS16_LE(BMP280_REGISTER_DIG_P4);
	_bmp280_calib.dig_P5 = bmp280_readS16_LE(BMP280_REGISTER_DIG_P5);
	_bmp280_calib.dig_P6 = bmp280_readS16_LE(BMP280_REGISTER_DIG_P6);
	_bmp280_calib.dig_P7 = bmp280_readS16_LE(BMP280_REGISTER_DIG_P7);
	_bmp280_calib.dig_P8 = bmp280_readS16_LE(BMP280_REGISTER_DIG_P8);
	_bmp280_calib.dig_P9 = bmp280_readS16_LE(BMP280_REGISTER_DIG_P9);
}

/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/

/**
 * @return 1 if succeeded, 0 otherwise
 */
uint8_t bmp280_init() {
	i2c_init(BMP280_BUS, 0x01, 0);

	if (bmp280_read8(BMP280_REGISTER_CHIPID) != BMP280_CHIPID) return 0;

	bmp280_readCoefficients();
	//bmp280_write8(BMP280_REGISTER_CONTROL, 0x3F);	// Tx1 | Px16 | normal mode
	bmp280_write8(BMP280_REGISTER_CONTROL, 0x37);	// oversampling: Tx4 | Px16 | power: normal mode

	bmp280_write8(BMP280_REGISTER_CONFIG, bmp280_read8(BMP280_REGISTER_CONFIG) | 0b00011100);	// IIR filter x 16

	return 1;
}

/**
 * @return temperature in [deg.C]
 */
double bmp280_readTemperature(void) {
	int32_t var1, var2;

	int32_t adc_T = bmp280_read24(BMP280_REGISTER_TEMPDATA);
	adc_T >>= 4;

	var1 = ((((adc_T >> 3) - ((int32_t) _bmp280_calib.dig_T1 << 1))) * ((int32_t) _bmp280_calib.dig_T2)) >> 11;

	var2 = (((((adc_T >> 4) - ((int32_t) _bmp280_calib.dig_T1)) * ((adc_T >> 4) - ((int32_t) _bmp280_calib.dig_T1))) >> 12) * ((int32_t) _bmp280_calib.dig_T3))
			>> 14;

	t_fine = var1 + var2;

	double T = (double)((t_fine * 5 + 128) >> 8);
	return T / 100;
}

/**
 * @return pressure in [Pa]
 */
double bmp280_readPressure(void) {
	int64_t var1, var2, p;

// Must be done first to get the t_fine variable set up
	bmp280_readTemperature();

	int32_t adc_P = bmp280_read24(BMP280_REGISTER_PRESSUREDATA);
	adc_P >>= 4;

	var1 = ((int64_t) t_fine) - 128000;
	var2 = var1 * var1 * (int64_t) _bmp280_calib.dig_P6;
	var2 = var2 + ((var1 * (int64_t) _bmp280_calib.dig_P5) << 17);
	var2 = var2 + (((int64_t) _bmp280_calib.dig_P4) << 35);
	var1 = ((var1 * var1 * (int64_t) _bmp280_calib.dig_P3) >> 8) + ((var1 * (int64_t) _bmp280_calib.dig_P2) << 12);
	var1 = (((((int64_t) 1) << 47) + var1)) * ((int64_t) _bmp280_calib.dig_P1) >> 33;

	if (var1 == 0) {
		return 0;  // avoid exception caused by division by zero
	}
	p = 1048576.0 - adc_P;
	p = (((p << 31) - var2) * 3125.0) / var1;
	var1 = (((int64_t) _bmp280_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((int64_t) _bmp280_calib.dig_P8) * p) >> 19;

	p = ((p + var1 + var2) >> 8) + (((int64_t) _bmp280_calib.dig_P7) << 4);
	return (double) p / 256.0;
}

/**
 *
 */
double bmp280_readAltitude(double seaLevelhPa) {
	double altitude;

	double pressure = bmp280_readPressure(); // in Si units for Pascal
	pressure /= 100;

	altitude = 44330 * (1.0 - pow(pressure / seaLevelhPa, 0.1903));

	return altitude;
}
