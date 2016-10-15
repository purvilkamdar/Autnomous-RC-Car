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
