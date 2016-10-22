#ifndef COMPASS_HPP_
#define COMPASS_HPP_

#include "singleton_template.hpp"


typedef struct compass_data
{
        float yaw;
}
compass_data_t;

void canbus_check(void);
void uart3_init(void);
void can_init(void);
void get_compass_data(void);
void can_tx(void);



#endif //COMPASS_HPP
