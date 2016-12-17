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
#include <iostream>
#include<vector>
#include <file_logger.h>
#include <semphr.h>
using namespace std;
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
char *size	= new char[2];
bool size_flag=0;
bool start = 0;
bool stop = 0;
bool last_flag = 0;
bool last_co=0;
bool match_flag=0;
int MIA_Unit=0;
int my_left=99,my_right=99,center=99,rear=99;
float speed=0,temporary_latitude_value=0,temporary_longitude_value=0;
int latitude=0, longitude=0;
double latitude1=0,longitude1=0;
volatile int n=0;
int lat1,lat2,lat3,lat4,lat5,lat6,lon1,lon2,lon3,lon4,lon5,lon6;
char a[1];
double s_latitude[100];
double s_longitude[100];
//int s_latitude[100];
//int s_longitude[100];
//float* s_latitude = new float[100];
//float* s_longitude = new float[100];
int count_semaphore=0;
int counter=0;
volatile int print_counter=0;
volatile int lat_counter=0;
extern const uint32_t MASTER_HB__MIA_MS = 3000;
extern const MASTER_HB_t MASTER_HB__MIA_MSG = { 0 };
extern const uint32_t SENSOR_DATA__MIA_MS = 3000;
extern const SENSOR_DATA_t SENSOR_DATA__MIA_MSG = { 100,100,100 };
extern const uint32_t GPS_Data__MIA_MS = 3000;
extern const GPS_Data_t GPS_Data__MIA_MSG = { 0.000000, 0.000000 };
extern const uint32_t MOTOR_STATUS__MIA_MS=3000;
extern const MOTOR_STATUS_t MOTOR_STATUS__MIA_MSG={0};
int i=0;
bool destiation_reached=false;
static SemaphoreHandle_t start_sending=0;

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
	LE.off(3);

}
void overrun(uint32_t arg) {
	LE.on(4);
}

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void) {
	for(i=0;i<100;i++)
	{
		s_latitude[i]=0;
		s_longitude[i]=0;
	}
	can_void_func_t busOff = busoff;
	can_void_func_t overr = overrun;
	u2.init(115200);
	CAN_init(can1, 100, 10, 10, busOff, overr);
	CAN_bypass_filter_accept_all_msgs();
	CAN_reset_bus(can1);
	vSemaphoreCreateBinary(start_sending);
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
	if(CAN_is_bus_off(can1))
		{
			CAN_reset_bus(can1);
			LE.on(3);
		}
	if(n==0 && !last_flag){
		while (CAN_rx(can1, &can_msg, 0)) {
				dbc_msg_hdr_t can_msg_hdr;
				can_msg_hdr.dlc = can_msg.frame_fields.data_len;
				can_msg_hdr.mid = can_msg.msg_id;
				APP_START_STOP_t start_cmd = { 0 };
				if(can_msg_hdr.mid == 0x20)
					{
					if (stop == 1) {

							start_cmd.APP_START_STOP_cmd = 0;
							start_cmd.APP_COORDINATE_READY=0;
							start_cmd.APP_FINAL_COORDINATE=0;
							counter=0;


						dbc_decode_MASTER_HB(&heartbeat, can_msg.data.bytes, &can_msg_hdr);
							//n is the number of the co-ordinates from the APP & last_flag is true if last is received from the app
						dbc_encode_and_send_APP_START_STOP(&start_cmd);
						}
					}

				if(can_msg_hdr.mid==0x41){
							if(MIA_Unit==4){
								LD.clear();
							}
							dbc_decode_GPS_Data(&gpsdata,can_msg.data.bytes,&can_msg_hdr);
							latitude1=gpsdata.GPS_READOUT_latitude;
							latitude=latitude1*1000000;
							longitude1=gpsdata.GPS_READOUT_longitude;
							longitude=longitude1*1000000;
						}

				if(can_msg_hdr.mid == 0x10){
							dbc_decode_SENSOR_DATA(&sensordata, can_msg.data.bytes,&can_msg_hdr);
							my_left=sensordata.SENSOR_left_sensor;
							center=sensordata.SENSOR_middle_sensor;
							my_right=sensordata.SENSOR_right_sensor;
							if(my_left>99){my_left=99;}
							if(center>99){center=99;}
							if(my_right>99){my_right=99;}
						}

				}
			char* data=new char[20];
			sprintf(data,"L%02dR%02dC%02dB%02dSPD%0.3f",my_left,my_right,center,rear,speed);
			u2.put(data, 0);
			sprintf(data,"@%d$%d",latitude,longitude);
			u2.put(data,0);
			delete[] data;
}
	//LE.toggle(1);
}

