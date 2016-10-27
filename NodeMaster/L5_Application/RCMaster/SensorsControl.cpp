/*
 * SensorsControl.cpp
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#include <RCMaster/SensorsControl.h>

SensorsControl::SensorsControl() {
	// TODO Auto-generated constructor stub

}

SensorsControl::~SensorsControl() {
	// TODO Auto-generated destructor stub
}

void SensorsControl::setName(){
    strcpy(nodeName, "SensorsNode");
}


void SensorsControl::check_sensors(status_t& status){

	// start with all clear
	center_state = clear;
	right_state = clear;
	left_state = clear;

	// center
	if (status.sensor_center > med_sensor_dist) {
		center_state = clear;
	} else if (status.sensor_center < min_sensor_dist) {
		center_state = too_close;
	} else {
		center_state = medium;
	}

	// right
	if (status.sensor_right > med_sensor_dist) {
		right_state = clear;
	} else if (status.sensor_right < min_sensor_dist) {
		right_state = too_close;
	} else {
		right_state = medium;
	}

	// left
	if (status.sensor_left > med_sensor_dist) {
		left_state = clear;
	} else if (status.sensor_left < min_sensor_dist) {
		left_state = too_close;
	} else {
		left_state = medium;
	}

	// populate the status message
	status.center_state = center_state;
	status.right_state = right_state;
	status.left_state = left_state;

}


void SensorsControl::set_thresholds (int min_sensor_dist_, int med_sensor_dist_){
	min_sensor_dist = min_sensor_dist_;
	med_sensor_dist = med_sensor_dist_;
}
