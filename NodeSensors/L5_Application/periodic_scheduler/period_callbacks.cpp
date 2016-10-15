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
#include "io.hpp"
#include "periodic_callback.h"
#include "utilities.h"
#include "stdio.h"
#include "eint.h"
#include "gpio.hpp"
#include "string.h"
#include "lpc_sys.h"
#include "can.h"
#include "file_logger.h"
#include "generated_can.h"



bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
    can_msg_t can_msg = { 0 };
    can_msg.msg_id                = mid;
    can_msg.frame_fields.data_len = dlc;
    memcpy(can_msg.data.bytes, bytes, dlc);

    return CAN_tx(can1, &can_msg, 0);
}

SENSOR_HB_t sonic_sensors_HB = {0};

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

GPIO sensor_trigger_left(P2_3);
GPIO sensor_trigger_middle(P2_4);
GPIO sensor_trigger_right(P2_5);

static SemaphoreHandle_t Send_CAN_Msg = 0; //semaphore variable

typedef struct ModeFilter
{
int sum[BUCKETS];
int count[BUCKETS];
int MAX;
int INDEX;
int filtered_val;
}ModeFilter;

enum SENSOR{LEFT,MIDDLE,RIGHT,WAIT};
SENSOR sensor;

ModeFilter left_filter;
ModeFilter middle_filter;
ModeFilter right_filter;

int HashIt(int Val)
{
	if(Val/12 > BUCKETS)   // 12 inches = 1 foot
		return BUCKETS;
	else
	return (Val / 12);
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
	   /* CAN INIT */

	      //can1 init at a baud rate of 250
	      CAN_init(can1,100,5,5,NULL,NULL);

		  //CAN message Filter
	      const can_std_id_t slist[]  = { CAN_gen_sid(can1, 0x030),   // Acknowledgment from the nodes that received sensor reading
										  CAN_gen_sid(can1, 0x031) }; // Only 1 ID is expected, hence small range

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
//LD.setNumber(left_filter.filtered_val);
if(CAN_is_bus_off(can1))
	 //Start the CAN bus
	 CAN_reset_bus(can1);
}

can_msg_t can_msg_tx;

void period_10Hz(uint32_t count)
{
	if(xSemaphoreTake(Send_CAN_Msg, 0))
			{
			LE.toggle(4);
			ApplyFilter();

			sonic_sensors_HB.SENSOR_HEARTBEAT_cmd = left_filter.filtered_val;
			//Log filtered left,middle & right sensor values
			//LOG_INFO("F %d %d %d\n",left_filter.filtered_val,middle_filter.filtered_val,right_filter.filtered_val);
			dbc_encode_and_send_SENSOR_HB(&sonic_sensors_HB);

			Reset_filters();
			}
}

MOTOR_HB_t motor_hb_msg = { 0 };
void period_100Hz(uint32_t count)
{
    can_msg_t can_msg;
   // LD.setNumber(0);

        // Empty all of the queued, and received messages within the last 10ms (100Hz callback frequency)
       if (CAN_rx(can1, &can_msg, 0))
        {
    	   LE.toggle(2);
            // Form the message header from the metadata of the arriving message
            dbc_msg_hdr_t can_msg_hdr;
            can_msg_hdr.dlc = can_msg.frame_fields.data_len;
            can_msg_hdr.mid = can_msg.msg_id;

            // Attempt to decode the message (brute force, but should use switch/case with MID)
            dbc_decode_MOTOR_HB(&motor_hb_msg, can_msg.data.bytes, &can_msg_hdr);

            LD.setNumber(motor_hb_msg.MOTOR_HEARTBEAT_cmd);

           }


}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{

		static int count_100 = 0;
		count_100 ++;

		switch(sensor)
		{
		case LEFT: //trigger left sensor
	    {
		sensor = WAIT;
		LE.toggle(1);
		sensor_trigger_right.setLow();
		sensor_trigger_left.setHigh();
		delay_us(25);
		break;
	    }

		case MIDDLE: //trigger middle sensor
	    {
		sensor = WAIT;
		//LE.toggle(2);
		sensor_trigger_left.setLow();
		sensor_trigger_middle.setHigh();
		delay_us(25);
		break;
	    }

		case RIGHT: //trigger right sensor
	    {
		sensor = WAIT;
	    LE.toggle(3);
	    sensor_trigger_middle.setLow();
	    sensor_trigger_right.setHigh();
		delay_us(25);
	    break;
	    }

		case WAIT: //Wait for the falling edge of PWM signal from the triggered sensor before triggering next one
			break;
		}

/*Apply filter every 100 sensor reading - filter is applied on 100ms task. Give semaphore to 100ms task once
  100 readings are taken from the sensors*/
		if(count_100 == 100)
			{
			xSemaphoreGiveFromISR(Send_CAN_Msg, NULL);
			count_100 = 0;
			}

		//Log unfiltered sensor values
		//LOG_INFO("U %d %d %d\n",left_distance,middle_distance,right_distance);

}
