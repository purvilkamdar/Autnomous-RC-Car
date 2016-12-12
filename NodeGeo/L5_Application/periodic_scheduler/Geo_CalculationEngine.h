/*
 * Geo_CalculationEngine.h
 *
 *  Created on: Dec 4, 2016
 *      Author: ythao
 */

#ifndef L5_APPLICATION_PERIODIC_SCHEDULER_GEO_CALCULATIONENGINE_H_
#define L5_APPLICATION_PERIODIC_SCHEDULER_GEO_CALCULATIONENGINE_H_

#include "geo_data.h"

class Geo_CalculationEngine{
public:
	Geo_CalculationEngine();
	virtual ~Geo_CalculationEngine();
	void deviation_filter_controller(geo_data geo_data_, calculated_geo_data &calculated_geo_data_);
	void linear_regression_computer(calculated_geo_data &calculated_geo_data_);
	double distanceToTargetLocation(geo_data geo_data_, master_data master_data_);
	double angleOfError(geo_data geo_data_, master_data master_data_);
	bool distanceToTargetLocation_wCalculatedGeoData(calculated_geo_data &calculated_geo_data_, master_data master_data_);
	bool angleOfError_wCalculatedGeoData(calculated_geo_data &calculated_geo_data_, master_data master_data_, geo_data geo_data_);

private:
	double distance_Magnitude;
	double angleError_ToNorth;

};



#endif /* L5_APPLICATION_PERIODIC_SCHEDULER_GEO_CALCULATIONENGINE_H_ */
