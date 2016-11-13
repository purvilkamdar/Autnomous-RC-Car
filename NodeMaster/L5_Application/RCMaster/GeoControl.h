/*
 * GeoControl.h
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#ifndef L5_APPLICATION_RCMASTER_GEOCONTROL_H_
#define L5_APPLICATION_RCMASTER_GEOCONTROL_H_

#include "NodeControl.h"
#include <vector>

#define MIN_TARGET_DIST 3

struct app_checkpoints_t {
	float app_lat;
	float app_long;
};
class GeoControl {//: public NodeControl {
public:
	GeoControl();
	virtual ~GeoControl();
	bool getStatus();
	bool sendOrder();
	void setName();
	void set_thresholds (int max_heading_error_, int med_heading_error_);
    void checkHeading(status_t& status);
    bool sendCheckpoints(app_checkpoints_t& checkpoints);
    void getCheckpoints(status_t& status);
    bool extractCheckpoints(order_t& order,status_t& status);
    bool start_iteration();
    bool on_target_reached(status_t& status);
    bool is_last_checkpoint();
private:
	char* nodeName;
	int max_heading_error;
	int med_heading_error;
	int heading_state;
	app_checkpoints_t checkpoints;
	app_checkpoints_t send_checkpoints;
	std::vector<app_checkpoints_t> app_checkpoints;
	std::vector<app_checkpoints_t>::iterator iterate_checkpoints;
};

enum heading_state {
	off_to_left,
	way_off_to_left,
	off_to_right,
	way_off_to_right,
	on_target
};

#endif /* L5_APPLICATION_RCMASTER_GEOCONTROL_H_ */
