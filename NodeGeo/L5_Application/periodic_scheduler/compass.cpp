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

can_msg_t can_msg;
//MASTER_HB_t master_hb_msg = { 0 };

//const uint32_t         MASTER_HB__MIA_MS = 1000;
//const MASTER_HB_t      MASTER_HB__MIA_MSG = {0};


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
	//printf("%0.2f\n",x);
	//heading = x;
	//sscanf(temp,"%f",&heading);
	}
	printf("Heading from Compas.pp = %i\n",heading);

	compassData->Com_head = heading;
}

//void can_task(void)
//{
//	while (CAN_rx(can1, &can_msg, 0))
//	        {
//		        LE.toggle(3);
//	            dbc_msg_hdr_t can_msg_hdr;
//	            can_msg_hdr.dlc = can_msg.frame_fields.data_len;
//	            can_msg_hdr.mid = can_msg.msg_id;
//	            dbc_decode_MASTER_HB(&master_hb_msg, can_msg.data.bytes, &can_msg_hdr);
//	            if(can_msg_hdr.mid == 0x20)
//	            {
//	            dbc_encode_and_send_COMPASS_Data(&COMPASS_Value);
//	            }
//	         }
//
//	       if(dbc_handle_mia_MASTER_HB(&master_hb_msg,10))
//	       {
//	    	   LD.setNumber(11);
//	       }
//
//	       if(CAN_is_bus_off(can1))
//	       {	 //Start the CAN bus
//	    	 LD.setNumber(55);
//	       	 CAN_reset_bus(can1);
//	       }
//
//	}

//
//bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
//{
//    can_msg_t can_msg1 = { 0 };
//    can_msg1.msg_id                = mid;
//    can_msg1.frame_fields.data_len = dlc;
//    memcpy(can_msg1.data.bytes, bytes, dlc);
//
//    return CAN_tx(can1, &can_msg1, 0);
//}
