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

GPS_DATA data_received = {0};

const uint32_t                             LIGHT_SENSOR__MIA_MS = 3000;
const LIGHT_SENSOR_t                       LIGHT_SENSOR__MIA_MSG = {99};

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
	logger_init(1);
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

/*
 * Available GPS Address Types:
 * GNVTG GNGGA GNGSA GPGSV GLGSV GNGLL GNRMC
 */

void period_1Hz(uint32_t count)
{
	check_reset_canbus();
}

void period_10Hz(uint32_t count)
{
//	LIGHT_SENSOR_t light_sensor_data = {0};
//	can_msg_t can_msg;
//
//	light_sensor_data.LIGHT_SENSOR_value = LS.getPercentValue();
//	dbc_msg_hdr_t msg_hdr = dbc_encode_LIGHT_SENSOR(can_msg.data.bytes, &light_sensor_data);
//	can_msg.msg_id = msg_hdr.mid;
//	can_msg.frame_fields.data_len = msg_hdr.dlc;
//	printf("Sensor data sending out is = %d\n\n",light_sensor_data.LIGHT_SENSOR_value);
//	printf("Message ID = %d\n",can_msg.msg_id);
//	printf("Data = %d\n", can_msg.data.bytes[0]);
//
//	CAN_tx(can1, &can_msg,0);

//Homeowrk assignment
//
//	MASTER_LIGHT_RQ_t master_light_request = {0};
//	LIGHT_SENSOR_t light_sensor_data = {0};
//	can_msg_t can_msg;
//	can_msg_t can_msg_rx_from_master;
//	can_msg_t can_msg_rx_from_sensor;
//
//	dbc_encode_and_send_MASTER_LIGHT_RQ(&master_light_request);
//	printf("Master requests sent.\n");
//	LE.toggle(2);
//	LE.toggle(3);
//
//	while(CAN_rx(can2, &can_msg_rx_from_master, 0))
//	{
//		printf("From Sensor Side:\n");
//		printf("Master message received.\n");
//		dbc_msg_hdr_t can_msg_hdr_sensor;
//		can_msg_hdr_sensor.dlc = can_msg_rx_from_master.frame_fields.data_len;
//		can_msg_hdr_sensor.mid = can_msg_rx_from_master.msg_id;
//
//		if(dbc_decode_MASTER_LIGHT_RQ(&master_light_request, can_msg_rx_from_master.data.bytes, &can_msg_hdr_sensor))
//		{
//			light_sensor_data.LIGHT_SENSOR_value = LS.getPercentValue();
//			dbc_msg_hdr_t msg_hdr = dbc_encode_LIGHT_SENSOR(can_msg.data.bytes, &light_sensor_data);
//			can_msg.msg_id = msg_hdr.mid;
//			can_msg.frame_fields.data_len = can_msg_hdr_sensor.dlc;
//			printf("Sensor data sending out is = %d\n\n",light_sensor_data.LIGHT_SENSOR_value);
//
//			CAN_tx(can2, &can_msg,0);
//		}
//	}
//
//	while(CAN_rx(can1, &can_msg_rx_from_sensor, 0))
//	{
//		printf("From Master Side:\n");
//		printf("Sensor message received.\n");
//		dbc_msg_hdr_t can_msg_hdr_master;
//		can_msg_hdr_master.dlc = can_msg_rx_from_sensor.frame_fields.data_len;
//		can_msg_hdr_master.mid = can_msg_rx_from_sensor.msg_id;
//
//		dbc_decode_LIGHT_SENSOR(&light_sensor_data, can_msg_rx_from_sensor.data.bytes, &can_msg_hdr_master);
//
//		if(can_msg_hdr_master.mid == LIGHT_SENSOR_HDR.mid )
//		{
//			printf("Light sensor data received = %d\n\n", light_sensor_data.LIGHT_SENSOR_value);
//			LD.setNumber(light_sensor_data.LIGHT_SENSOR_value);
//		}
//
//	}

//	if( dbc_handle_mia_LIGHT_SENSOR(&light_sensor_data, 100))
//		LD.setNumber(LIGHT_SENSOR__MIA_MSG.LIGHT_SENSOR_value);

// Home work //

  //Group Project Code
	MASTER_HB_t master_hb = {};
	GPS_HB_t gps_hb = {0x1};
	GPS_DATA_t gps_rx_data = {0};
	can_msg_t can_msg;
	can_msg_t can_msg2;

	readGPS(GNRMC, &data_received);

	gps_rx_data.GPS_READOUT_valid_bit = data_received.valid_bit;
	gps_rx_data.GPS_READOUT_read_counter = data_received.counter;
	gps_rx_data.GPS_READOUT_latitude = data_received.latitude;
	gps_rx_data.GPS_READOUT_longitude = data_received.longitude;

	while(CAN_rx(can1, &can_msg,0))
	{
		dbc_msg_hdr_t can_msg_hdr;
		can_msg_hdr.dlc = can_msg.frame_fields.data_len;
		can_msg_hdr.mid = can_msg.msg_id;

		//Checks to see if msg ID matches Mater's HB ID. If so, send GPS HB.
		if(dbc_decode_MASTER_HB(&master_hb, can_msg.data.bytes, &can_msg_hdr))
		{
			dbc_encode_and_send_GPS_DATA(&gps_rx_data);
		}

		/*
		if(dbc_decode_MASTER_()
		{
			dbc_encode_and_send_GPS_DATA(&gps_rx_data);
		}
		*/

	}


/*
	//Quick Canbus Test using on board Switches & LEDs
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
