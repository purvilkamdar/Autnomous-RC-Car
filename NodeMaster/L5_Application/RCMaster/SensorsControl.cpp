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

void SensorsControl::printThresholds(void){
	/*printf("med_sensor_dist: %d\n", med_sensor_dist);
	printf("min_sensor_dist: %d\n", min_sensor_dist);
	printf("side_med_sensor_dist: %d\n", side_med_sensor_dist);
	printf("side_min_sensor_dist: %d\n", side_min_sensor_dist);
*/
}

void SensorsControl::check_sensors(status_t& status){

	static int debug_ctr = 0;
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
	if (status.sensor_right > side_med_sensor_dist) {
		right_state = clear;
	} else if (status.sensor_right < side_min_sensor_dist) {
		right_state = too_close;
	} else {
		right_state = medium;
	}

	// left
	if (status.sensor_left > side_med_sensor_dist) {
		left_state = clear;
	} else if (status.sensor_left < side_min_sensor_dist) {
		left_state = too_close;
	} else {
		left_state = medium;
	}

	// We need to prioritize if both sensors are at the same state
	status.leftCloser = status.sensor_left < status.sensor_right;
	status.rightCloser = !status.leftCloser;

	// populate the status message
	status.center_state = center_state;
	status.right_state = right_state;
	status.left_state = left_state;
    if (debug_ctr++ > 10){
    	/*printf("status.center_state = %d\n",center_state);
    	printf("status.right_state = %d \n",right_state);
    	printf("status.left_state = %d \n",left_state);*/
    	printThresholds();
    	debug_ctr = 0;
    }



}




void SensorsControl::set_thresholds (int min_sensor_dist_, int med_sensor_dist_, int side_min_sensor_dist_, int side_med_sensor_dist_ ){
	min_sensor_dist = min_sensor_dist_;
	med_sensor_dist = med_sensor_dist_;
	side_min_sensor_dist = side_min_sensor_dist_;
	side_med_sensor_dist = side_med_sensor_dist_;
}
