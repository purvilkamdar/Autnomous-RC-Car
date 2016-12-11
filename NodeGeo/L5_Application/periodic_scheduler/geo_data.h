/*
 * geo_data.h
 *
 *  Created on: Dec 2, 2016
 *      Author: ythao
 */

#ifndef L5_APPLICATION_PERIODIC_SCHEDULER_GEO_DATA_H_
#define L5_APPLICATION_PERIODIC_SCHEDULER_GEO_DATA_H_

#include <stdint.h>

#define arr_size 20

enum Geo_message_status{
	invalid,
	valid
};

enum Geo_circularArr{
	lat_Arr,
	long_Arr,
	time_Arr
};

typedef struct geo_data{
	Geo_message_status GPS_message_status;
	int GPS_message_counter;
	double latitude;
	double longitude;
	int compass_heading;
}geo_data;

typedef struct master_data{
	double m_latitude;
	double m_longitude;
}master_data;

typedef struct calculated_geo_data{
	int arr_index;
	uint64_t counter;
	double geo_circularArr [3][arr_size];
	double c_latitude;
	double c_longitude;
	float c_distanceToLocation;
	float c_errorAngle;
	double c_lat_mean;
	double c_long_mean;
}calculated_geo_data;

#endif /* L5_APPLICATION_PERIODIC_SCHEDULER_GEO_DATA_H_ */
