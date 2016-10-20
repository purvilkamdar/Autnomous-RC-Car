#ifndef COMPASS_HPP_
#define COMPASS_HPP_


#include "singleton_template.hpp"


typedef struct compass_data{
        float yaw;
        float pitch;
        float roll;
}compass_data_t;


void uart_rx(void);
void uart3_init(void);


#endif //COMPASS_HPP
