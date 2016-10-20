
#include "uart3.hpp"
#include "tasks.hpp"
#include "examples/examples.hpp"
#include "stdio.h"
#include "io.hpp"
#include "file_logger.h"
#include "string.h"
#include "compass.hpp"
#include "switches.hpp"
#include "math.h"
#include "can.h"
#include "command_handler.hpp"

#define compass_baudrate 9600


compass_data_t compass_values;

static Uart3& u3 = Uart3::getInstance();
static const int rx_q = 100;
static const int tx_q = 100;

char *temp;

QueueHandle_t compass_data_q;

void uart3_init(void)
{
	u3.init(compass_baudrate,rx_q,tx_q);
}

void uart_rx(void)
{
	const char s[2] = ",";
	char rx_buff[10];
	char rx_str[10];
    u3.gets(rx_buff, sizeof(rx_buff), 0);         //get data from compass module

    strcpy(rx_str,rx_buff);                       //copy the compass data to a temporary string
	temp = strtok(rx_str,s);                      //Separate the data by ','
	printf("%s\n",temp);
}
