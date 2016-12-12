/*
 * gps_Node.cpp
 *
 *  Created on: Oct 13, 2016
 *      Author: ythao
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "io.hpp"
#include "uart3.hpp"
#include "uart2.hpp"
#include "i2c2.hpp"
#include "string.h"
#include "gps_Node.h"
#include "_can_dbc/generated_can.h"
#include "can.h"
#include "math.h"

/*
 * Initializes serial ports
 * 	->Uart2 to receive Data from GPS module
 * 	->Uart3 to receive Data from COMPASS module
 * 	->Can1 main Canbus communication port
 * 	->Can2 backup or for Canbus testing
 */
void serialInit(void)
{
	I2C2::getInstance().init(100);
	Uart3::getInstance().init(57600,1,0);
	Uart2::getInstance().init(115200,75,0);
	CAN_init(can1,100,5,5,NULL,NULL);
	CAN_init(can2,100,5,5,NULL,NULL);
	const can_std_id_t slist[]  = { CAN_gen_sid(can1, 0x020),  // Acknowledgment from the nodes that received sensor reading
									CAN_gen_sid(can1, 0x021)}; // Only 1 ID is expected, hence small range
	CAN_setup_filter(slist, 2, NULL, 0, NULL, 0, NULL, 0);
	CAN_reset_bus(can1);
	CAN_reset_bus(can2);
}

/*
 * Function to check if Canbus is off. If so, turn is back on
 * Runs in the 1Hz periodic function
 */
void check_reset_canbus(void)
{
	if(CAN_is_bus_off(can1))
		CAN_reset_bus(can1);
	if(CAN_is_bus_off(can2))
		CAN_reset_bus(can2);
}


/*
 * Function to take in 3 GPS positions(start, destination, current) and calculates
 * 	the car's current angle of approach torwards destination point in reference to path between start and destination point.
 *
 * This functions uses vectors to find the arc-cosine of the angle
 */
double angleOfError(GPS_DATA *gps_data, double destination_lat, double destination_long, double compass_angle)
{
	struct Coordinates{
		double x;
		double y;
	} current_A, projection_B, destination_C, vector_P, vector_R;

	if(gps_data->valid_bit)
	{
	double angle = 0;
	double angle_abs_difference = 0;
	double angle_final = 0;
	double compass_new = 0;
	double productVectors = 0 ;
	double magnitude_R = 0;
	double magnitude_P = 0;

	/*---Reassigning input to better visual them as coordinates---*/
	current_A.x = gps_data->longitude;
	current_A.y = gps_data->latitude;
	destination_C.x = destination_long * (-1);
	destination_C.y = destination_lat;
	projection_B.x = gps_data->longitude;
	projection_B.y = gps_data->latitude + 0.1;

	/*---Vector AB-> (vector of car position to its destination)  is equal to A-C ---*/
	vector_R.x = projection_B.x - current_A.x;
	vector_R.y = projection_B.y - current_A.y;

	/*---Vector AC-> (vector of car position to north) is equal to B-C ---*/
	vector_P.x = destination_C.x - current_A.x;
	vector_P.y = destination_C.y - current_A.y;

	/*---Product of both Vectors R & P (CA * CB)---*/
	productVectors = (vector_R.x * vector_P.x) + (vector_R.y * vector_P.y);
	//printf("Product of Vectors = %f\n",productVectors);

	/*---Magnitude of vector R (||CA||)---*/
	magnitude_R = sqrt(pow(vector_R.x,2) + pow(vector_R.y,2));
	//printf("Magnitude_R = %f\n", magnitude_R);

	/*---Magnitude of vector P (||CB||)---*/
	magnitude_P = sqrt(pow(vector_P.x,2) + pow(vector_P.y,2));
	//printf("Magnitude_p = %f\n\n", magnitude_P);

	/*---Computes Arc-Cosine and converts results to degrees. Result is angle between north vector and next destination vector---*/
	angle = acos(productVectors/(magnitude_R*magnitude_P)) * (180.0 / M_PI);

	/*---Next 20 lines of code determines if next destination is East or West then determines to turn left or right---*/
	/*---Checks if destination_longitude is less than current_longitude and change sign of angle to mark if destination is on East(+) or West(-) side.---*/
	if(destination_C.x < current_A.x)
		angle = angle*(-1);
	printf("Angle with respect to North = %f\n", angle);
	/*---Perform an offset to compass heading once it reads over 180 degree. This is to help next calculation.---*/
	if(compass_angle > 180)
		compass_new = (-1)*(360 - compass_angle);
	else
		compass_new = compass_angle;

	/*---Checks which side (East or West) next destination is and then calculate the angle the car needs to turn (left or right).---*/
	if(angle < 0)
	{
		angle_abs_difference = fabs(angle - compass_new);

		if(angle_abs_difference > 180)
			angle_final = 360.0 - angle_abs_difference;
		else
			angle_final = angle - compass_new;
	}
	else
	{
		angle_abs_difference = fabs(angle - compass_new);

		if(angle_abs_difference > 180)
			angle_final = angle_abs_difference - 360.0;
		else
			angle_final = angle - compass_new;
	}

	return angle_final;
	}
	else
		return 0;
}


/*
 * Function takes in two GPS coordinates then calculates the actual distance in Meter between them.
 * This is used to monitor the distance between the car and its targeted location.
 */
