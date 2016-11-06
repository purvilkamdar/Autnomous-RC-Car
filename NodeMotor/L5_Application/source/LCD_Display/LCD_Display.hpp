/*
 * LCD_Display.hpp
 *
 *  Created on: Nov 6, 2016
 *      Author: Kay
 */

#ifndef L5_APPLICATION_SOURCE_LCD_DISPLAY_LCD_DISPLAY_HPP_
#define L5_APPLICATION_SOURCE_LCD_DISPLAY_LCD_DISPLAY_HPP_



class LCD_touch_Task : public scheduler_task
{
    public:
	    LCD_touch_Task();
        bool run(void *p);
};

class LCD_Rx_Task : public scheduler_task
{
    public:
	    LCD_Rx_Task();
        bool run(void *p);
};


#endif /* L5_APPLICATION_SOURCE_LCD_DISPLAY_LCD_DISPLAY_HPP_ */
