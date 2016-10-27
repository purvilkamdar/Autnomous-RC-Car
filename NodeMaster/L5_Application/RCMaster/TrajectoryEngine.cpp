/*
 * TrajectoryEngine.cpp
 *
 *  Created on: Oct 26, 2016
 *      Author: home
 */

#include <RCMaster/TrajectoryEngine.h>

TrajectoryEngine::TrajectoryEngine() {
	// TODO Auto-generated constructor stub
	current_state = idle;

}

TrajectoryEngine::~TrajectoryEngine() {
	// TODO Auto-generated destructor stub
}

void TrajectoryEngine::run_trajectory (status_t& status, order_t& order) {

	switch (current_state){
	case (idle):
         if (status.app_cmd == drive && all_sensors_clear(status) ){
        	 next_state = forward;
         }
	     order.steer_order = center;
	     order.speed_order = full_stop;
			break;
	case (forward):

		order.steer_order = center;
	    order.speed_order = slow_speed;
		// NOTE: the priority of decisions is encoded in the if-then statements order
		// we first check the heading and set direction
		/// However, obstable avoidance takes precedence,
	    //  and the subsequent if-then obstacle avoidance statements
	    // override the outcome of heading choice if necessary


		if (status.heading_state == way_off_to_left){
			next_state = hard_right;
		} else if (status.heading_state == way_off_to_right) {
			next_state = hard_left;
		} else if (status.heading_state == off_to_left) {
			next_state = soft_right;
		} else if (status.heading_state == off_to_right) {
			next_state = soft_left;
		}

		// if all sensors are blocked, stop
		if (status.left_state == too_close && status.center_state == too_close && status.right_state == too_close){
			next_state = idle;
		}
	    // go right if left or center are too close
		else if (status.left_state == too_close || status.center_state == too_close)	{
			next_state = hard_right;
		}
		else if (status.right_state == too_close) {
			next_state = hard_left;
		}
		else if (status.left_state == medium || status.center_state == medium)	{
			next_state = soft_right;
		}
		else if (status.right_state == medium) {
			next_state = soft_left;
		}

		if (status.app_cmd == stop) next_state = idle;
			break;
	// on the turn states we just wait to recover
    // and return to forward
    //if subsequent turn decisions are needed, it will be decided in forward state
	case (soft_right):

		order.steer_order = right_half;
	    order.speed_order = slow_speed;
		if (status.left_state == clear && status.heading_state != off_to_left){
			next_state = forward;
		}
		if (status.app_cmd == stop) next_state = idle;
			break;
	case (hard_right):

		order.steer_order = right_full;
	    order.speed_order = slow_speed;
		if (status.left_state != too_close && status.heading_state != way_off_to_left){
			next_state = forward;
		}
		if (status.app_cmd == stop) next_state = idle;
		break;

	case (soft_left):

		order.steer_order = left_half;
	    order.speed_order = slow_speed;

		if (status.right_state != medium && status.heading_state != off_to_right){
			next_state = forward;
		}
		if (status.app_cmd == stop) next_state = idle;
		break;
	case (hard_left):

		order.steer_order = left_full;
	    order.speed_order = slow_speed;
		if (status.right_state != too_close && status.heading_state != way_off_to_right){
			next_state = forward;
		}
		if (status.app_cmd == stop) next_state = idle;
		break;
	default:
		break;
	}

}


bool TrajectoryEngine::all_sensors_clear(status_t& status){
	return status.center_state == clear && status.left_state == clear && status.right_state == clear;
}

