/*
 * GeoControl.cpp
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#include <RCMaster/GeoControl.h>

GeoControl::GeoControl() {
	// TODO Auto-generated constructor stub
	max_heading_error = -1;
	med_heading_error = -1;
}

void GeoControl::setName(){
    strcpy(nodeName, "GeoNode");
}

GeoControl::~GeoControl() {
	// TODO Auto-generated destructor stub
}

void GeoControl::set_thresholds (int max_heading_error_, int med_heading_error_){
	max_heading_error = max_heading_error_;
	med_heading_error = med_heading_error_;
}


void GeoControl::checkHeading(status_t& status){
    //TODO check real heading error here
	//int heading_error =  wayPointdirection - status.compass_heading;
	int heading_error =  0;

	if(heading_error < ((-1)*max_heading_error)){
		status.heading_state = way_off_to_left;
	}
	else if(heading_error <((-1)*med_heading_error ))
	{
       status.heading_state = off_to_left;
	}
	else if(heading_error > max_heading_error){
		status.heading_state = way_off_to_right;
	}
	else if(heading_error  > med_heading_error)
	{
       status.heading_state = off_to_right;
	} else {
		status.heading_state = on_target;
	}



}
