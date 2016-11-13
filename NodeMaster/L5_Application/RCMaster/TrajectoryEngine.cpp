/*
 * TrajectoryEngine.cpp
 *
 *  Created on: Oct 26, 2016
 *      Author: home
 */

#include <RCMaster/TrajectoryEngine.h>
#include "io.hpp"

TrajectoryEngine::TrajectoryEngine() {
	// TODO Auto-generated constructor stub
	current_state = idle;
	last_reverse_dir = false;

}

TrajectoryEngine::~TrajectoryEngine() {
	// TODO Auto-generated destructor stub
}

void TrajectoryEngine::run_trajectory (status_t& status, order_t& order) {
	static int debug_ctr = 0;
#ifdef NO_APP
	if (SW.getSwitch(1)) { /* Check if button 1 is pressed */
		printf("button 1\n");
		status.app_cmd = drive;          /* Turn on LED # 1              */
	}
	else if (SW.getSwitch(2)){
		printf("button 2\n");
		status.app_cmd = stop;
	}
#endif

	switch (current_state){
	case (idle): // TODO remove comment when app is used
      if (status.app_cmd == drive )
      {
         next_state = forward;
         if ( status.center_state == too_close){
        	 next_state = backward;
         }
       }
	     order.steer_order = center;
	     order.speed_order = full_stop;
	break;
	case (backward):
	if (!(status.center_state == too_close ))
	{
		next_state = forward;
	}
    if (last_reverse_dir){
    	order.steer_order = hard_left;
    } else {
    	order.steer_order = hard_right;
    }
    order.speed_order = reverse;
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


		// if all center is blocked, go back
		if (status.center_state == too_close ){
			next_state = backward;
			last_reverse_dir = !last_reverse_dir;
		}
		// if both sensors are in the same threshold levels,
		// pick the closest to clear
		else if (status.rightCloser && status.right_state != clear){
			//  only turn if not clear
			if (status.right_state == too_close){
				next_state = hard_left;
			} else if (status.right_state == medium) {
				next_state = soft_left;
			}

		} else if (status.leftCloser && status.left_state != clear){
			// only turn if not clear
			if (status.left_state == too_close){
				next_state = hard_right;
			} else if (status.left_state == medium){
				next_state = soft_right;
			}

		} 
		else if ((status.center_state == medium)){
				  if ((status.leftCloser) && (status.right_state != too_close)){
				    next_state = soft_right_center;
				  } else if (status.left_state != too_close){
				    next_state = soft_left_center;
				  }
				}
		else if (status.left_state == medium )	{
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
	    // if right sensor is closer then left, or right clear but heading not off to left, stop turning
		if ((status.rightCloser) || (status.left_state == clear && status.heading_state != off_to_left))
		{
			next_state = forward;
		}
		// if all sensors are blocked, stop
		checkStop(status);

		break;

	case (soft_right_center):

		order.steer_order = right_half;
	    order.speed_order = slow_speed;
	    // if right sensor is closer then left, stop turning
	    if ((status.rightCloser) || (status.center_state == clear && status.heading_state != off_to_left))
	    {
			next_state = forward;
		}
		// if all sensors are blocked, stop
		checkStop(status);

		break;

	case (hard_right):

		order.steer_order = right_full;
	    order.speed_order = slow_speed;
	    // if right sensor is closer then left, stop turning
	    if ((status.rightCloser) || (status.left_state != too_close && status.heading_state != way_off_to_left))
	    {
			next_state = forward;
		}

		// if all sensors are blocked, stop
		checkStop(status);

		break;
	case (hard_right_center):
		order.steer_order = right_full;
		order.speed_order = slow_speed;
	    // if right sensor is closer then left, stop turning
	    if ((status.rightCloser) || (status.center_state == clear && status.heading_state != off_to_left))
	    {
			next_state = forward;
		}
		// if all sensors are blocked, stop
		checkStop(status);

		break;
	case (soft_left):

		order.steer_order = left_half;
	    order.speed_order = slow_speed;
	    // if left sensor is closer then right, stop turning
	    if ((status.leftCloser) || (status.right_state == clear && status.heading_state != off_to_right))
		{
			next_state = forward;
		}
		// if all sensors are blocked, stop
		checkStop(status);

		break;
	case (soft_left_center):
		order.steer_order = left_half;
	    order.speed_order = slow_speed;
	    // if left sensor is closer then right, stop turning
	    if ((status.leftCloser) || (status.center_state == clear && status.heading_state != off_to_left))
	    {
			next_state = forward;
		}
		// if all sensors are blocked, stop
		checkStop(status);

		break;
	case (hard_left):

		order.steer_order = left_full;
	    order.speed_order = slow_speed;
	    // if left sensor is closer then right, stop turning
	    if ((status.leftCloser) || (status.right_state != too_close && status.heading_state != way_off_to_right))
	    {
			next_state = forward;
		}
		// if all sensors are blocked, stop
		checkStop(status);
		break;
	case (hard_left_center):
		order.steer_order = left_full;
	    order.speed_order = slow_speed;
	    // if left sensor is closer then right, stop turning
	    if ((status.leftCloser) || (status.center_state == clear && status.heading_state != off_to_left))
	    {
			next_state = forward;
	    }
		// if all sensors are blocked, stop
		checkStop(status);

		break;
	default:
		next_state = idle;
		break;
	}

    LD.setNumber(current_state);
	current_state=next_state;
//printf("Trajectory current State : %d \n",current_state);
//printf("Trajectory next State : %d \n",next_state);
}

void TrajectoryEngine::checkStop(status_t status){
	if (status.center_state == too_close)
	{
		next_state = backward;
	}
	if (status.app_cmd == stop) next_state = idle;
}

bool TrajectoryEngine::all_sensors_clear(status_t& status){
	return status.center_state == clear && status.left_state == clear && status.right_state == clear;
}

