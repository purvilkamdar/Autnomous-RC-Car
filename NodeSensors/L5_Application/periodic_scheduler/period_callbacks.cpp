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

#define BUCKETS 12

/* GLOBALS */
int start = 0;
int stop = 0;
int left_distance = 0;
int middle_distance = 0;
int right_distance = 0;
int distance = 0;
bool left = 0,middle = 0,right = 0;

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

ModeFilter left_filter;
ModeFilter middle_filter;
ModeFilter right_filter;

int HashIt(int Val)
{
	if(Val/12 > 12)   // 12 inches = 1 foot
		return 12;
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
	distance = left_distance;
	  int index = HashIt(left_distance);
	      left_filter.sum[index] += left_distance;
	      left_filter.count[index] ++;
	      if(left_filter.count[index] > left_filter.MAX)
	      	{left_filter.MAX = left_filter.count[index];
	      	left_filter.INDEX = index;}
	middle = 1;
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
	distance = middle_distance;
	int index = HashIt(middle_distance);
			      middle_filter.sum[index] += middle_distance;
			      middle_filter.count[index] ++;
			      if(middle_filter.count[index] > middle_filter.MAX)
			      	{middle_filter.MAX = middle_filter.count[index];
			      	 middle_filter.INDEX = index;}
	right = 1;

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
	distance = right_distance;
	int index = HashIt(right_distance);
    right_filter.sum[index] += right_distance;
    right_filter.count[index] ++;
    if(right_filter.count[index] > right_filter.MAX)
    	{right_filter.MAX = right_filter.count[index];
    	 right_filter.INDEX = index;}
	left = 1;
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
	      left = 1;

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
	LD.setNumber(middle_filter.filtered_val);
}

void period_10Hz(uint32_t count)
{
	if(xSemaphoreTake(Send_CAN_Msg, 0))
			{
			LE.toggle(1);
			ApplyFilter();
			Reset_filters();
			}
}

void period_100Hz(uint32_t count)
{

}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{

		static int count_100 = 0;
		count_100 ++;

		if(left)
	    {
		left = 0;
		LE.toggle(1);
		sensor_trigger_right.setLow();
		sensor_trigger_left.setHigh();
		delay_us(25);

	    }

		if(middle)
	    {
		middle = 0;
		LE.toggle(2);
		sensor_trigger_left.setLow();
		sensor_trigger_middle.setHigh();
		delay_us(25);

	    }


		if(right)
	    {
		right = 0;
	    LE.toggle(3);
	    sensor_trigger_middle.setLow();
	    sensor_trigger_right.setHigh();
		delay_us(25);
	    }

		if(count_100 == 100)
			{
			xSemaphoreGiveFromISR(Send_CAN_Msg, NULL);
			count_100 = 0;
			}
}
