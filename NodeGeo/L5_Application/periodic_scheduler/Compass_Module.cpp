/*
 * Compass_Module.cpp
 *
 *  Created on: Dec 2, 2016
 *      Author: ythao
 */

#include "Compass_Module.h"
#include "stdlib.h"
#include <string.h>
#include "uart3.hpp"
#include <stdio.h>

CompassModule::CompassModule() {
	this->compass_Heading = 0;
	this->compass_Offset = 0;
}

bool CompassModule::setCompass_Offset(int offset) {
	compass_Offset = offset;
	return true;
}

bool CompassModule::getCompass_Data() {
	const char s[2] = ",";                        // ","  is delimter used to parse data
	char rx_buff[10];
	char rx_str[10];
	char *temp;

	static Uart3& u3 = Uart3::getInstance();

	memset(&rx_buff[0],0,sizeof(rx_buff));
	memset(&rx_str[0],0,sizeof(rx_str));

	u3.gets(rx_buff, sizeof(rx_buff), 0);         // get data from compass module

	strcpy(rx_str,rx_buff);                       // copy the compass data to a temporary string
	temp = strtok(rx_str,s);                      // Separate the data by ','
	if(temp!= NULL)								  // check if the data valid
	{
	//printf("%s\n",temp);

		compass_Heading = atoi(temp);

		compass_Heading += compass_Offset;

		if(compass_Heading >=360)
			compass_Heading = compass_Heading - 360;
			//printf("%0.2f\n",x);
			//compass_Heading = x;
			//sscanf(temp,"%f",&compass_Heading);
			//printf("Heading from Compass = %i\n",compass_Heading);
		return true;
	}
	else
		//Received no Compass Data. Returns false.
		return false;
}

bool CompassModule::updateToManager(geo_data &geo_data_){

	if(getCompass_Data())
	{
		geo_data_.compass_heading = this->compass_Heading;
		return true;
	}
	else
		//Received no Compass_Data. Nothing to return to Geo_Manager.
		return false;
}

CompassModule::~CompassModule() {

}


