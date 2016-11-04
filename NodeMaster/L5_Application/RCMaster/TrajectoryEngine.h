/*
 * TrajectoryEngine.h
 *
 *  Created on: Oct 26, 2016
 *      Author: home
 */

#ifndef L5_APPLICATION_RCMASTER_TRAJECTORYENGINE_H_
#define L5_APPLICATION_RCMASTER_TRAJECTORYENGINE_H_

#include "order_and_status.h"
#include "GeoControl.h"
#include "SensorsControl.h"

enum trajectoryStates {
	idle,                // 0
	forward,             //1
	backward,             //2
	soft_right,             //3
	hard_right,             //4
	soft_left,             //5
	hard_left,             //6
	soft_right_center,             //7
	hard_right_center,             //8
	soft_left_center,             //9
	hard_left_center             //0
};



class TrajectoryEngine {
public:
	TrajectoryEngine();
	virtual ~TrajectoryEngine();
	void run_trajectory (status_t& status, order_t& order);
	void set_thresholds (int min_sensor_dist_, int med_sensor_dist_, int max_heading_error_, int med_heading_error_);
	bool all_sensors_clear(status_t& status);

private:
	int current_state;
	int next_state;
	int min_sensor_dist;
	int med_sensor_dist;
	int max_heading_error;
	int med_heading_error;
	void checkStop(status_t status);
};

#endif /* L5_APPLICATION_RCMASTER_TRAJECTORYENGINE_H_ */
