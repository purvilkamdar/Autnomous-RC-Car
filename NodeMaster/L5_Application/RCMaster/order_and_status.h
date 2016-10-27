#ifndef ORDER_AND_STATUS_H
#define ORDER_AND_STATUS_H

enum motor_steer {
	left_full,
	left_half,
	center,
	right_half,
	right_full
};

enum motor_speed {
	full_stop,
	slow_speed,
	medium_speed,
	fast_speed
};

enum app_cmd{
	stop,
	drive
};


typedef struct status_t {
	int app_cmd;
	double gps_lat;
	double gps_long;
	int    compass_heading;
	int    sensor_right;
	int    sensor_left;
	int    sensor_center;
	int    right_state;
	int    left_state;
	int    center_state;
	int    heading_state;
	int    motor_speed;
};


typedef struct order_t {
	int speed_order;
	int steer_order;
};


#endif
