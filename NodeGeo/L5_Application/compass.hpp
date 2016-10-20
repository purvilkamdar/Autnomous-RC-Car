#ifndef COMPASS_HPP_
#define COMPASS_HPP_


#include "singleton_template.hpp"


typedef struct compass_data
{
        float yaw;
}
compass_data_t;



void uart3_init(void);
void get_compass_data(void);

#endif //COMPASS_HPP
