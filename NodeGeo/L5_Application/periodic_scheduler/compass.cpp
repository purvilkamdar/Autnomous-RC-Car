#include "stdlib.h"
#include <string.h>
#include "stdio.h"
#include "uart3.hpp"
#include "tasks.hpp"
#include "examples/examples.hpp"
#include "io.hpp"
#include "file_logger.h"
#include "compass.hpp"
#include "math.h"
#include "can.h"
#include "command_handler.hpp"
#include "_can_dbc/generated_can.h"

#define compass_baudrate 57600

static Uart3& u3 = Uart3::getInstance();

char *temp;

uint16_t heading;

void canbus_check()
{
	if(CAN_is_bus_off(can1))
	{
		CAN_reset_bus(can1);
	}

}

void get_compass_data(COM_DATA *compassData)
{
	const char s[2] = ",";                        // ","  is delimter used to parse data
	char rx_buff[10];
	char rx_str[10];
    u3.gets(rx_buff, sizeof(rx_buff), 0);         // get data from compass module

    strcpy(rx_str,rx_buff);                       // copy the compass data to a temporary string
	temp = strtok(rx_str,s);                      // Separate the data by ','
	if(temp!=NULL)								  // check if the data valid
	{
	//printf("%s\n",temp);

	heading = atoi(temp);

	heading = heading + 13;

	if(heading >=360)
		heading = heading - 360;
	//printf("%0.2f\n",x);
	//heading = x;
	//sscanf(temp,"%f",&heading);
	}
	printf("Heading from Compass = %i\n",heading);

	compassData->Com_head = heading;
}


//bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
//{
//    can_msg_t can_msg1 = { 0 };
//    can_msg1.msg_id                = mid;
//    can_msg1.frame_fields.data_len = dlc;
//    memcpy(can_msg1.data.bytes, bytes, dlc);
//
//    return CAN_tx(can1, &can_msg1, 0);
//}
