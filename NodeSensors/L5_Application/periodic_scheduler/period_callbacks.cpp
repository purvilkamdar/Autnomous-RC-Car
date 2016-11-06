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
#include "string.h"
#include "lpc_sys.h"
#include "file_logger.h"
#include "generated_can.h"
#include "../source/sensors/sensors.h"



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
	init_all();
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

}

can_msg_t can_msg_tx;

void period_10Hz(uint32_t count)
{
	if(xSemaphoreTake(Send_CAN_Msg, 0))
			{
			LE.toggle(4);
			ApplyFilter();

			sonic_sensor_data.SENSOR_left_sensor = left_filter.filtered_val;
			sonic_sensor_data.SENSOR_middle_sensor = middle_filter.filtered_val;
			sonic_sensor_data.SENSOR_right_sensor = right_filter.filtered_val;

			LD.setNumber(sonic_sensor_data.SENSOR_left_sensor);
			//sensor_dbg.SENSOR_left_filt = left_filter.filtered_val;
			//sensor_dbg.SENSOR_middle_filt = middle_filter.filtered_val;
			//sensor_dbg.SENSOR_right_filt = right_filter.filtered_val;

			//dbc_encode_and_send_SENSOR_DBG(&sensor_dbg);
			/*is_valid();
			sonic_sensor_data.SENSOR_left_invalid = left_invalid;
			sonic_sensor_data.SENSOR_middle_invalid = middle_invalid;
			sonic_sensor_data.SENSOR_right_invalid = right_invalid;*/
#if 0
//Log filtered left,middle & right sensor values
LOG_INFO("F %d %d %d\n",left_filter.filtered_val,middle_filter.filtered_val,right_filter.filtered_val);
#endif

			Reset_filters();
			}

    if(dbc_handle_mia_MASTER_HB(&master_hb_msg,100))
    	LE.toggle(1);
}


void period_100Hz(uint32_t count)
{
    can_msg_t can_msg;


        // Empty all of the queued, and received messages within the last 10ms (100Hz callback frequency)
       while (CAN_rx(can1, &can_msg, 0))
        {
    	   LE.toggle(2);
            // Form the message header from the metadata of the arriving message
            dbc_msg_hdr_t can_msg_hdr;
            can_msg_hdr.dlc = can_msg.frame_fields.data_len;
            can_msg_hdr.mid = can_msg.msg_id;

            // Attempt to decode the message (brute force, but should use switch/case with MID)
            dbc_decode_MASTER_HB(&master_hb_msg, can_msg.data.bytes, &can_msg_hdr);
            if(can_msg_hdr.mid == 0x20)
            {
            dbc_encode_and_send_SENSOR_DATA(&sonic_sensor_data);
            }

         }

       if(dbc_handle_mia_MASTER_HB(&master_hb_msg,10))
    	   LD.setNumber(0);

       if(CAN_is_bus_off(can1))
       	   CAN_reset_bus(can1);
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
}
