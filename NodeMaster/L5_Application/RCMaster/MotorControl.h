/*
 * MotorControl.h
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#ifndef L5_APPLICATION_RCMASTER_MOTORCONTROL_H_
#define L5_APPLICATION_RCMASTER_MOTORCONTROL_H_

#include "NodeControl.h"

class MotorControl : public NodeControl {
public:
	MotorControl();
	virtual ~MotorControl();
	bool getStatus();
	bool sendOrder();
	void setName(char* name);
};

#endif /* L5_APPLICATION_RCMASTER_MOTORCONTROL_H_ */
