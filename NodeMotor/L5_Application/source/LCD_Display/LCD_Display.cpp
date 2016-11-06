/*
 * LCD_Display.cpp
 *
 *  Created on: Nov 6, 2016
 *      Author: Kay
 */

#include "LCD_Display.hpp"


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



// RX Task
LCD_Tx_Task::LCD_Tx_Task() :
    scheduler_task("LCD_Tx_Task", 3 * 512, PRIORITY_HIGH)
{
    /**
     * This is optional such that run() will be called every 3000ms.
     * If you don't do this, run() will be called without a delay.
     */
	ack = 0x00;
    setRunDuration(500);

}


bool LCD_Tx_Task::run(void *p)
{


    return true;
}


void LCD_Tx_Task::write_LCD(char command,char obj_type,char obj_idx,char arr[],char len)
{
	int checksum = command ^ obj_type ^ obj_idx;
	LCD_Rx.putChar(command,0);
	LCD_Rx.putChar(obj_type,0);
	LCD_Rx.putChar(obj_idx,0);

	for(int i=0;i<len;i++)
	{
		LCD_Rx.putChar(arr[i],0);
		checksum ^= arr[i];
	}

	LCD_Rx.putChar(checksum ,0); //checksum
	LCD_Rx.getChar(&ack,10); // Rx ACK/NACK from LCD

}
