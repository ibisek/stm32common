/*
 * mpu6500.h
 *
 *  Created on: Sep 25, 2018
 *      Author: ibisek
 */

#ifndef MPU6500_I2C_H_
#define MPU6500_I2C_H_

#include "serial.h"
#ifdef STM32F10x
#include "i2c.f103.h"
#else
#include "i2c.h"
#endif

namespace mpu6500 {

#define MPU6500_ACC_CONF1          0x1C
#define MPU6500_ACC_CONF2          0x1D
#define MPU6500_LP_ACCEL_ODR       0x1E
#define ACCEL_XOUT_H               0x3B
#define MPU6500_PWR_MGMT1          0x6B
#define MPU6500_PWR_MGMT2          0x6C
#define MPU6500_WHO_AM_I           0x75   // read only; value = 0x70

const int MPU6500_ADDR = 0x68 << 1; // I2C address of the MPU-6050
const int MPU6500_DEVID = 0x70;

//------------------------------------------
//                    VARs
//------------------------------------------

bool available = true;

I2C_TypeDef* i2cx;

int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

typedef enum {
	TWO = 2,      // 00
	FOUR = 4,     // 01
	EIGHT = 8,    // 10
	SIXTEEN = 16  // 11
} AccRange;

AccRange accRange = TWO;

typedef struct {
	uint8_t valid = 0;
	double ax = 0;
	double ay = 0;
	double az = 0;
} MpuData;

MpuData data;

//------------------------------------------
//                    STUFF
//------------------------------------------

uint8_t _checkDeviceIdentity() {
//	uint8_t id = spi_readByte(MPU6500_WHO_AM_I);
	uint8_t id = i2c_readByte(i2cx, MPU6500_ADDR, MPU6500_WHO_AM_I);

//	serial_print("Device identity: 0x");
//	serial_print_hex(id);
//	serial_print("..");

	if (id != MPU6500_DEVID) {
//		serial_println("FAIL!");
		return 0;
	} else {
//		serial_println("OK");
		return 1;
	}
}

void setGRange(AccRange range) {
	uint8_t r = 0;
	switch (range) {
	case TWO:
		r = 0;
		break;
	case FOUR:
		r = 1;
		break;
	case EIGHT:
		r = 2;
		break;
	case SIXTEEN:
		r = 3;
		break;
	}

	i2c_writeByte(i2cx, MPU6500_ADDR, MPU6500_ACC_CONF1, (r << 3) & 0b00011000);
}

void setAccLowPassFilter() {
	i2c_writeByte(i2cx, MPU6500_ADDR, MPU6500_ACC_CONF2, 0b00000100);	// A_DLPF_CFG = 4 -> bandwidth 20Hz
}

void setLowPowerMode() {
	//limit sampling frequency:
	i2c_writeByte(i2cx, MPU6500_ADDR, MPU6500_LP_ACCEL_ODR, 6);	// LPOSC_CLKSEL = 6 -> 15.63Hz output freq.

	// disable GYRO:
	i2c_writeByte(i2cx, MPU6500_ADDR, MPU6500_PWR_MGMT2, 0b10000111);	// LP_WAKE_CTRL = 2 (20Hz) + disable GX, GY, GZ
}

void scaleUp() {
	switch (accRange) {
	case TWO:
		accRange = FOUR;
		break;
	case FOUR:
		accRange = EIGHT;
		break;
	case EIGHT:
		accRange = SIXTEEN;
		break;
	case SIXTEEN:
		return;
	}

	setGRange(accRange);
}

void scaleDown() {
	switch (accRange) {
	case SIXTEEN:
		accRange = EIGHT;
		break;
	case EIGHT:
		accRange = FOUR;
		break;
	case FOUR:
		accRange = TWO;
		break;
	case TWO:
		return;
	}

	setGRange(accRange);
}

void readAndPrintData() {
	const uint8_t NUM_BYTES = 6;	// 14
	uint8_t buf[NUM_BYTES];
	ErrorStatus e = i2c_readBytes(i2cx, MPU6500_ADDR, ACCEL_XOUT_H, NUM_BYTES, buf);
	if (e == SUCCESS) {
		AcX = buf[0] << 8 | buf[1]; // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
		AcY = buf[2] << 8 | buf[3]; // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
		AcZ = buf[4] << 8 | buf[5]; // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
//		Tmp = buf[6] << 8 | buf[7]; // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
//		GyX = buf[8] << 8 | buf[9]; // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
//		GyY = buf[10] << 8 | buf[11]; // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
//		GyZ = buf[12] << 8 | buf[13]; // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

//		serial_println_arr_double("T:", Tmp / 334.0 + 21.0, 2);	//equation for temperature in degrees C from datasheet

		// adjust range if needed:
		if (accRange != SIXTEEN && (AcX < -32700 || AcY < -32700 || AcZ < -32700 || AcX > 32700 || AcY > 32700 || AcZ > 32700)) {
			scaleUp();
			return;
		} else if (accRange != TWO && (AcX > -16000 && AcX < 16000) && (AcY > -16000 && AcY < 16000) && (AcY > -16000 && AcY < 16000)) {
			scaleDown();
			return;
		}

		serial_print("aXYZ: ");
  		double ax = accRange / 32767.0 * AcX;
		serial_print_double(ax, 3);
		serial_print(" | ");
		double ay = accRange / 32767.0 * AcY;
		serial_print_double(ay, 3);
		serial_print(" | ");
		double az = accRange / 32767.0 * AcZ;
		serial_print_double(az, 3);
		serial_print_char('\n');

	} else {
		serial_println("RDF!");
	}
}

/**
 * To be called by Tasker in required intervals.
 */
void readData() {
	const uint8_t NUM_BYTES = 6;	// 14
	uint8_t buf[NUM_BYTES];
	ErrorStatus e = i2c_readBytes(i2cx, MPU6500_ADDR, ACCEL_XOUT_H, NUM_BYTES, buf);
	if (e == SUCCESS) {
		AcX = buf[0] << 8 | buf[1]; // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
		AcY = buf[2] << 8 | buf[3]; // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
		AcZ = buf[4] << 8 | buf[5]; // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
//		Tmp = buf[6] << 8 | buf[7]; // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
//		GyX = buf[8] << 8 | buf[9]; // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
//		GyY = buf[10] << 8 | buf[11]; // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
//		GyZ = buf[12] << 8 | buf[13]; // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

//		double temperature = Tmp / 334.0 + 21.0;	//equation for temperature in degrees C from datasheet

		// adjust range if needed:
		if (accRange != SIXTEEN && (AcX < -32700 || AcY < -32700 || AcZ < -32700 || AcX > 32700 || AcY > 32700 || AcZ > 32700)) {
			scaleUp();
			return;
		} else if (accRange != TWO && (AcX > -16000 && AcX < 16000) && (AcY > -16000 && AcY < 16000) && (AcY > -16000 && AcY < 16000)) {
			scaleDown();
			return;
		}

  		data.ax = accRange / 32767.0 * AcX;
  		data.ay = accRange / 32767.0 * AcY;
  		data.az = accRange / 32767.0 * AcZ;

  		data.valid = 1;

	} else {
		data.valid = 0;
		serial_println("TRD FAIL!");
	}
}

//------------------------------------------
//                    INIT
//------------------------------------------

uint8_t init(I2C_TypeDef* I2Cx) {
	i2cx = I2Cx;
//	spi_writeByte(MPU6500_PWR_MGMT1, 0);	// set to zero - wakes up the MPU-6500
	ErrorStatus err = i2c_writeByte(i2cx, MPU6500_ADDR, MPU6500_PWR_MGMT1, 0);	// set to zero - wakes up the MPU-6500

	if(err == ERROR) {
		available = false;
		return 0;
	}

	uint8_t retVal = _checkDeviceIdentity();

	if (retVal) {
//		setLowPowerMode();
//		setAccLowPassFilter();
		setGRange(accRange);
	}

	return retVal;
}

} //~namespace

#endif /* MPU6500_I2C_H_ */
