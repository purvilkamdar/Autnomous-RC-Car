/*
 * NodeControl.cpp
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#include <RCMaster/NodeControl.h>

NodeControl::NodeControl() {
	// TODO Auto-generated constructor stub
    nodeName = NULL;
}

bool NodeControl::getStatus(){
	printf("Getting status from node %s\n", nodeName);
	return true;
}

bool NodeControl::sendOrder(){
	printf("Sending order to node %s\n", nodeName);
	return true;
}



NodeControl::~NodeControl() {
	// TODO Auto-generated destructor stub
}

