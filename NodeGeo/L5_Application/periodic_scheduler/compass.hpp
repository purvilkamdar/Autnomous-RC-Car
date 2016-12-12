#ifndef COMPASS_HPP_
#define COMPASS_HPP_

typedef struct{
	uint16_t Com_head;
}COM_DATA;

void canbus_check(void);
void get_compass_data(COM_DATA *compassData);
void can_task(void);


//bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8]);



#endif //COMPASS_HPP
