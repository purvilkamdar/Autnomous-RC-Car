/*
 * coordinator.cpp
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#include <RCMaster/coordinator.h>
#include "GeoControl.h"
#include "MotorControl.h"
#include "SensorsControl.h"
#include "MobileControl.h"

coordinator::coordinator() {
	// TODO Auto-generated constructor stub
	itsMotorNode  = new MotorControl();
	itsGeoNode    = new GeoControl();
	itsSensorNode = new SensorsControl();
	itsMobileNode = new MobileControl();
}

void coordinator::on1HzHearbeat(){

}

coordinator::~coordinator() {
	// TODO Auto-generated destructor stub
}

