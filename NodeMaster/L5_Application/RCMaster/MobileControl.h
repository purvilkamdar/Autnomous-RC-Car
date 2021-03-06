/*
 * MobileControl.h
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#ifndef L5_APPLICATION_RCMASTER_MOBILECONTROL_H_
#define L5_APPLICATION_RCMASTER_MOBILECONTROL_H_

#include "NodeControl.h"

class MobileControl {//: public NodeControl {
public:
	MobileControl();
	virtual ~MobileControl();
	bool getStatus();
	bool sendOrder();
	void setName();
private:
	char* nodeName;
};

#endif /* L5_APPLICATION_RCMASTER_MOBILECONTROL_H_ */
