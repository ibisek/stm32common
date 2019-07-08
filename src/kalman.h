/*
 * KalmanFilter.h
 *
 *  Created on: Nov 23, 2016
 *      Author: ibisek
 */

#ifndef KALMAN_H_
#define KALMAN_H_

typedef struct Kalman_Data {
	double f_1;
	double x;
	double x_last;
	double p;
	double p_last;
	double k;
	double q;
	double r;
} KalmanData;

void kalman_init(KalmanData* data);
double kalman_calc(KalmanData* data, double value);

#endif /* KALMAN_H_ */
