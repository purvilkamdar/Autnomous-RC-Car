#ifndef COMPASS_HPP_
#define COMPASS_HPP_



void canbus_check(void);
void serial_init(void);
void get_compass_data(void);
void can_task(void);


bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8]);



#endif //COMPASS_HPP
