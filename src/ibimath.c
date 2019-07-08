/*
 * ibimath.c
 *
 *  Created on: Dec 7, 2016
 *      Author: ibisek
 */

#include "ibimath.h"

double abs_double(double d) {
	if (d < 0)
		return d*= -1;

	return d;
}

//double pow(uint8_t x, uint8_t y) {
//
//	if (y == 1)
//		return x;
//
//	double val = x;
//	for (uint8_t i = 1; i < y; i++) {
//		val *= x;
//	}
//
//	return val;
//}




