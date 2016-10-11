/*
 * coordinator.h
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#ifndef L5_APPLICATION_RCMASTER_COORDINATOR_H_
#define L5_APPLICATION_RCMASTER_COORDINATOR_H_

#include "NodeControl.h"

class coordinator {
public:
	coordinator();
	virtual ~coordinator();
	void on1HzHearbeat();
	bool getNodeStatus();
	void onStatusReceived();
	void onOrderSent();
private:
	NodeControl *itsMotorNode;
	NodeControl *itsSensorNode;
	NodeControl *itsGeoNode;
	NodeControl *itsMobileNode;


};

#endif /* L5_APPLICATION_RCMASTER_COORDINATOR_H_ */