/*
 * coordinator.h
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#ifndef L5_APPLICATION_RCMASTER_COORDINATOR_H_
#define L5_APPLICATION_RCMASTER_COORDINATOR_H_

#include "NodeControl.h"
#include "GeoControl.h"
#include "MotorControl.h"
#include "SensorsControl.h"
#include "MobileControl.h"
//#include "singleton_template.hpp"

class coordinator {    //: public SingletonTemplate<coordinator> {
public:

	virtual ~coordinator();
	void on1HzHearbeat();
	bool getNodeStatus();
	void onStatusReceived();
	void processAndSendOrder();
	coordinator();
private:
	MotorControl *itsMotorNode;
	SensorsControl *itsSensorNode;
	GeoControl *itsGeoNode;
	MobileControl *itsMobileNode;

	//friend class SingletonTemplate<coordinator>;

};

#endif /* L5_APPLICATION_RCMASTER_COORDINATOR_H_ */
