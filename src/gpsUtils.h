/*
 * gpsUtils.h
 *
 *  Created on: Apr 21, 2017
 *      Author: ibisek
 */

#include <stdint.h>
#include <math.h>

#ifndef GPSUTILS_H_
#define GPSUTILS_H_

/**
 * @param lat1 in radians
 * @param lon1 in radians
 * @param lat2 in radians
 * @param lon2 in radians
 * @return distance in meters
 */
double calcDistance(double lat1, double lon1, double lat2, double lon2) {
	double arg = sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lon2 - lon1);
	double distance = acos(arg) * 6378 * 1000;	// [m]

	return distance;
}

/**
 * @param lat1 in radians
 * @param lat2 in radians
 * @return distance in direction north-south. If P2 is south-wards from the P1 , the distance is negative
 */
double calcLatDistance(double lat1, double lat2) {
	double arg = sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2);
	double dist = acos(arg) * 6378 * 1000;	// [m]

	return (lat2 < lat1 ? -1 * dist : dist);
}

/**
 * @param lat1 or lat2
 * @param lon1 in radians
 * @param lon2 in radians
 * @return distance in direction east-west. If the point P2 is wet-wards from P1, the distance is negative.
 */
double calcLonDistance(double lat1, double lon1, double lon2) {
	double arg = sin(lat1) * sin(lat1) + cos(lat1) * cos(lat1) * cos(lon2 - lon1);
	double dist = acos(arg) * 6378 * 1000;	// [m]

	return (lon2 < lon1 ? -1 * dist : dist);
}

#endif /* GPSUTILS_H_ */
