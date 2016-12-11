/*
 * Geo_Manager.h
 *
 *  Created on: Dec 2, 2016
 *      Author: ythao
 */

#ifndef L5_APPLICATION_PERIODIC_SCHEDULER_GEO_MANAGER_H_
#define L5_APPLICATION_PERIODIC_SCHEDULER_GEO_MANAGER_H_

#include <stdint.h>
#include "GPS_Module.h"
#include "Compass_Module.h"
#include "Geo_CalculationEngine.h"
#include "geo_data.h"

class GeoManager {
public:
	GeoManager();
	virtual ~GeoManager();
	geo_data geo_data_ = {};
	master_data master_data_ = {};
	calculated_geo_data calculated_geo_data_ = {};

	void serialInit(void);
	bool retrieve_Geo_Data();
	bool filterAndcompute_Geo_Data();
	bool getFilter_DistanceAndAngle();
	bool run_GeoNode();
	void check_reset_canbus(void);
	bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8]);

private:
	GPSModule *my_GPS_module;
	CompassModule *my_Compass_module;
	Geo_CalculationEngine *my_GeoCalculator_engine;
};



#endif /* L5_APPLICATION_PERIODIC_SCHEDULER_GEO_MANAGER_H_ */
