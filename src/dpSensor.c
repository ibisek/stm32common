/*
 * dpSensor.c
 *
 * Library for differential pressure sensor Measurement Specialties 4525DO-DS5AI001DP.
 *
 *  Created on: Nov 23, 2016
 *      Author: ibisek
 */

#include <dpSensor.h>

DiffPSensorState dp;

//TODO remove:
int16_t bridgeData;

uint8_t beforeCalibration;
uint16_t beforeCalibrationCountdown;

void _dp_clearDPStructure() {
	dp.dataReady = 0;

	dp.ongoingCalibration = 0;
	dp.calibrationSamplesCount = 0;
	dp.calibrationPressureSum = 0.0;

	dp.status = -1;
}

void dp_measurementRequest() {
	i2c_readByte(DIFF_P_SENSOR_ADDR, 0);
}

void dp_printSensorStatus(uint8_t status) {
	switch (status) {
	case 0:
		serial_print("OK");
		break;
	case 2:
		serial_print("stale");
		break;
	case 3:
		serial_print("FAULT");
		break;
	}
}

void dp_readSensorData() {
	dp.dataReady = 0;

	ErrorStatus res = i2c_readBytes(DIFF_P_SENSOR_ADDR, 0, DIFF_P_SENSOR_DATA_LEN, dp.buffer);
	if (res == ERROR) {
		serial_println("Error reading DP data!");
		dp.dataReady = 1;
		dp.status = 3;	// 3 = FAIL
		return;
	}

#if TRACE
	serial_print("RAW: ");
	for (uint8_t i = 0; i < DIFF_P_SENSOR_DATA_LEN; i++) {
		if (dp.buffer[i] <= 0x0F)
		serial_print_char('0');
		serial_print_hex(dp.buffer[i]);
		serial_print_char(' ');
	}
	serial_print_char('\n');
#endif

	// PRESSURE DELTA:
	dp.status = (dp.buffer[0] & 0xC0) >> 6;
	bridgeData = ((dp.buffer[0] & 0x3F) << 8) | (dp.buffer[1]);
	// 0psi = 1638
	// P_SENSOR_RANGEpsi = 14746
	dp.pressureDelta = (bridgeData - 1638.0) * (DIFF_P_SENSOR_RANGE / (14746.0 - 1638.0));
	dp.pressureDelta = dp.pressureDelta * 6894.76;  // conversion psi -> Pa

	if (beforeCalibration) {
		if (--beforeCalibrationCountdown == 0) {
			beforeCalibration = 0;
			dp.ongoingCalibration = 1;	// commence calibration
		}

		return; // no printouts before calibration

	} else if (dp.ongoingCalibration) {
		dp.calibrationSamplesCount++;
		dp.calibrationPressureSum += dp.pressureDelta;

		if (dp.calibrationSamplesCount >= DIFF_P_SENSOR_NUM_CALIBRATION_SAMPLES) {
			dp.pressureOffset = dp.calibrationPressureSum / dp.calibrationSamplesCount;

			serial_print("done. pressureOffset = ");
			serial_print_double(dp.pressureOffset, 0);
			serial_print(" Pa\n\n");

			dp.calibrationDone = 1;
			dp.ongoingCalibration = 0;
		}

		return; // no printouts when calibrating
	}

	dp.pressureDelta = dp.pressureDelta - dp.pressureOffset;

	// TEMPERATURE:
	int16_t rawTemperature = ((dp.buffer[2]) << 3) | (dp.buffer[3] >> 5);
	dp.temperature = (rawTemperature - 511.0) * (85.0 / (1381.0 - 511.0));

	dp.dataReady = 1;
}

void dp_printValues() {
	// sensor status:
	serial_print_int(dp.status);
	serial_print_char(';');

	// differential sensor data:
	serial_print_double(dp.temperature, 1);   // [deg. C]
	serial_print_char(';');
	serial_print_double(dp.pressureDelta, 1); // [Pa]
	serial_print_char('\n');
}

DiffPSensorState* dp_getState() {
	return &dp;
}

void dp_init() {
	_dp_clearDPStructure();

	beforeCalibrationCountdown = NUM_SAMPLES_BEFORE_CALIBRATION;
	beforeCalibration = 1; // let the sensor stabilise after power on

	serial_print("\nCalibrating differential pressure sensor..");
}

