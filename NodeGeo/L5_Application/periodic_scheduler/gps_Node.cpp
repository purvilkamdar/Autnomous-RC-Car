/*
 * gps_Node.cpp
 *
 *  Created on: Oct 13, 2016
 *      Author: ythao
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "uart3.hpp"
#include "uart2.hpp"
#include "string.h"
#include "gps_Node.h"
#include "_can_dbc/generated_can.h"
#include "can.h"
#include "math.h"


/* defines 0.000001 decimal degree equals to 0.8865491 centimeter */
#define decimalDegreeToCM 0.8865491

/*
 * Initializes serial ports
 * 	->Uart2 to receive Data from GPS module
 * 	->Uart3 to receive Data from COMPASS module
 * 	->Can1 main Canbus communication port
 * 	->Can2 backup or for Canbus testing
 */
void serialInit(void)
{
	Uart3::getInstance().init(57600,100,100);
	Uart2::getInstance().init(115200,128,100);
	CAN_init(can1,100,5,5,NULL,NULL);
	CAN_init(can2,100,5,5,NULL,NULL);
	const can_std_id_t slist[]  = { CAN_gen_sid(can1, 0x020),   // Acknowledgment from the nodes that received sensor reading
											CAN_gen_sid(can1, 0x021) }; // Only 1 ID is expected, hence small range
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
double angleOfApproach(double start_lat, double start_long, double destination_lat, double destination_long, double current_lat, double current_long)
{
	struct Coordinates{
		double x;
		double y;
	} current_A, start_B, destination_C, vector_P, vector_R;

	double angle = 0;
	double productVectors = 0 ;
	double magnitude_R = 0;
	double magnitude_P = 0;

	//Reassigning input to better visual them as coordinates
	current_A.x = current_lat;
	current_A.y = current_long;
	destination_C.x = destination_lat;
	destination_C.y = destination_long;
	start_B.x = start_lat;
	start_B.y = start_long;

	//Vector CA-> (real car position)  is equal to A-C
	vector_R.x = current_A.x - destination_C.x;
	vector_R.y = current_A.y - destination_C.y;

	//Vector CB-> (ideal path of car position) is equal to B-C
	vector_P.x = start_B.x - destination_C.x;
	vector_P.y = start_B.y - destination_C.y;

	//Product of both Vectors R & P (CA * CB)
	productVectors = (vector_R.x * vector_P.x) + (vector_R.y * vector_P.y);
	//printf("Product of Vectors = %f\n",productVectors);

	//Magnitude of vector R (||CA||)
	magnitude_R = sqrt(pow(vector_R.x,2) + pow(vector_R.y,2));
	//printf("Magnitude_R = %f\n", magnitude_R);

	//Magnitude of vector P (||CB||)
	magnitude_P = sqrt(pow(vector_P.x,2) + pow(vector_P.y,2));
	//printf("Magnitude_p = %f\n\n", magnitude_P);

	//Computes Arc-Cosine and converts results to degrees
	angle = acos(productVectors/(magnitude_R*magnitude_P)) * (180.0 / M_PI);

	return angle;
}

/*
 * Function to take in raw GPS data format DDDMM.MMMM and converts it to Decimal Degree
 * Gets called in readGPS() function after receiving GPS data
 */
uint32_t floatToDecimalDegree(float strDegree)
{
	uint32_t degree;
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
	degree = (uint32_t)(decimal_Degree * 1000000);

	return degree;
}

/*
 * This function takes in the type of GPS Address and GPS_DATA structure
 * The desired GPS data is filtered and parsed into the GPS_DATA structure
 * 		-So far, this function is only optimized to filter GNRMC gps address type
 * 		 which is all we really need.
 */
void readGPS(gps_name addr, GPS_DATA *data_r)
{
	static Uart2 &u2 = Uart2::getInstance();
	u2.flush();
	char data[128];
	char addrCode[6];
	int i = 0;
	const char parser = ',';

	/*
	 * Set memory arrays to zero
	 */
	memset(&data[0],0,sizeof(data));
	memset(&addrCode[0],0,sizeof(addrCode));

	/*
	 * Read string of GPS data into buffer data[]
	 */
	u2.gets(data,128,0);

	//printf("GPS: %s\n",data); //Print out for debugging

	/*
	 * Copy GPS Address Code from data[] into array addrCode[]
	 */
	for(i=0; i<5; i++)
		addrCode[i] = data[i+1];

	char *valid_bit;

	/*
	 * Compares GPS address type and if true, print desired GPS data
	 */
	if(strcmp(gps_addr[addr],addrCode) == 0)
	{
		/*
		 * Increments each time new GPS data is received.
		 */
		if(data_r->counter >= 2000)
				data_r->counter = 0;
		data_r->counter++;

		float temp_Latitude=0;
		float temp_Longitude=0;

		printf("\nGPS Data = %s\n",data);
		strtok(data, &parser);
		printf("GPS Addr Type: %s\n", addrCode);
		printf("GPS Time Type: %s\n", strtok(NULL, &parser));
		valid_bit = strtok(NULL, &parser);

		//Checks to see if GPS data is valid
		//"A" is valid and "V" is invalid
		if(*valid_bit == 'A')
		{
			//GPS Data is valid
			data_r->valid_bit = 1;
			temp_Latitude = atof(strtok(NULL, &parser));
			strtok(NULL, &parser);
			temp_Longitude = atof(strtok(NULL, &parser));
			data_r->latitude = floatToDecimalDegree(temp_Latitude);
			data_r->longitude = floatToDecimalDegree(temp_Longitude);

//			printf("Valid bit = %d\n", data_r->valid_bit);
//			printf("Counter =   %d\n", data_r->counter);
//			printf("Latitude =  %lu\n", data_r->latitude);
//			printf("Longitude = %lu\n", data_r->longitude);
		}
		else
			{
			//GPS Data is not valid

//			data_r->valid_bit = ;
//						temp_Latitude = atof(strtok(NULL, &parser));
//						strtok(NULL, &parser);
//						temp_Longitude = atof(strtok(NULL, &parser));
//						data_r->latitude = floatToDecimalDegree(temp_Latitude);
//						data_r->longitude = floatToDecimalDegree(temp_Longitude);

			data_r->valid_bit = 0;
			data_r->latitude = 0;
			data_r->longitude = 0;
//			printf("Valid bit = %d\n", data_r->valid_bit);
//			printf("Counter =   %d\n", data_r->counter);
//			printf("Latitude =  %lu\n", data_r->latitude);
//			printf("Longitude = %lu\n", data_r->longitude);
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
