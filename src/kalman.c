/*
 * KalmanFilter.cpp
 *
 *  Created on: Nov 23, 2016
 *      Author: ibisek
 */

#include "kalman.h"

/**
 * Constructor.
 */
void kalman_init(KalmanData* data) {
	data->f_1 = 1.00000; //cast as double
	data->q = 4.0001; //filter parameters, you can play around with them
	data->r = .20001; // but these values appear to be fairly optimal
	data->x = 0;
	data->p = 0;
	data->x_last = 0;
	data->p_last = 0;
	data->k = 0;
}

double kalman_calc(KalmanData* data, double value) {
	//Predict x_temp, p_temp
	double x_temp = data->x_last;
	double p_temp = data->p_last + data->r;

	//Update kalman values
	data->k = (data->f_1 / (p_temp + data->q)) * p_temp;
	data->x = x_temp + (data->k * (value - x_temp));
	data->p = (data->f_1 - data->k) * p_temp;

	//Save this state for next time
	data->x_last = data->x;
	data->p_last = data->p;

	return data->x;
}
