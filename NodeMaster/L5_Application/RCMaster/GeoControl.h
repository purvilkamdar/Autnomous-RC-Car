/*
 * GeoControl.h
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#ifndef L5_APPLICATION_RCMASTER_GEOCONTROL_H_
#define L5_APPLICATION_RCMASTER_GEOCONTROL_H_

#include "NodeControl.h"

class GeoControl {//: public NodeControl {
public:
	GeoControl();
	virtual ~GeoControl();
	bool getStatus();
	bool sendOrder();
	void setName();
private:
	char* nodeName;
};

#endif /* L5_APPLICATION_RCMASTER_GEOCONTROL_H_ */
