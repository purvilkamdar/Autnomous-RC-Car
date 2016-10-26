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

can_msg_t message;

static Uart3& u3 = Uart3::getInstance();
static const int rx_q = 100;
static const int tx_q = 100;

char *temp;

uint16_t heading;

COMPASS_Data_t COMPASS_Value = {0};

void serial_init(void)
{
	u3.init(compass_baudrate,rx_q,tx_q);

	CAN_init(can1,100,10,10,NULL,NULL);
	CAN_reset_bus(can1);
	CAN_bypass_filter_accept_all_msgs();
/*
	const can_std_id_t slist[]      = { CAN_gen_sid(can1, 0x100), CAN_gen_sid(can1, 0x110),   // 2 entries
	                                     CAN_gen_sid(can1, 0x120), CAN_gen_sid(can1, 0x130)    // 2 entries
	};
	CAN_setup_filter(slist, 4 , NULL, 0, NULL, 0, NULL, 0);
*/
}

void canbus_check()
{
	if(CAN_is_bus_off(can1))
	{
		CAN_reset_bus(can1);
	}

}

void get_compass_data(void)
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

	float x = atof(temp);
	printf("%0.2f\n",x);
	heading = (x *100);
	//sscanf(temp,"%f",&heading);
	}
	printf("%i\n",heading);

	//COMPASS_Value.COMPASS_Heading = heading;
}

void can_tx(void)
{
	//dbc_encode_and_send_COMPASS_Data(&COMPASS_Value);
}
