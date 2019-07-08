#include <bmp180.h>
#include <i2c.h>
#include <stdio.h>
#include <math.h>

double c5, c6, mc, md, x0, x1, x2, _y0, _y1, y2, p0, p1, p2;
char _error;

volatile double T, P;		// [deg.C, hPa]
volatile uint8_t newTPSampleSet;

// Initialize library for subsequent pressure measurements
uint8_t bmp180_init() {
	double c3, c4, b1;
	int16_t AC1, AC2, AC3, VB1, VB2, MB, MC, MD;
	uint16_t AC4, AC5, AC6;

	i2c_init(0x01, 0);

	// The BMP180 includes factory calibration data stored on the device.
	// Each device has different numbers, these must be retrieved and
	// used in the calculations when taking pressure measurements.

	// Retrieve calibration data from device:
	if (_bmp180_readInt(0xAA, &AC1) && _bmp180_readInt(0xAC, &AC2) && _bmp180_readInt(0xAE, &AC3) && _bmp180_readUInt(0xB0, &AC4) && _bmp180_readUInt(0xB2, &AC5)
			&& _bmp180_readUInt(0xB4, &AC6) && _bmp180_readInt(0xB6, &VB1) && _bmp180_readInt(0xB8, &VB2) && _bmp180_readInt(0xBA, &MB) && _bmp180_readInt(0xBC, &MC)
			&& _bmp180_readInt(0xBE, &MD)) {

		// All reads completed successfully!

		// If you need to check your math using known numbers,
		// you can uncomment one of these examples.
		// (The correct results are commented in the below functions.)

		// Example from Bosch datasheet
		// AC1 = 408; AC2 = -72; AC3 = -14383; AC4 = 32741; AC5 = 32757; AC6 = 23153;
		// B1 = 6190; B2 = 4; MB = -32768; MC = -8711; MD = 2868;

		// Example from http://wmrx00.sourceforge.net/Arduino/BMP180-Calcs.pdf
		// AC1 = 7911; AC2 = -934; AC3 = -14306; AC4 = 31567; AC5 = 25671; AC6 = 18974;
		// VB1 = 5498; VB2 = 46; MB = -32768; MC = -11075; MD = 2432;

		/*
		 serial_println_arr_int16("AC1:", AC1);
		 serial_println_arr_int16("AC2:", AC2);
		 serial_println_arr_int16("AC3:", AC3);
		 serial_println_arr_int16("AC4:", AC4);
		 serial_println_arr_int16("AC5:", AC5);
		 serial_println_arr_int16("AC6:", AC6);
		 serial_println_arr_int16("VB1:", VB1);
		 serial_println_arr_int16("VB2:", VB2);
		 serial_println_arr_int16("MB:", MB);
		 serial_println_arr_int16("MC:", MC);
		 serial_println_arr_int16("MD:", MD);
		 */

		// Compute floating-point polynominals:
		c3 = 160.0 * pow(2, -15) * AC3;
		c4 = pow(10, -3) * pow(2, -15) * AC4;
		b1 = pow(160, 2) * pow(2, -30) * VB1;
		c5 = (pow(2, -15) / 160) * AC5;
		c6 = AC6;
		mc = (pow(2, 11) / pow(160, 2)) * MC;
		md = MD / 160.0;
		x0 = AC1;
		x1 = 160.0 * pow(2, -13) * AC2;
		x2 = pow(160, 2) * pow(2, -25) * VB2;
		_y0 = c4 * pow(2, 15);
		_y1 = c4 * c3;
		y2 = c4 * b1;
		p0 = (3791.0 - 8.0) / 1600.0;
		p1 = 1.0 - 7357.0 * pow(2, -20);
		p2 = 3038.0 * 100.0 * pow(2, -36);

		return (1); // Success!

	} else {
		// Error reading calibration data; bad component or connection?
		return (0);
	}
}

