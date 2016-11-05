/*
 * sensors.h
 *
 *  Created on: Oct 14, 2016
 *      Author: Sathishkumar
 */

#ifndef L5_APPLICATION_SOURCE_SENSORS_SENSORS_H_
#define L5_APPLICATION_SOURCE_SENSORS_SENSORS_H_

#include <stdint.h>
#include "sensors_struct.h"
#include "math.h"
#include "eint.h"
#include "can.h"
#include "gpio.hpp"

/* 21 buckets - 6.5 meters(256 inches) is the max range of sonic sensors.
 256 inches is divided into 12 inch ranges - total 21 buckets (12 * 21 ~ 252 inches) */
#define BUCKETS 21

/* GLOBALS */
int start = 0;
int stop = 0;
int left_distance = 0;
int middle_distance = 0;
int right_distance = 0;
int distance = 0;

static SemaphoreHandle_t Send_CAN_Msg = 0; //semaphore variable

ModeFilter left_filter;
ModeFilter middle_filter;
ModeFilter right_filter;

// For SD
uint8_t SD_index = 0;
uint8_t left_invalid,middle_invalid,right_invalid;
SD left,middle,right;

SENSOR_DATA_t sonic_sensor_data;
MASTER_HB_t master_hb_msg = { 0 };
SENSOR_DBG_t sensor_dbg;

const uint32_t         MASTER_HB__MIA_MS = 1000;
const MASTER_HB_t      MASTER_HB__MIA_MSG = {0};

enum SENSOR{LEFT,MIDDLE,RIGHT,WAIT};
SENSOR sensor;


GPIO sensor_trigger_left(P2_3);
GPIO sensor_trigger_middle(P2_4);
GPIO sensor_trigger_right(P2_5);


int HashIt(int Val)
{
	if(Val/12 > BUCKETS)   // 12 inches = 1 foot
		return BUCKETS;
	else
	return (Val / 12);
}


/*INTERRUPT CALLBACKS*/

void sensor_rise_left(void)
	{
	 start = sys_get_uptime_us();
	}

void sensor_fall_left(void)
	{
	stop = sys_get_uptime_us();
	//distance = (stop - start)/58;
	left_distance = (stop - start)/147;
	  int index = HashIt(left_distance);
	  left_filter.sum[index] += left_distance;
	  left_filter.count[index] ++;
	  if(left_filter.count[index] > left_filter.MAX)
		{left_filter.MAX = left_filter.count[index];
		left_filter.INDEX = index;}
	sensor = MIDDLE;
	}

void sensor_rise_middle(void)
	{
	 start = sys_get_uptime_us();
	}

void sensor_fall_middle(void)
	{
	stop = sys_get_uptime_us();
	//distance = (stop - start)/58;
	middle_distance = (stop - start)/147;
	int index = HashIt(middle_distance);
    middle_filter.sum[index] += middle_distance;
	middle_filter.count[index] ++;
    if(middle_filter.count[index] > middle_filter.MAX)
	  {middle_filter.MAX = middle_filter.count[index];
	   middle_filter.INDEX = index;}
	sensor = RIGHT;

	}

void sensor_rise_right(void)
	{
	 start = sys_get_uptime_us();
	}

void sensor_fall_right(void)
	{
	stop = sys_get_uptime_us();
	//distance = (stop - start)/58;      // In cms
	right_distance = (stop - start)/147; //In inches
	int index = HashIt(right_distance);
	right_filter.sum[index] += right_distance;
	right_filter.count[index] ++;
	if(right_filter.count[index] > right_filter.MAX)
		{right_filter.MAX = right_filter.count[index];
		 right_filter.INDEX = index;}
	sensor = LEFT;
	}


