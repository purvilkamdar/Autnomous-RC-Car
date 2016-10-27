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
#include "string.h"
#include "gps_Node.h"
#include "_can_dbc/generated_can.h"
#include "can.h"

/*
 * Initializes serial ports
 * 	->Uart3 to receive Data from GPS module
 * 	->Can1 main Canbus communication port
 * 	->Can2 backup or for Canbus testing
 */
void serialInit(void)
{
	Uart3::getInstance().init(9600,128,128);
	CAN_init(can1,100,5,5,NULL,NULL);
	CAN_init(can2,100,5,5,NULL,NULL);
	CAN_reset_bus(can1);
	CAN_reset_bus(can2);
	CAN_bypass_filter_accept_all_msgs();
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
 * This function takes in the type of GPS Address and GPS_DATA structure
 * The desired GPS data is filtered and parsed into the GPS_DATA structure
 * 		-So far, this function is only optimized to filter GNRMC gps address type
 * 		 which is all we really need.
 */
void readGPS(gps_name addr, GPS_DATA *data_r)
{
	static Uart3 &u3 = Uart3::getInstance();
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
	u3.gets(data,128,100);

	/*
	 * Copy GPS Address Code from data[] into array addrCode[]
	 */
	for(i=0; i<5; i++)
		addrCode[i] = data[i+1];

	char *valid_bit;

	/*
	 * Compare GPS address and if true, print desired GPS data
	 */
	if(strcmp(gps_addr[addr],addrCode) == 0)
	{
		/*
		 * Increments each time new GPS data is received.
		 */
		if(data_r->counter >= 2000)
				data_r->counter = 0;
		data_r->counter++;

		printf("\nGPS Data = %s\n",data);
		strtok(data, &parser);
		printf("GPS Addr Type: %s\n", addrCode);
		printf("GPS Time Type: %s\n", strtok(NULL, &parser));
		valid_bit = strtok(NULL, &parser);
		if(*valid_bit == 'A')
		{
			//GPS Data is valid
			data_r->valid_bit = 1;
			data_r->latitude = atof(strtok(NULL, &parser));
//			strtok(NULL, &parser);
//			strtok(NULL, &parser);
			data_r->longitude = atof(strtok(NULL, &parser));

			printf("Valid bit = %d\n", data_r->valid_bit);
			printf("Counter =   %d\n", data_r->counter);
			printf("Latitude =  %f\n", data_r->latitude);
			printf("Longitude = %f\n", data_r->longitude);
		}
		else
			{
			//GPS Data is not valid
			data_r->valid_bit = 0;
			data_r->latitude = 0;
			data_r->longitude = 0;
			printf("Valid bit = %d\n", data_r->valid_bit);
			printf("Counter =   %d\n", data_r->counter);
			printf("Latitude =  %f\n", data_r->latitude);
			printf("Longitude = %f\n", data_r->longitude);
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
