/*
 * NodeControl.h
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#ifndef L5_APPLICATION_RCMASTER_NODECONTROL_H_
#define L5_APPLICATION_RCMASTER_NODECONTROL_H_

#include <stdio.h>

#define MAX_NAME_CHARS 50

class NodeControl {
public:
	NodeControl();
	virtual ~NodeControl();
	virtual bool getStatus();
	virtual bool sendOrder();
protected:
	virtual void setName (char* name) = 0;
private :
	char* nodeName[MAX_NAME_CHARS];

};

#endif /* L5_APPLICATION_RCMASTER_NODECONTROL_H_ */
