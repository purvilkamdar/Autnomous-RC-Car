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
	idle,
	forward,
	soft_right,
	hard_right,
	soft_left,
	hard_left
};



class TrajectoryEngine {
public:
	TrajectoryEngine();
	virtual ~TrajectoryEngine();
	void run_trajectory (status_t& status, order_t& order);
	void set_thresholds (int min_sensor_dist_, int med_sensor_dist_, int max_heading_error_, int med_heading_error_);
	bool all_sensors_clear(status_t& status);
	friend int calculate_next_state(status_t& status);

private:
	int current_state;
	int next_state;
	int min_sensor_dist;
	int med_sensor_dist;
	int max_heading_error;
	int med_heading_error;
};

#endif /* L5_APPLICATION_RCMASTER_TRAJECTORYENGINE_H_ */
