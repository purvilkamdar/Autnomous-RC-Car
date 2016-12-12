/*
 * GPS_Module.h
 *
 *  Created on: Dec 2, 2016
 *      Author: ythao
 */

#ifndef L5_APPLICATION_PERIODIC_SCHEDULER_GPS_MODULE_H_
#define L5_APPLICATION_PERIODIC_SCHEDULER_GPS_MODULE_H_

#include"geo_data.h"

#define GPS C(GNVTG)C(GNGGA)C(GNGSA)C(GPGSV)C(GLGSV)C(GNGLL)C(GNRMC)C(GPRMC)
#define C(x) x,
enum gps_name { GPS CODE };
#undef C
#define C(x) #x,
const char * const gps_addr[] = { GPS };

class GPSModule {
public:
	GPSModule();
	virtual ~GPSModule();
	bool setGPS_Offset(double latitude_offset_, double longitude_offset_);
	bool getGPS_Data(gps_name addr);
	double doubleToDecimalDegree(double strDegree);
	bool updateToManager(geo_data &geo_data_);

private:
	double latitude;
	double longitude;
	Geo_message_status message_status;
	int message_counter;
	double latitude_offset;
	double longitude_offset;

};


#endif /* L5_APPLICATION_PERIODIC_SCHEDULER_GPS_MODULE_H_ */