// Read an array of bytes from device
// values: external array to hold data. Put starting register in values[0].
// length: number of bytes to read
uint8_t _bmp180_readBytes(unsigned char *values, char length) {

	ErrorStatus res = i2c_readBytes(BMP180_ADDR, values[0], length, values);
	if (res == ERROR) { // failed
		return (0); // false
	} else {
		return (1); // true
	}

//	char x;
//	Wire.beginTransmission(BMP180_ADDR);
//	Wire.write(values[0]);
//	_error = Wire.endTransmission();
//	if (_error == 0) {
//		Wire.requestFrom(BMP180_ADDR, length);
//		while (Wire.available() != length)
//			; // wait until bytes are ready
//		for (x = 0; x < length; x++) {
//			values[x] = Wire.read();
//		}
//		return (1);
//	}
//	return (0);
}

// Write an array of bytes to device
// values: external array of data to write. Put starting register in values[0].
// length: number of bytes to write
uint8_t _bmp180_writeBytes(unsigned char *values, char length) {

	uint8_t regAddr = values[0];
	for (uint8_t i = 1; i < length; i++) {
		ErrorStatus res = i2c_writeByte(BMP180_ADDR, regAddr++, values[i]);
		if (res == ERROR)
			return (0); // error
	}

	return (1);

//	char x;
//
//	Wire.beginTransmission(BMP180_ADDR);
//	Wire.write(values, length);
//	_error = Wire.endTransmission();
//	if (_error == 0)
//		return (1);
//	else
//		return (0);
}

// Read a signed integer (two bytes) from device
// address: register to start reading (plus subsequent register)
// value: external variable to store data (function modifies value)
uint8_t _bmp180_readInt(uint8_t address, int16_t *value) {
	unsigned char data[2];

	data[0] = address;
	if (_bmp180_readBytes(data, 2)) {
		*value = (int16_t) ((data[0] << 8) | data[1]);
		//if (*value & 0x8000) *value |= 0xFFFF0000; // sign extend if negative
		return (1);
	}
	*value = 0;
	return (0);
}

// Read an unsigned integer (two bytes) from device
// address: register to start reading (plus subsequent register)
// value: external variable to store data (function modifies value)
uint8_t _bmp180_readUInt(uint8_t address, uint16_t *value) {
	unsigned char data[2];

	data[0] = address;
	if (_bmp180_readBytes(data, 2)) {
		*value = (((uint16_t) data[0] << 8) | (uint16_t) data[1]);
		return (1);
	}
	*value = 0;
	return (0);
}

// Begin a temperature reading.
// Will return delay in ms to wait, or 0 if I2C error
uint8_t bmp180_startTemperature(void) {
	unsigned char data[2], result;

	data[0] = BMP180_REG_CONTROL;
	data[1] = BMP180_COMMAND_TEMPERATURE;
	result = _bmp180_writeBytes(data, 2);
	if (result) // good write?
		return (5); // return the delay in ms (rounded up) to wait before retrieving data
	else
		return (0); // or return 0 if there was a problem communicating with the BMP
}

// Retrieve a previously-started temperature reading.
// Requires begin() to be called once prior to retrieve calibration parameters.
// Requires startTemperature() to have been called prior and sufficient time elapsed.
// T: external variable to hold result.
// Returns 1 if successful, 0 if I2C error.
uint8_t bmp180_getTemperature(double *T) {
	unsigned char data[2];
	char result;
	double tu, a;

	data[0] = BMP180_REG_RESULT;

	result = _bmp180_readBytes(data, 2);
	if (result) // good read, calculate temperature
	{
		tu = (data[0] * 256.0) + data[1];

		//example from Bosch datasheet
		//tu = 27898;

		//example from http://wmrx00.sourceforge.net/Arduino/BMP085-Calcs.pdf
		//tu = 0x69EC;

		a = c5 * (tu - c6);
		*T = a + (mc / (a + md));

		/*		
		 Serial.println();
		 Serial.print("tu: "); Serial.println(tu);
		 Serial.print("a: "); Serial.println(a);
		 Serial.print("T: "); Serial.println(*T);
		 */
	}
	return (result);
}

