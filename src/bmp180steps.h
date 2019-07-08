/*
 * bmp180steps.h
 *
 *  Created on: Dec 06, 2016
 *      Author: ibisek
 *
 * @version 2016-11-04 for STM32
 */

#ifndef BMP180STEPS_H_
#define BMP180STEPS_H_


#ifdef STM32F0xx
#include "stm32f0xx.h"
#else
	error "Yet unsupported architecture"
#endif

#include "bmp180.h"

#include "tasker.h"
#include "serial.h"

extern double T, P;
extern uint8_t newTPSampleSet;

// forward declarations:
void bmp180_step2(uint8_t param);
void bmp180_step3(uint8_t param);


void bmp180_step1(uint8_t param) {
//	swSerial_println("Ps.1");
	uint8_t status = bmp180_startTemperature();
	if (status > 0) {
		tasker_setTimeout((TaskCallback)bmp180_step2, status, 0);

	} else {
		serial_println("BMP180 step 1 err");
	}

}

void bmp180_step2(uint8_t param) {
//	swSerial_println("Ps.2");
	uint8_t status = bmp180_getTemperature(&T);
	if (status != 0) {
		status = bmp180_startPressure(3);
		if (status != 0) {
			tasker_setTimeout((TaskCallback)bmp180_step3, status, 0);

		} else {
			serial_println("BMP180 step 2 err");
		}
	}
}

void bmp180_step3(uint8_t param) {
//	swSerial_println("Ps.3");
	uint8_t status = bmp180_getPressure(&P, &T);
	if (status != 0) {
		newTPSampleSet = 1;

	} else {
		serial_println("BMP180 step 3 err");
	}

}

#endif /* BMP180STEPS_H_ */
