/*
 * LCD_Display.cpp
 *
 *  Created on: Nov 6, 2016
 *      Author: Kay
 */

#include "LCD_Display.hpp"
#include "LCD_Display_includes.hpp"
#include "io.hpp"
#include "utilities.h"
//#include "../../../_can_dbc/generated_can.h"

//SENSOR_DATA_t sensor_data;

LCD_Rx_Task::LCD_Rx_Task() :
    scheduler_task("LCD_Rx_Task", 3 * 512, PRIORITY_HIGH)
{

    setRunDuration(500);
}

bool LCD_Rx_Task::run(void *p)
{
	LE.toggle(1);
    return true;
}



// RX Task
LCD_Tx_Task::LCD_Tx_Task() :
    scheduler_task("LCD_Tx_Task", 3 * 512, PRIORITY_HIGH)
{

	LCD_Tx.init(9600,10,10);
	ack = 0x00;
	data[0] = 0x00;
    setRunDuration(500);

}


bool LCD_Tx_Task::run(void *p)
{
	LE.toggle(2);
	/* Display sensor readings */
	write_LCD(0x01,0x0B,0x00,sensor_data.SENSOR_left_sensor);
	delay_ms(5);

	write_LCD(0x01,0x0B,0x02,sensor_data.SENSOR_middle_sensor);
	delay_ms(5);

	write_LCD(0x01,0x0B,0x03,sensor_data.SENSOR_right_sensor);
	delay_ms(5);

	/* Display battery remaining */
	write_LCD(0x01,0x0B,0x01,90);
	delay_ms(15);

	/* Display motor speed - speedometer & digital display */
	write_LCD(0x01,0x10,0x00,(motor_msg.MOTOR_STATUS_speed_mph));
	delay_ms(15);

	write_LCD(0x01,0x0F,0x01,(motor_msg.MOTOR_STATUS_speed_mph));
	delay_ms(15);

	/* Display compass heading */
	char byte1 = char((compass_heading.COMPASS_Heading >> 8) );
	char byte2 = char(compass_heading.COMPASS_Heading & 0x00FF);
	write_LCD(0x01,0x07,0x00,byte2,byte1);

    return true;
}


void LCD_Tx_Task::write_LCD(char command,char obj_type,char obj_idx,char byte2,char byte1)
{
	int checksum = command ^ obj_type ^ obj_idx ^ byte1 ^ byte2;
	LCD_Tx.putChar(command,0);
	LCD_Tx.putChar(obj_type,0);
	LCD_Tx.putChar(obj_idx,0);
	LCD_Tx.putChar(byte1,0);
	LCD_Tx.putChar(byte2,0);
	LCD_Tx.putChar(checksum ,0); //checksum

	LCD_Tx.getChar(&ack,10); // Rx ACK/NACK from LCD
	LD.setNumber(ack);

}
