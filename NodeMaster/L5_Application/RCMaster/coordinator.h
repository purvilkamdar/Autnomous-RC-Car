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

#define MOTOR_STATUS 400
#define SENSOR_DATA 16
#define GPS_Data 65
#define APP_START_STOP 1

#define MIN_DISTANCE_INCHES 12

class coordinator {    //: public SingletonTemplate<coordinator> {
public:

	virtual ~coordinator();
	void on1HzHearbeat();
	bool sendHeartbeat(int motor_cmd,int steer_cmd);
	bool getNodeStatus();
	void onStatusReceived();
	void processAndSendOrder();
	coordinator();
	int motor_cmd;
	int steer_cmd;
private:
	MotorControl *itsMotorNode;
	SensorsControl *itsSensorNode;
	GeoControl *itsGeoNode;
	MobileControl *itsMobileNode;
 //comment
	//friend class SingletonTemplate<coordinator>;

};

#endif /* L5_APPLICATION_RCMASTER_COORDINATOR_H_ */
