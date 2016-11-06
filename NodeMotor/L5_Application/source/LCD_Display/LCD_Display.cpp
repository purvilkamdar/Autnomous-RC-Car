/*
 * LCD_Display.cpp
 *
 *  Created on: Nov 6, 2016
 *      Author: Kay
 */
#include <stdio.h>
#include "LCD_Display.hpp"
#include "uart3.hpp"

LCD_touch_Task::LCD_touch_Task() :
    scheduler_task("LCD_touch_Task", 3 * 512, PRIORITY_HIGH)
{
    /**
     * This is optional such that run() will be called every 3000ms.
     * If you don't do this, run() will be called without a delay.
     */
    setRunDuration(500);
}

bool LCD_touch_Task::run(void *p)
{

    return true;
}



// RX Task
LCD_Rx_Task::LCD_Rx_Task() :
    scheduler_task("LCD_Rx_Task", 3 * 512, PRIORITY_HIGH)
{
    /**
     * This is optional such that run() will be called every 3000ms.
     * If you don't do this, run() will be called without a delay.
     */
    setRunDuration(500);
}

bool LCD_Rx_Task::run(void *p)
{

    return true;
}