void period_10Hz(uint32_t count) {


	if(xSemaphoreTake(start_sending,0))
	{
		//This is the code for printing the app co-ordinates. Uncomment it in case of verification.
	/*if(n!=0 && last_flag)
	{
		if(print_counter<lat_counter)
		{
			printf("The value of n is:%d \n",n);
			printf("The value of counter is %d \n",lat_counter);
			printf("%d) Co-ordinates=%lf,%lf \n",print_counter,s_latitude[print_counter],s_longitude[print_counter]);
		}
		print_counter++;
	}*/

		/*if(n!=0 && last_flag)
			{
				//printf("The value of counter variable=%d",counter);
				if(counter<lat_counter)
				{
					//printf("The value of n is:%d \n",n);
					printf("The value of counter is %d \n",lat_counter);
					printf("%d) Co-ordinates=%lf,%lf \n",counter,s_latitude[counter],s_longitude[counter]);
				}
				counter++;
			}*/

	char* data=new char[20];
	if(!destiation_reached)
		sprintf(data,"L%02dR%02dC%02dB%02dSPD%0.2f0",my_left,my_right,center,rear,speed);
	else
		sprintf(data,"L%02dR%02dC%02dB%02dSPD%0.2f1",my_left,my_right,center,rear,speed);
	u2.put(data, 0);
	sprintf(data,"@%d$%d",latitude,longitude);
	u2.put(data,0);
	delete[] data;
	APP_START_STOP_t start_cmd = { 0 };
	if (start == 1) {
		start_cmd.APP_START_STOP_cmd = 1;

	} else if (stop == 1) {
		start_cmd.APP_START_STOP_cmd = 0;
		start_cmd.APP_COORDINATE_READY=0;
		counter=0;

	}else if(SW.getSwitch(1)){
		stop=1;
	}
	//printf("The value of counter is %d \n",lat_counter);

	while (CAN_rx(can1, &can_msg, 0)) {
		dbc_msg_hdr_t can_msg_hdr;
		can_msg_hdr.dlc = can_msg.frame_fields.data_len;
		can_msg_hdr.mid = can_msg.msg_id;
		//Checks to see if msg ID matches Mater's HB ID. If so, send START_STOP.

		if(can_msg_hdr.mid == 0x20)
		{
			dbc_decode_MASTER_HB(&heartbeat, can_msg.data.bytes, &can_msg_hdr);
			//n is the number of the co-ordinates from the APP & last_flag is true if last is received from the app
			//printf("n=%d,last_flag=%d",n,last_flag);
			if(n!=0 && last_flag)
			{
				//printf("entered");
				start_cmd.APP_COORDINATE_READY=1;
			}
			else
			{
				start_cmd.APP_COORDINATE_READY=0;
			}
			if(heartbeat.MASTER_START_COORD==1)
			{
				//counter is the iterator for the array and (lat_counter-1) is the number of co-ordinates after
				//removing the repeated co-ordinates. lat_counter value & n will not be the same always.
				if(counter<lat_counter)
				{
					start_cmd.APP_ROUTE_latitude=s_latitude[counter];
					start_cmd.APP_ROUTE_longitude=s_longitude[counter];
					printf("%d) Co-ordinates=%lf,%lf \n",counter,s_latitude[counter],s_longitude[counter]);
					if(counter==(lat_counter-1))
					{	start_cmd.APP_FINAL_COORDINATE=1;
						//start_cmd.APP_COORDINATE_READY=0;
						//last_flag=false;
						n=0;
						//counter=-1;
					}
					else
					{
						start_cmd.APP_FINAL_COORDINATE=0;
					}
					counter++;
				}
			}
				if(counter>=lat_counter)
				{
					//printf("I entered");
					//start_cmd.APP_FINAL_COORDINATE=0;
					start_cmd.APP_COORDINATE_READY=0;
				}
				/*else
				{
					//printf("I entered");
					start_cmd.APP_FINAL_COORDINATE=0;
					start_cmd.APP_COORDINATE_READY=0;
				}
				printf("counter=%d",counter);*/

			dbc_encode_and_send_APP_START_STOP(&start_cmd);
		}
		if(can_msg_hdr.mid == 0x10){
			dbc_decode_SENSOR_DATA(&sensordata, can_msg.data.bytes,&can_msg_hdr);
			my_left=sensordata.SENSOR_left_sensor;
			center=sensordata.SENSOR_middle_sensor;
			my_right=sensordata.SENSOR_right_sensor;
			if(my_left>99){my_left=99;}
			if(center>99){center=99;}
			if(my_right>99){my_right=99;}
		}
		if(can_msg_hdr.mid==0x30){
			dbc_decode_MOTOR_STATUS(&motorstatus,can_msg.data.bytes,&can_msg_hdr);
			printf("\nMotor speed=%lf\n",motorstatus.MOTOR_STATUS_speed_mph);
			speed=((float)motorstatus.MOTOR_STATUS_speed_mph)/100;
		}
		if(can_msg_hdr.mid==0x41){
			if(MIA_Unit==4){
				LD.clear();
			}
			dbc_decode_GPS_Data(&gpsdata,can_msg.data.bytes,&can_msg_hdr);
			latitude1=gpsdata.GPS_READOUT_latitude;
			latitude=latitude1*1000000;
			longitude1=gpsdata.GPS_READOUT_longitude;
			longitude=longitude1*1000000;
		}
		if(can_msg_hdr.mid==0x21){
								destiation_reached=true;
								stop=1;
								lat_counter=0;
								counter=0;
								start=0;
								size_flag=0;
								last_flag=0;
								n=0;

								LE.off(4);
								LE.off(2);
								LE.off(3);
								LE.off(1);

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
}

void period_100Hz(uint32_t count) {

	char *stat=new char[14];
			u2.gets(stat,14,0);

				if(strcmp(stat,"start")==0) {
					destiation_reached=false;
					start=1;
					lat_counter=0;
					print_counter=0;
					stop=0;
					size_flag=0;
					n=0;
					LE.on(4);
					}

				else if(strcmp(stat,"stop")==0) {
					stop=1;
					lat_counter=0;
					counter=0;
					start=0;
					size_flag=0;
					last_flag=0;
					n=0;

					LE.off(4);
					LE.off(2);
					LE.off(3);
					LE.off(1);
					}

				if(size_flag)
				{
					LE.on(3);
					size_flag=0;
					sscanf(stat,"%d",&n);
				}

				if(strcmp(stat,"Size")==0)
					{
						size_flag=1;
					}
				 if(strcmp(stat,"Last")==0)
				{
					LE.on(2);
					last_flag=1;
				}

				if(stat[0]=='#' || stat[1]=='#')
				{
					match_flag=0;
					lat1=stat[1]-'0';
					lat2=stat[2]-'0';
					lat3=stat[3]-'0';
					lat4=stat[4]-'0';
					lat5=stat[5]-'0';
					lat6=stat[6]-'0';

					lon1=stat[8]-'0';
					lon2=stat[9]-'0';
					lon3=stat[10]-'0';
					lon4=stat[11]-'0';
					lon5=stat[12]-'0';
					lon6=stat[13]-'0';


					temporary_latitude_value=(37+(lat1*0.1)+(lat2*0.01)+(lat3*0.001)+(lat4*0.0001)+(lat5*0.00001)+(lat6*0.000001));
					temporary_longitude_value=(121+(lon1*0.1)+(lon2*0.01)+(lon3*0.001)+(lon4*0.0001)+(lon5*0.00001)+(lon6*0.000001));
					int repeat_counter;
					for(repeat_counter=0;repeat_counter<lat_counter;repeat_counter++)
					{
						if((s_latitude[repeat_counter]==temporary_latitude_value) && (s_longitude[repeat_counter]==temporary_longitude_value))
						{
							match_flag=1;
						}

					}
					if(match_flag==0)
					{
						LE.on(1);
						s_latitude[lat_counter]=temporary_latitude_value;
						s_longitude[lat_counter]=temporary_longitude_value;
						lat_counter++;
					}
				}

					delete[] stat;
					if(last_flag)
					{
						xSemaphoreGive(start_sending);
					}

}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count) {

}

