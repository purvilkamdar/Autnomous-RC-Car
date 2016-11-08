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
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "io.hpp"
#include "periodic_callback.h"
#include "uart2.hpp"
#include "can.h"
#include "_can_dbc/generated_can.h"
/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

/**
 * This is the stack size of the dispatcher task that triggers the period tasks to run.
 * Minimum 1500 bytes are needed in order to write a debug file if the period tasks overrun.
 * This stack size is also used while calling the period_init() and period_reg_tlm(), and if you use
 * printf inside these functions, you need about 1500 bytes minimum
 */
const uint32_t PERIOD_DISPATCHER_TASK_STACK_SIZE_BYTES = (512 * 3);
Uart2 &u2 = Uart2::getInstance();
char msg[10];
bool start = 0;
bool stop = 0;
int MIA_Unit=0;
int left=99,right=99,center=99,rear=99;
float latitude=0,longitude=0,speed=0;
extern const uint32_t MASTER_HB__MIA_MS = 3000;
extern const MASTER_HB_t MASTER_HB__MIA_MSG = { 0 };
extern const uint32_t SENSOR_DATA__MIA_MS = 3000;
extern const SENSOR_DATA_t SENSOR_DATA__MIA_MSG = { 100,100,100 };
extern const uint32_t GPS_Data__MIA_MS = 3000;
extern const GPS_Data_t GPS_Data__MIA_MSG = { 0.000000, 0.000000 };
extern const uint32_t MOTOR_STATUS__MIA_MS=3000;
extern const MOTOR_STATUS_t MOTOR_STATUS__MIA_MSG={0};


MASTER_HB_t heartbeat = { 0 };
SENSOR_DATA_t sensordata={0};
GPS_Data_t gpsdata={0};
MOTOR_STATUS_t motorstatus={0};
can_msg_t can_msg;

bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8]) {
	can_msg = { 0 };
	can_msg.msg_id = mid;
	can_msg.frame_fields.data_len = dlc;
	memcpy(can_msg.data.bytes, bytes, dlc);

	return CAN_tx(can1, &can_msg, 0);
}
void busoff(uint32_t arg) {
	LE.on(3);
	if(CAN_is_bus_off(can1))
	{
		CAN_reset_bus(can1);
		LE.off(3);
	}
}
void overrun(uint32_t arg) {
	LE.on(4);
}

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void) {
	can_void_func_t busOff = busoff;
	can_void_func_t overr = overrun;
	u2.init(115200);
	CAN_init(can1, 100, 10, 10, busOff, overr);
	CAN_bypass_filter_accept_all_msgs();
	CAN_reset_bus(can1);
	return true; // Must return true upon success
}

/// Register any telemetry variables
bool period_reg_tlm(void) {
	// Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry
	return true; // Must return true upon success
}

/**
 * Below are your periodic functions.
 * The argument 'count' is the number of times each periodic task is called.
 */

void period_1Hz(uint32_t count) {
	//LE.toggle(1);


}

void period_10Hz(uint32_t count) {



	char* data=new char[20];
	sprintf(data,"L%02dR%02dC%02dB%02dSPD%0.3f",left,right,center,rear,speed);
	u2.put(data, 0);
	sprintf(data,"LAT=%16.6f",latitude);
	//u2.put(data,0);
	sprintf(data,"LON=%16.6f",longitude);
	//u2.put(data,0);
	delete[] data;
	APP_START_STOP_t start_cmd = { 0 };
	if (start == 1) {
		start_cmd.APP_START_STOP_cmd = 1;

	} else if (stop == 1) {
		start_cmd.APP_START_STOP_cmd = 0;

	}

	while (CAN_rx(can1, &can_msg, 0)) {
		dbc_msg_hdr_t can_msg_hdr;
		can_msg_hdr.dlc = can_msg.frame_fields.data_len;
		can_msg_hdr.mid = can_msg.msg_id;
		//Checks to see if msg ID matches Mater's HB ID. If so, send START_STOP.
		if(can_msg_hdr.mid == 0x20)
		{
			dbc_decode_MASTER_HB(&heartbeat, can_msg.data.bytes, &can_msg_hdr);
			dbc_encode_and_send_APP_START_STOP(&start_cmd);
		}
		if(can_msg_hdr.mid == 0x10){
			dbc_decode_SENSOR_DATA(&sensordata, can_msg.data.bytes,&can_msg_hdr);
			left=sensordata.SENSOR_left_sensor;
			center=sensordata.SENSOR_middle_sensor;
			right=sensordata.SENSOR_right_sensor;
			if(left>99){left=99;}
			if(center>99){center=99;}
			if(right>99){right=99;}
		}
		if(can_msg_hdr.mid==0x30){
			dbc_decode_MOTOR_STATUS(&motorstatus,can_msg.data.bytes,&can_msg_hdr);
			printf("\nMotor speed=%d\n",motorstatus.MOTOR_STATUS_speed_mph);
			speed=((float)motorstatus.MOTOR_STATUS_speed_mph)/100;
		}
		if(can_msg_hdr.mid==0x41){
			if(MIA_Unit==4){
				LD.clear();
			}
			dbc_decode_GPS_Data(&gpsdata,can_msg.data.bytes,&can_msg_hdr);
			latitude=gpsdata.GPS_READOUT_latitude;
			longitude=gpsdata.GPS_READOUT_longitude;
		}
	}
	if (dbc_handle_mia_MASTER_HB(&heartbeat, 100)) {
		LD.clear();
		LD.setRightDigit('1');
		MIA_Unit=1;
	}
	if(dbc_handle_mia_MOTOR_STATUS(&motorstatus,100)){
		LD.clear();
		LD.setRightDigit('2');
		MIA_Unit=1;
	}
	if(dbc_handle_mia_SENSOR_DATA(&sensordata,100)){
		LD.clear();
		LD.setRightDigit('3');
		MIA_Unit=1;
	}
	if(dbc_handle_mia_GPS_Data(&gpsdata,100)){
		LD.clear();
		LD.setRightDigit('4');
		MIA_Unit=1;
	}

}

void period_100Hz(uint32_t count) {
	LE.off(1);

	if (u2.getChar(msg, 0)) {
		LE.on(1);

		printf("\nReceived:%d",msg[0]);


		if(msg[0]==1) {
			start=1;
			stop=0;
			LE.on(4);
		}
		if(msg[0]==0) {
			stop=1;
			start=0;
			LE.off(4);
		}
	}

	//LE.toggle(3);
}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count) {
	//LE.toggle(4);
}
