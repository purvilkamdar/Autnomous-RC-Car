/*
 * coordinator.cpp
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#include "coordinator.h"



coordinator::coordinator() {
	// TODO Auto-generated constructor stub
	itsMotorNode  = new MotorControl();
	itsGeoNode    = new GeoControl();
	itsSensorNode = new SensorsControl();
	itsMobileNode = new MobileControl();

	itsMotorNode->setName();
	itsGeoNode->setName();
	itsSensorNode->setName();
	itsMobileNode->setName();
}



void coordinator::on1HzHearbeat(){
	getNodeStatus();

}

bool coordinator::getNodeStatus(){
	bool status_received = false;
    //GET PREVIOUS STATUS
	//TODO
	// if CAN messages arrived from all nodes status_received = true
	printf("Get CAN previous status messages \n");
	// CAN SEND HEARBEAT TODO
	printf("Hearbeat sent!\n");


	return true;// status_received;   TODO restore testing only
}

void coordinator::onStatusReceived(){
	processAndSendOrder();
}
void coordinator::processAndSendOrder(){
	//TODO
		//
		printf("Master processing order \n");
		//call trajectory engine here TODO

		// then send the order
		if(itsMotorNode->sendOrder()){
			printf("Sending order\n");
		}
}
coordinator::~coordinator() {
	// TODO Auto-generated destructor stub
}

