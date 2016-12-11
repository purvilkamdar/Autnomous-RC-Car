/*
 * GPS_Module.cpp
 *
 *  Created on: Dec 2, 2016
 *      Author: ythao
 */

#include "GPS_Module.h"
#include "uart2.hpp"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


GPSModule::GPSModule() {
	message_status = invalid;
	message_counter = 0;
	latitude = 0.000000;
	longitude = 0.000000;
	latitude_offset = 0.000000;
	longitude_offset = 0.000000;
}

GPSModule::~GPSModule() {

}

bool GPSModule::setGPS_Offset(double latitude_offset_, double longitude_offset_) {
	this->latitude_offset = latitude_offset_;
	this->longitude_offset = longitude_offset_;

	return true;
}

bool GPSModule::getGPS_Data(gps_name addr) {
	static Uart2 &u2 = Uart2::getInstance();
	u2.flush();
	char data[128];
	char addrCode[6];
	int i = 0;
	const char parser = ',';

	/* Set memory arrays to zero */
	memset(&data[0],0,sizeof(data));
	memset(&addrCode[0],0,sizeof(addrCode));

	/*  Read string of GPS data into buffer data[] */
	u2.gets(data,128,0);

	//printf("GPS: %s\n",data); //Print out for debugging

	/* Copy GPS Address Code from data[] into array addrCode[] */
	for(i=0; i<5; i++)
		addrCode[i] = data[i+1];

	char *valid_bit;

	/* Compares GPS address type and if true, print desired GPS data */
	if(strcmp(gps_addr[addr],addrCode) == 0)
	{
		/*
		 * Increments each time new GPS data is received.
		 */
		if(this->message_counter >= 60)
			this->message_counter = 0;
		this->message_counter++;

		double temp_Latitude=0;
		double temp_Longitude=0;

		printf("\nGPS Data = %s\n",data);
		strtok(data, &parser);
		//printf("GPS Addr Type: %s\n", addrCode);
		strtok(NULL, &parser);
		valid_bit = strtok(NULL, &parser);

		//Checks to see if GPS data is valid
		//"A" is valid and "V" is invalid
		if(*valid_bit == 'A')
		{
			/*GPS Data is valid*/
			this->message_status = valid;
			temp_Latitude = atof(strtok(NULL, &parser));
			strtok(NULL, &parser);
			temp_Longitude = atof(strtok(NULL, &parser));
			this->latitude = doubleToDecimalDegree(temp_Latitude) + this->latitude_offset;
			this->longitude = (-1)*doubleToDecimalDegree(temp_Longitude) + this->longitude_offset;

//			printf("Valid bit = %d\n", this->GPS_message_status);
//			printf("Counter =   %d\n", this->message_counter);
//			printf("Latitude =  %f\n", this->latitude);
//			printf("Longitude = %f\n", this->longitude);
			}
		else
			{
			/*GPS Data is not valid. Set valid bit, latitude and longitude to 0*/
			this->message_status = invalid;
//			this->latitude = 0;
//			this->longitude = 0;

//			printf("Valid bit = %d\n", this->GPS_message_status);
//			printf("Counter =   %d\n", this->message_counter);
//			printf("Latitude =  %f\n", this->latitude);
//			printf("Longitude = %f\n", this->longitude);
			}
		return true;
		}
	else
		//Either not receiving correct GPS Message Data or no connectivity to GPS.
		return false;
}

double GPSModule::doubleToDecimalDegree(double strDegree) {
	int temp_DD;
	double decimal_Degree;
	double temp_MM;

	//Retrieves the DDD (degree)
	temp_DD = (int)(strDegree*0.01);

	//Calculates the degree from MM.MMMM
	temp_MM = (strDegree - ((float)temp_DD*100))/60;

	//Adds DDD (degree) with the calculated degree from MM.MMM
	decimal_Degree = (float)temp_DD + temp_MM;

	return decimal_Degree;
}

bool GPSModule::updateToManager(geo_data& geo_data_) {

	if (getGPS_Data(GPRMC))
	{
		geo_data_.GPS_message_status = this->message_status;
		geo_data_.GPS_message_counter = this->message_counter;
		geo_data_.latitude = this->latitude;
		geo_data_.longitude = this->longitude;
		return true;
	}
	else
		//Did not receive any GPS data. Does not return any values back to GEO_Manager.
		return false;
}


