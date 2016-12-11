/*
 * Compass_Module.h
 *
 *  Created on: Dec 2, 2016
 *      Author: ythao
 */

#ifndef L5_APPLICATION_PERIODIC_SCHEDULER_COMPASS_MODULE_H_
#define L5_APPLICATION_PERIODIC_SCHEDULER_COMPASS_MODULE_H_

#include"geo_data.h"

class CompassModule {
public:
	CompassModule();
	virtual ~CompassModule();
	bool setCompass_Offset(int offset);
	bool getCompass_Data();
	bool updateToManager(geo_data &geo_data_);

private:
	int compass_Heading;
	int compass_Offset;

};



#endif /* L5_APPLICATION_PERIODIC_SCHEDULER_COMPASS_MODULE_H_ */
