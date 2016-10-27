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
#include "TrajectoryEngine.h"
#include "order_and_status.h"
//#include "singleton_template.hpp"

#define MOTOR_STATUS 400
#define SENSOR_DATA 16
#define GPS_Data 65
#define APP_START_STOP 1

#define MIN_DISTANCE_INCHES 10
#define MED_DISTANCE_INCHES 15

#define MAX_HEADING_ERR_DEG 45
#define MED_HEADING_ERR_DEG 25

class coordinator {    //: public SingletonTemplate<coordinator> {
public:

	virtual ~coordinator();
	void on1HzHearbeat();
	bool sendHeartbeat(void);
	bool getNodeStatus();
	void onStatusReceived(void);
	void processAndSendOrder(status_t& status, order_t& order);
	coordinator();
	int motor_cmd;
	int steer_cmd;
	status_t status;
	order_t  order;
private:
	MotorControl *itsMotorNode;
	SensorsControl *itsSensorNode;
	GeoControl *itsGeoNode;
	MobileControl *itsMobileNode;
 //comment 2
//=======
	TrajectoryEngine *itsTrajectoryEngine;
 //comment
	//friend class SingletonTemplate<coordinator>;

};

#endif /* L5_APPLICATION_RCMASTER_COORDINATOR_H_ */
