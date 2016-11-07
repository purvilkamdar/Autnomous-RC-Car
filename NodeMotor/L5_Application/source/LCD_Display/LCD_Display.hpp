/*
 * LCD_Display.hpp
 *
 *  Created on: Nov 6, 2016
 *      Author: Kay
 */

#ifndef L5_APPLICATION_SOURCE_LCD_DISPLAY_LCD_DISPLAY_HPP_
#define L5_APPLICATION_SOURCE_LCD_DISPLAY_LCD_DISPLAY_HPP_

#include "scheduler_task.hpp"
#include "shared_handles.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "uart3.hpp"

class LCD_Rx_Task : public scheduler_task
{
    public:
		LCD_Rx_Task();
        bool run(void *p);
};

class LCD_Tx_Task : public scheduler_task
{
    public:
		LCD_Tx_Task();
	    bool run(void *p);
        void write_LCD(char command,char obj_type,char obj_idx,char byte2,char byte1 = 0x00);
    private:
        Uart3 &LCD_Tx = Uart3::getInstance();
        char data[2];
        char ack;
};






#endif /* L5_APPLICATION_SOURCE_LCD_DISPLAY_LCD_DISPLAY_HPP_ */
