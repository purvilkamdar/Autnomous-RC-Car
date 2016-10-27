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
	void set_thresholds (int max_heading_error_, int med_heading_error_);
    void checkHeading(status_t& status);

private:
	char* nodeName;
	int max_heading_error;
	int med_heading_error;
	int heading_state;
};

enum heading_state {
	off_to_left,
	way_off_to_left,
	off_to_right,
	way_off_to_right,
	on_target
};

#endif /* L5_APPLICATION_RCMASTER_GEOCONTROL_H_ */