void init_all()
{
	 /* CAN INIT */

		      //can1 init at a baud rate of 250
		      CAN_init(can1,100,5,5,NULL,NULL);

			  //CAN message Filter
		      const can_std_id_t slist[]  = { CAN_gen_sid(can1, 0x020),   // Acknowledgment from the nodes that received sensor reading
											  CAN_gen_sid(can1, 0x021) }; // Only 1 ID is expected, hence small range

		     CAN_setup_filter(slist, 2, NULL, 0, NULL, 0, NULL, 0);

			 //Start the CAN bus
			 CAN_reset_bus(can1);


		      vSemaphoreCreateBinary(Send_CAN_Msg);

		      sensor_trigger_left.setAsOutput();
		      sensor_trigger_left.setLow();
		      sensor_trigger_middle.setAsOutput();
		      sensor_trigger_middle.setLow();
		      sensor_trigger_right.setAsOutput();
		      sensor_trigger_right.setLow();


			  const uint8_t port2_0 = 0;
		      eint3_enable_port2(port2_0, eint_rising_edge, sensor_rise_left);
		      eint3_enable_port2(port2_0, eint_falling_edge, sensor_fall_left);

			  const uint8_t port2_1 = 1;
		      eint3_enable_port2(port2_1, eint_rising_edge, sensor_rise_middle);
		      eint3_enable_port2(port2_1, eint_falling_edge, sensor_fall_middle);

			  const uint8_t port2_2 = 2;
		      eint3_enable_port2(port2_2, eint_rising_edge, sensor_rise_right);
		      eint3_enable_port2(port2_2, eint_falling_edge, sensor_fall_right);

		      //Trigger the left sensor first
		      sensor = LEFT;
}

void Reset_filters()
{
for(int i=0;i<BUCKETS;i++)
{
left_filter.sum[i] = 0;
middle_filter.sum[i] = 0;
right_filter.sum[i] = 0;

left_filter.count[i] = 0;
middle_filter.count[i] = 0;
right_filter.count[i] = 0;

left_filter.MAX = 0;
middle_filter.MAX = 0;
right_filter.MAX = 0;
}
}

void ApplyFilter()
{

	left_filter.filtered_val   =  left_filter.sum[left_filter.INDEX]/left_filter.count[left_filter.INDEX];
	middle_filter.filtered_val =  middle_filter.sum[middle_filter.INDEX]/middle_filter.count[middle_filter.INDEX];
	right_filter.filtered_val  =  right_filter.sum[right_filter.INDEX]/right_filter.count[right_filter.INDEX];

}

void is_valid()
{

	left.val[SD_index] = left_filter.filtered_val;
	middle.val[SD_index] = middle_filter.filtered_val;
	right.val[SD_index] = right_filter.filtered_val;

	left.sum   += left.val[SD_index];
	middle.sum += middle.val[SD_index];
	right.sum  +=right.val[SD_index];
	SD_index++;

	if(SD_index == 4)
	{
	left.mean = left.sum/5;
	middle.mean = middle.sum/5;
	right.mean = right.sum/5;
	SD_index = 0;

	for(int i=0;i<5;i++)
	{
		left.variance += (left.val[SD_index] - left.mean)^2;
		middle.variance += (middle.val[SD_index] - middle.mean)^2;
		right.variance += (right.val[SD_index] - right.mean)^2;
	}
	left.variance /= 5;
	middle.variance /= 5;
	right.variance /= 5;

	if(sqrt(left.variance)/left.mean > 0.7)
		left_invalid = 1;
	else
		left_invalid = 0;

	if(sqrt(middle.variance)/middle.mean > 0.7)
		middle_invalid = 1;
	else
		middle_invalid = 0;

	if(sqrt(right.variance)/right.mean > 0.7)
		right_invalid = 1;
	else
		right_invalid = 0;
	}
}


bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
    can_msg_t can_msg = { 0 };
    can_msg.msg_id                = mid;
    can_msg.frame_fields.data_len = dlc;
    memcpy(can_msg.data.bytes, bytes, dlc);

    return CAN_tx(can1, &can_msg, 0);
}

#endif /* L5_APPLICATION_SOURCE_SENSORS_SENSORS_H_ */
