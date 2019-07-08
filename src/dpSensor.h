/*
 * dpSensor.h
 *
 * Library for differential pressure sensor Measurement Specialties 4525DO-DS5AI001DP.
 *
 *  Created on: Nov 23, 2016
 *      Author: ibisek
 */

#ifndef DPSENSOR_H_
#define DPSENSOR_H_

#include "i2c.h"

//-----------------------------------------------------
//										CONFIGURATION
//-----------------------------------------------------

#define DIFF_P_SENSOR_ADDR 0x28 << 1	// diff P sensor
#define DIFF_P_SENSOR_DATA_LEN 4
#define DIFF_P_SENSOR_RANGE 2.0	// [psi] <-1; 1> = 2 psi

// Number of samples after power on and before calibration:
// (it' seems like there is a filter in the sensor that needs some time to stabilise on a zero value).
#define NUM_SAMPLES_BEFORE_CALIBRATION 300	// MAX 65535 (uint16_t)

// Number of samples for the zero offset calibration:
#define DIFF_P_SENSOR_NUM_CALIBRATION_SAMPLES 200	// MAX 255 (uint8_t)

#define DIFF_P_SAMPLE_INTERVAL 100 // 10/s
#define DIFF_P_CALIBRATION_SAMPLE_INTERVAL 10


//-----------------------------------------------------
//										DATA TYPES
//-----------------------------------------------------

typedef struct Diff_P_Sensor_State {
	uint8_t buffer[DIFF_P_SENSOR_DATA_LEN];

	uint8_t dataReady;

	uint8_t ongoingCalibration;
	uint8_t calibrationDone;

	double calibrationPressureSum;
	uint8_t calibrationSamplesCount;
	double pressureOffset;  // [Pa]

	double temperature;
	double pressureDelta;
	uint8_t status; // 0 = OK; 1 reserved; 2 = data already read (stale data); 3 = FAIL
} DiffPSensorState;

//-----------------------------------------------------
//										INTERFACE
//-----------------------------------------------------

void dp_measurementRequest();
void dp_printSensorStatus(uint8_t status);
void dp_readSensorData();
DiffPSensorState* dp_getState();

void _dp_clearDPStructure();
void dp_init();


#endif /* DPSENSOR_H_ */
