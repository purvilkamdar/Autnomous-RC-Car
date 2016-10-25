/*
 * MotorControl.h
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#ifndef L5_APPLICATION_RCMASTER_MOTORCONTROL_H_
#define L5_APPLICATION_RCMASTER_MOTORCONTROL_H_

#include "NodeControl.h"

enum motor_steer {
	left_full,
	left_half,
	center,
	right_half,
	right_full
};

class MotorControl {//: public NodeControl {
public:
	MotorControl();
	virtual ~MotorControl();
	bool getStatus();
	bool sendOrder();
	void setName();
	int motor_cmd;
private:
	char* nodeName;

};

#endif /* L5_APPLICATION_RCMASTER_MOTORCONTROL_H_ */
