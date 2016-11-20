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

/*
 * Switches between test-code and run-code. 1 for test code. 0 for run code.
 */
#define TEST_GPS 1
#define test_dest_latitude 37.336829
#define test_dest_longitude -121.882407

COMPASS_Data_t COMPASS_Value = {0};
GPS_Data_t gps_rx_data = {0};
GEO_Header_t geo_data = {0};
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
//	I2C2& i2c = I2C2::getInstance();
//	uint8_t byte[1] = {0x02};
//	i2c.writeRegisters(LCD_ADDR,0x02,byte,1);

	COM_DATA compass = {0};
	static GPS_DATA data_received = {0};

	/*---Gets compass Data---*/
	get_compass_data(&compass);

	COMPASS_Value.COMPASS_Heading = compass.Com_head;

	/*---Gets GPS Data---*/
	get_GPS(GPRMC, &data_received);

	/*---Assigns GPS data to GPS_READOUT header---*/
	gps_rx_data.GPS_READOUT_valid_bit = data_received.valid_bit;
	gps_rx_data.GPS_READOUT_read_counter = data_received.counter;
	gps_rx_data.GPS_READOUT_latitude = data_received.latitude;
	gps_rx_data.GPS_READOUT_longitude = data_received.longitude;

	/*---Sending to Canbus Periodically for all to read---*/
	dbc_encode_and_send_GPS_Data(&gps_rx_data);
	dbc_encode_and_send_COMPASS_Data(&COMPASS_Value);


#if TEST_GPS
/* ---------- This area is for manual testing of angle and distance using a fixed coordinate from Google Map and readings from GPS module --------*/
	//takes coordinates (GPS_Data, nextwaypoint(lat,long), compass_header)
	//data_received.latitude = 0;
	//data_received.longitude = 0;
	double angle = angleOfError(&data_received,test_dest_latitude,test_dest_longitude,compass.Com_head);
	printf("\nAngle of approach is: %f\n", angle);
	double distance = distanceToTargetLocation(&data_received,test_dest_latitude,test_dest_longitude);

	printf("Distance away from destination: %f meter(s) \n\n", distance);

	/*---Assigns calculated angle and distance values to GEO dbc header---*/
	geo_data.GPS_ANGLE_degree = angle;
	geo_data.GPS_DISTANCE_meter = distance;
	/*---Sends angle and distance data onto CAN bus---*/
	dbc_encode_and_send_GEO_Header(&geo_data);

#else
/*	Polls for master's HB message. Once received, sends Compass and GPS data out */
/*-----NEED TO DO: need MASTER's msg id to retrieve next checkpoint GPS coordinate.The received coordinates will be used to calculate---*/
/*-----------------both angle of error and distance and then sends the data back to the MASTER using GEO's msg id 67------*/

	can_msg_t can_msg;

	while(CAN_rx(can1, &can_msg,0))
	{
		dbc_msg_hdr_t can_msg_hdr;
		can_msg_hdr.dlc = can_msg.frame_fields.data_len;
		can_msg_hdr.mid = can_msg.msg_id;

		if(dbc_decode_MASTER_HB(&master_hb, can_msg.data.bytes, &can_msg_hdr))
		{
			double error_angle = angleOfError(&data_received,master_hb.MASTER_LAT_cmd,master_hb.MASTER_LONG_cmd,compass.Com_head);
			printf("\nAngle of approach is: %f\n", error_angle);
			double distance_to_checkpoint = distanceToTargetLocation(&data_received,master_hb.MASTER_LAT_cmd,master_hb.MASTER_LONG_cmd);
			printf("Distance away from destination: %f meter(s) \n\n", distance_to_checkpoint);

			/*---Assigns calculated angle and distance values to GEO dbc header---*/
			geo_data.GPS_ANGLE_degree = error_angle;
			geo_data.GPS_DISTANCE_meter = distance_to_checkpoint;
			/*---Sends angle and distance data onto CAN bus---*/
			dbc_encode_and_send_GEO_Header(&geo_data);
		}
	}

	/*----MIA handler for MASTER HB signal. Toggles 1 & 4 LEDs on board when in MIA state---*/
	if(dbc_handle_mia_MASTER_HB(&master_hb,100))
	{
		//printf("In MIA State.\n");
		LE.toggle(1);
		LE.toggle(4);
	}

#endif


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
