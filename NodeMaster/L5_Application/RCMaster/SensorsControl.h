/*
 * SensorsControl.h
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#ifndef L5_APPLICATION_RCMASTER_SENSORSCONTROL_H_
#define L5_APPLICATION_RCMASTER_SENSORSCONTROL_H_

#include "NodeControl.h"

class SensorsControl : public NodeControl {
public:
	SensorsControl();
	virtual ~SensorsControl();
	void setName(char* name);
};

#endif /* L5_APPLICATION_RCMASTER_SENSORSCONTROL_H_ */