double distanceToTargetLocation(GPS_DATA *gps_data, double destination_lat, double destination_long)
{
    struct Coordinates{
		double x;
		double y;
	} current_A, destination_B, vector_R;

	if (gps_data->valid_bit)
	{
	double distance = 0;
	double magnitude = 0;

	current_A.x = gps_data->latitude;
	current_A.y = gps_data->longitude;
	destination_B.x = destination_lat;
	destination_B.y = destination_long * (-1);

	vector_R.x = current_A.x - destination_B.x;
	vector_R.y = current_A.y - destination_B.y;

	magnitude = sqrt(pow(vector_R.x,2)+pow(vector_R.y,2));
	//printf("Magnitude = %f\n", magnitude);

	distance = (magnitude/decimalDegrees)*meterPerDecimalDegree(gps_data->latitude);

	/*---Updates 8-segment display to show current distance to checkpoint (in meters)---*/
	if((int)distance >= 99)
		LD.setNumber(99);
	else
		LD.setNumber((int)distance);

	return distance;

	}
	else
		{
		LD.setLeftDigit('F');
		LD.setRightDigit('F');
		return 5555.5555;
		}
}

/*
 * Function to take in raw GPS data format DDDMM.MMMM and converts it to Decimal Degree
 * Gets called in readGPS() function after receiving GPS data
 */
float floatToDecimalDegree(float strDegree)
{
//	uint32_t degree;
	float decimal_Degree;
	int temp_DD;
	float temp_MM;

	//Retrieves the DDD (degree)
	temp_DD = (int)(strDegree*0.01);

	//Calculates the degree from MM.MMMM
	temp_MM = (strDegree - ((float)temp_DD*100))/60;

	//Adds DDD (degree) with the calculated degree from MM.MMM
	decimal_Degree = (float)temp_DD + temp_MM;

	//Converts float decimal_Degree into correct unint32 format. DBC file will revert it back to correct float value.
	//degree = (uint32_t)(decimal_Degree * 1000000);

	return decimal_Degree;
}

/*
 * This function takes in the type of GPS Address and GPS_DATA structure
 * The desired GPS data is filtered and parsed into the GPS_DATA structure
 * 		-So far, this function is only optimized to filter GPRMC gps address type
 * 		 which is all we really need.
 *
 * Available GPS Address Types(gps_name):
 * GNVTG GNGGA GNGSA GPGSV GLGSV GNGLL GNRMC GPRMC
 * Types are available depending on GPS module and if the messages are enabled on the GPS chip
 */
void get_GPS(gps_name addr, GPS_DATA *data_r)
{
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
		if(data_r->counter >= 60)
				data_r->counter = 0;
		data_r->counter++;

		float temp_Latitude=0;
		float temp_Longitude=0;

		//printf("\nGPS Data = %s\n",data);
		strtok(data, &parser);
		//printf("GPS Addr Type: %s\n", addrCode);
		strtok(NULL, &parser);
		//printf("GPS Time Type: %s\n", strtok(NULL, &parser));
		valid_bit = strtok(NULL, &parser);

		//Checks to see if GPS data is valid
		//"A" is valid and "V" is invalid
		if(*valid_bit == 'A')
		{
			/*GPS Data is valid*/
			data_r->valid_bit = 1;
			temp_Latitude = atof(strtok(NULL, &parser));
			strtok(NULL, &parser);
			temp_Longitude = atof(strtok(NULL, &parser));
			data_r->latitude = floatToDecimalDegree(temp_Latitude) + LATTIDUE_OFFSET;
			data_r->longitude = (-1)*floatToDecimalDegree(temp_Longitude) + LONGITUDE_OFFSET;

//			printf("Valid bit = %d\n", data_r->valid_bit);
////			printf("Counter =   %d\n", data_r->counter);
//			printf("Latitude =  %f\n", data_r->latitude);
//			printf("Longitude = %f\n", data_r->longitude);
		}
		else
			{
			/*GPS Data is not valid. Set valid bit, latitude and longitude to 0*/
			data_r->valid_bit = 0;
//			data_r->latitude = 0;
//			data_r->longitude = 0;
//			printf("Valid bit = %d\n", data_r->valid_bit);
////			printf("Counter =   %d\n", data_r->counter);
//			printf("Latitude =  %f\n", data_r->latitude);
//			printf("Longitude = %f\n", data_r->longitude);
			}
	}
}

/*
 * This function is used by GPS node coupled with functions from generated code to send messages onto the bus
 */
bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
    can_msg_t can_msg = { 0 };
    can_msg.msg_id                = mid;
    can_msg.frame_fields.data_len = dlc;
    memcpy(can_msg.data.bytes, bytes, dlc);

    return CAN_tx(can1, &can_msg, 0);
}

/*
 * Just another quick function to send a message onto Canbus
 * Mostly use it for Canbus testing and troubleshooting only.
 */
void sendCan1_Any_Message(uint32_t id, uint32_t frame, uint32_t data_len, uint64_t data)
{
can_msg_t msg;

msg.frame = frame;
msg.msg_id = id;
msg.frame_fields.is_29bit = 0;
msg.frame_fields.data_len = data_len;
msg.data.qword = data;
CAN_tx(can1, &msg, 0);
}