// Begin a pressure reading.
// Oversampling: 0 to 3, higher numbers are slower, higher-res outputs.
// Will return delay in ms to wait, or 0 if I2C error.
uint8_t bmp180_startPressure(char oversampling) {
	unsigned char data[2], result, delay;

	data[0] = BMP180_REG_CONTROL;

	switch (oversampling) {
	case 0:
		data[1] = BMP180_COMMAND_PRESSURE0;
		delay = 5;
		break;
	case 1:
		data[1] = BMP180_COMMAND_PRESSURE1;
		delay = 8;
		break;
	case 2:
		data[1] = BMP180_COMMAND_PRESSURE2;
		delay = 14;
		break;
	case 3:
		data[1] = BMP180_COMMAND_PRESSURE3;
		delay = 26;
		break;
	default:
		data[1] = BMP180_COMMAND_PRESSURE0;
		delay = 5;
		break;
	}
	result = _bmp180_writeBytes(data, 2);
	if (result) // good write?
		return (delay); // return the delay in ms (rounded up) to wait before retrieving data
	else
		return (0); // or return 0 if there was a problem communicating with the BMP
}

// Retrieve a previously started pressure reading, calculate absolute pressure in mbars.
// Requires begin() to be called once prior to retrieve calibration parameters.
// Requires startPressure() to have been called prior and sufficient time elapsed.
// Requires recent temperature reading to accurately calculate pressure.

// P: external variable to hold pressure.
// T: previously-calculated temperature.
// Returns 1 for success, 0 for I2C error.

// Note that calculated pressure value is absolute mbars, to compensate for altitude call sealevel().
uint8_t bmp180_getPressure(double *P, double *T) {
	unsigned char data[3];
	char result;
	double pu, s, x, y, z;

	data[0] = BMP180_REG_RESULT;

	result = _bmp180_readBytes(data, 3);
	if (result) // good read, calculate pressure
	{
		pu = (data[0] * 256.0) + data[1] + (data[2] / 256.0);

		//example from Bosch datasheet
		//pu = 23843;

		//example from http://wmrx00.sourceforge.net/Arduino/BMP085-Calcs.pdf, pu = 0x982FC0;	
		//pu = (0x98 * 256.0) + 0x2F + (0xC0/256.0);

		s = *T - 25.0;
		x = (x2 * pow(s, 2)) + (x1 * s) + x0;
		y = (y2 * pow(s, 2)) + (_y1 * s) + _y0;
		z = (pu - x) / y;
		*P = (p2 * pow(z, 2)) + (p1 * z) + p0;

		/*
		 Serial.println();
		 Serial.print("pu: "); Serial.println(pu);
		 Serial.print("T: "); Serial.println(*T);
		 Serial.print("s: "); Serial.println(s);
		 Serial.print("x: "); Serial.println(x);
		 Serial.print("y: "); Serial.println(y);
		 Serial.print("z: "); Serial.println(z);
		 Serial.print("P: "); Serial.println(*P);
		 */
	}
	return (result);
}

// Given a pressure P (mb) taken at a specific altitude (meters),
// return the equivalent pressure (mb) at sea level.
// This produces pressure readings that can be used for weather measurements.
double bmp180_sealevel(double P, double A) {
	return (P / pow(1 - (A / 44330.0), 5.255));
}

// Given a pressure measurement P (mb) and the pressure at a baseline P0 (mb),
// return altitude (meters) above baseline.
double bmp180_altitude(double P, double P0) {
	return (44330.0 * (1 - pow(P / P0, 1 / 5.255)));
}

// If any library command fails, you can retrieve an extended
// error code using this command. Errors are from the wire library:
// 0 = Success
// 1 = Data too long to fit in transmit buffer
// 2 = Received NACK on transmit of address
// 3 = Received NACK on transmit of data
// 4 = Other error
uint8_t bmp180_getError(void) {
	return (_error);
}

