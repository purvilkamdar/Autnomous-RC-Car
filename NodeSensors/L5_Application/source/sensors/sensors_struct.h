/*
 * sensors_struct.h
 *
 *  Created on: Nov 3, 2016
 *      Author: Sathishkumar
 */

#ifndef L5_APPLICATION_SOURCE_SENSORS_SENSORS_STRUCT_H_
#define L5_APPLICATION_SOURCE_SENSORS_SENSORS_STRUCT_H_

/* 21 buckets - 6.5 meters(256 inches) is the max range of sonic sensors.
 256 inches is divided into 12 inch ranges - total 21 buckets (12 * 21 ~ 252 inches) */
#define BUCKETS 21

typedef struct SD
{
	uint8_t mean;
	uint8_t val[5];
	float variance;
	unsigned int sum;
}SD;

typedef struct ModeFilter
{
int sum[BUCKETS];
int count[BUCKETS];
int MAX;
int INDEX;
int filtered_val;
}ModeFilter;



#endif /* L5_APPLICATION_SOURCE_SENSORS_SENSORS_STRUCT_H_ */
