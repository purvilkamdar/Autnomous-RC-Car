/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * This contains the period callback functions for the periodic scheduler
 *
 * @warning
 * These callbacks should be used for hard real-time system, and the priority of these
 * tasks are above everything else in the system (above the PRIORITY_CRITICAL).
 * The period functions SHOULD NEVER block and SHOULD NEVER run over their time slot.
 * For example, the 1000Hz take slot runs periodically every 1ms, and whatever you
 * do must be completed within 1ms.  Running over the time slot will reset the system.
 */

#include <stdint.h>
#include <stdio.h>
#include "string.h"
#include "io.hpp"
#include "periodic_callback.h"
#include "file_logger.h"
#include "uart3.hpp"
#include "gps_Node.h"
#include "_can_dbc/generated_can.h"
#include "can.h"
#include "compass.hpp"

COMPASS_Data_t COMPASS_Value = {0};
GPS_Data_t gps_rx_data = {0};
MASTER_HB_t master_hb = {0};

const uint32_t         MASTER_HB__MIA_MS = 1000;
const MASTER_HB_t      MASTER_HB__MIA_MSG = {0};

/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

/**
 * This is the stack size of the dispatcher task that triggers the period tasks to run.
 * Minimum 1500 bytes are needed in order to write a debug file if the period tasks overrun.
 * This stack size is also used while calling the period_init() and period_reg_tlm(), and if you use
 * printf inside these functions, you need about 1500 bytes minimum
 */
const uint32_t PERIOD_DISPATCHER_TASK_STACK_SIZE_BYTES = (512 * 3);

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
	serialInit();
    return true; // Must return true upon success
}

/// Register any telemetry variables
bool period_reg_tlm(void)
{
    // Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry
    return true; // Must return true upon success
}


/**
 * Below are your periodic functions.
 * The argument 'count' is the number of times each periodic task is called.
 */

void period_1Hz(uint32_t count)
{
	check_reset_canbus();
}

void period_10Hz(uint32_t count)
{

	COM_DATA compass = {0};
	static GPS_DATA data_received = {0};
	can_msg_t can_msg;

	/*---Gets compass Data---*/
	get_compass_data(&compass);

	COMPASS_Value.COMPASS_Heading = compass.Com_head;

	/*---Gets GPS Data---*/
	get_GPS(GPRMC, &data_received);

	gps_rx_data.GPS_READOUT_valid_bit = data_received.valid_bit;
		//printf("Valid Bit = %d\n",gps_rx_data.GPS_READOUT_valid_bit);
	gps_rx_data.GPS_READOUT_read_counter = data_received.counter;
		//printf("GPS Counter = %d\n",gps_rx_data.GPS_READOUT_read_counter);
	gps_rx_data.GPS_READOUT_latitude = data_received.latitude;
		//printf("GPS Latitude = %f\n",gps_rx_data.GPS_READOUT_latitude);
	gps_rx_data.GPS_READOUT_longitude = data_received.longitude;
		//printf("GPS Longitude = %f\n",gps_rx_data.GPS_READOUT_longitude);

/* --------Sending to Canbus Periodically for anyone node to read------------*/
	dbc_encode_and_send_GPS_Data(&gps_rx_data);
	dbc_encode_and_send_COMPASS_Data(&COMPASS_Value);
/*-----------------END Canbus message force send--------------------*/

/* ---------- This area is for manual testing of angle and distance using a fixed coordinate from Google Map and readings from GPS module --------*/
	//takes coordinates (start(x,y) , destination(x,y), current(x,y))
	double angle = angleOfError(data_received.latitude,data_received.longitude,37.336610,-121.881404,compass.Com_head);
	//double angle = angleOfError(37.069436,-120.85369,37.069213,-120.853929,227);
	printf("\nAngle of approach is: %f\n", angle);
	double distance = distanceToTargetLocation(data_received.latitude,data_received.longitude,37.336610,-121.881404);
	//double distance = distanceToTargetLocation(37.069436,-120.85369,37.069134,-120.854213);
	int int_distance = (int)distance;

	if(int_distance >= 99)
		LD.setNumber(99);
	else
		LD.setNumber(int_distance);

	printf("Distance away from destination: %f meter(s) \n\n", distance);

/*-------------------------END of manual testing code-----------------------------------*/

/*	Polls for master's HB message. Once received, sends Compass and GPS data out */
/*-----NEED TO DO: need MASTER's msg id to retrieve next checkpoint GPS coordinate.The received coordinates will be used to calculate---*/
/*-----------------both angle of error and distance and then sends the data back to the MASTER using GEO's msg id 67------*/

	while(CAN_rx(can1, &can_msg,0))
	{
		dbc_msg_hdr_t can_msg_hdr;
		can_msg_hdr.dlc = can_msg.frame_fields.data_len;
		can_msg_hdr.mid = can_msg.msg_id;

		if(dbc_decode_MASTER_HB(&master_hb, can_msg.data.bytes, &can_msg_hdr))
		{
			double angle = angleOfError(data_received.latitude,data_received.longitude,37.071419,-120.85464,compass.Com_head);
			printf("\nAngle of approach is: %f\n", angle);
			double distance = distanceToTargetLocation(data_received.latitude,data_received.longitude,37.071419,-120.85464);
			printf("Distance away from destination: %f meter(s) \n\n", distance);



			int int_distance = (int)distance;

				if(int_distance >= 99)
					LD.setNumber(99);
				else
					LD.setNumber(int_distance);
		}
	}

//	if(dbc_handle_mia_MASTER_HB(&master_hb,100))
//	{
//		printf("In MIA State.\n");
//		LE.toggle(3);
//	}


/*
	//Quick Canbus Test using on board Switches & LEDs
	can_msg_t can_msg2;

	if(SW.getSwitch(1))
	{
		LE.toggle(1);
		sendCan1_Any_Message(0x111, 0, 1, 1);
	}

	can_msg_t can_receiver_test;

	if(CAN_rx(can2,&can_receiver_test,0))
	{
		LE.toggle(4);
	}
*/
}

void period_100Hz(uint32_t count)
{

}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{

}
