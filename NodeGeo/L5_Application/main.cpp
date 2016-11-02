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
 * @brief This is the application entry point.
 * 			FreeRTOS and stdio printf is pre-configured to use uart0_min.h before main() enters.
 * 			@see L0_LowLevel/lpc_sys.h if you wish to override printf/scanf functions.
 *
 */
#include "tasks.hpp"
#include "examples/examples.hpp"
#include <stdio.h>
#include "acceleration_sensor.hpp"
#include "io.hpp"


/**
 * The main() creates tasks or "threads".  See the documentation of scheduler_task class at scheduler_task.hpp
 * for details.  There is a very simple example towards the beginning of this class's declaration.
 *
 * @warning SPI #1 bus usage notes (interfaced to SD & Flash):
 *      - You can read/write files from multiple tasks because it automatically goes through SPI semaphore.
 *      - If you are going to use the SPI Bus in a FreeRTOS task, you need to use the API at L4_IO/fat/spi_sem.h
 *
 * @warning SPI #0 usage notes (Nordic wireless)
 *      - This bus is more tricky to use because if FreeRTOS is not running, the RIT interrupt may use the bus.
 *      - If FreeRTOS is running, then wireless task may use it.
 *        In either case, you should avoid using this bus or interfacing to external components because
 *        there is no semaphore configured for this bus and it should be used exclusively by nordic wireless.
 */


/// IDs used for getSharedObject() and addSharedObject()
typedef enum {
   shared_SensorQueueId,
} sharedHandleId_t;

/// Orientation type enumeration
typedef enum {
    invalid = 0,
    left = 1,
    right = 2,
	up = 3,
    down = 4,
} orientation_t;

class orient_producer : public scheduler_task
{
    public:
        orient_producer(uint8_t priority) : scheduler_task("compute", 2048, priority)
        {
            /* We save the queue handle by using addSharedObject() */

            QueueHandle_t my_queue = xQueueCreate(1, sizeof(orientation_t));
            addSharedObject(shared_SensorQueueId, my_queue);
        }

        bool run(void *p)
        {
        	 orientation_t orientation = invalid;
            /* Compute orientation here, and send it to the queue once a second */
        	int16_t x,y;
        	Acceleration_Sensor &sensor = Acceleration_Sensor::getInstance();

        	sensor.init();

        	x = sensor.getX();
        	y = sensor.getY();

        	if(y<-200)
        		orientation = down;
        	else if (y>200)
        		orientation = up;
        	else if(x<-200)
        		orientation = right;
        	else if (x>200)
        		orientation = left;
        	else
        		orientation = invalid;

        	printf("Preparing to send Orientation to queue...\n");

            xQueueSend(getSharedObject(shared_SensorQueueId), &orientation, portMAX_DELAY);
            //portMAX_DELAY

            printf("Orientation sent to queue!\n");

            vTaskDelay(1000);
            return true;
        }
};

class orient_consumer : public scheduler_task
{
    public:
        orient_consumer (uint8_t priority) : scheduler_task("process", 2048, priority)
        {
        	/* Creates a que*/
        	QueueHandle_t my_queue = xQueueCreate(1, sizeof(orientation_t));
        	addSharedObject(shared_SensorQueueId, my_queue);
        }

        bool run(void *p)
        {
            /* We first get the queue handle the other task added using addSharedObject() */
            orientation_t orientation = invalid;
            QueueHandle_t qid = getSharedObject(shared_SensorQueueId);

            /* Sleep the task forever until an item is available in the queue */
            if (xQueueReceive(qid, &orientation, 0))
            {
            	LE.setAll(0);

            	if(orientation == left)
            	{
            		printf("Received! Orientation is: left\n\n");
                    LE.on(1);
            	}
            	else if(orientation == right)
            	{
            		printf("Received! Orientation is: right\n\n");
            		LE.on(2);
            	}
            	else if(orientation == up)
            	{
            		printf("Received! Orientation is: up\n\n");
            		LE.on(3);
            	}
            	else if(orientation == down)
            	{
            		printf("Received! Orientation is: down\n\n");
            		LE.on(4);
            	}
            	else
            		printf("Received! Orientation is: invalid\n\n");

            }

            return true;
        }
};

int main(void)
{

#if 0
    scheduler_add_task(new orient_producer(PRIORITY_LOW));
#endif

#if 0
    scheduler_add_task(new orient_consumer(PRIORITY_LOW));
#endif

    /**
     * A few basic tasks for this bare-bone system :
     *      1.  Terminal task provides gateway to interact with the board through UART terminal.
     *      2.  Remote task allows you to use remote control to interact with the board.
     *      3.  Wireless task responsible to receive, retry, and handle mesh network.
     *
     * Disable remote task if you are not using it.  Also, it needs SYS_CFG_ENABLE_TLM
     * such that it can save remote control codes to non-volatile memory.  IR remote
     * control codes can be learned by typing the "learn" terminal command.
     */
    scheduler_add_task(new terminalTask(PRIORITY_HIGH));

    /* Consumes very little CPU, but need highest priority to handle mesh network ACKs */
    //scheduler_add_task(new wirelessTask(PRIORITY_CRITICAL));

    /* Change "#if 0" to "#if 1" to run period tasks; @see period_callbacks.cpp */
    #if 1
    const bool run_1Khz = false;
    scheduler_add_task(new periodicSchedulerTask(run_1Khz));
    #endif

    /* The task for the IR receiver to "learn" IR codes */
    // scheduler_add_task(new remoteTask  (PRIORITY_LOW));

    /* Your tasks should probably used PRIORITY_MEDIUM or PRIORITY_LOW because you want the terminal
     * task to always be responsive so you can poke around in case something goes wrong.
     */

    /**
     * This is a the board demonstration task that can be used to test the board.
     * This also shows you how to send a wireless packets to other boards.
     */
    #if 0
        scheduler_add_task(new example_io_demo());
    #endif

    /**
     * Change "#if 0" to "#if 1" to enable examples.
     * Try these examples one at a time.
     */
    #if 0
        scheduler_add_task(new example_task());
        scheduler_add_task(new example_alarm());
        scheduler_add_task(new example_logger_qset());
        scheduler_add_task(new example_nv_vars());
    #endif

    /**
	 * Try the rx / tx tasks together to see how they queue data to each other.
	 */
    #if 0
        scheduler_add_task(new queue_tx());
        scheduler_add_task(new queue_rx());
    #endif

    /**
     * Another example of shared handles and producer/consumer using a queue.
     * In this example, producer will produce as fast as the consumer can consume.
     */
    #if 0
        scheduler_add_task(new producer());
        scheduler_add_task(new consumer());
    #endif

    /**
     * If you have RN-XV on your board, you can connect to Wifi using this task.
     * This does two things for us:
     *   1.  The task allows us to perform HTTP web requests (@see wifiTask)
     *   2.  Terminal task can accept commands from TCP/IP through Wifly module.
     *
     * To add terminal command channel, add this at terminal.cpp :: taskEntry() function:
     * @code
     *     // Assuming Wifly is on Uart3
     *     addCommandChannel(Uart3::getInstance(), false);
     * @endcode
     */
    #if 0
        Uart3 &u3 = Uart3::getInstance();
        u3.init(WIFI_BAUD_RATE, WIFI_RXQ_SIZE, WIFI_TXQ_SIZE);
        scheduler_add_task(new wifiTask(Uart3::getInstance(), PRIORITY_LOW));
    #endif

    scheduler_start(); ///< This shouldn't return
    return -1;
}
