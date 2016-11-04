#ifndef ORDER_AND_STATUS_H
#define ORDER_AND_STATUS_H

#define NO_APP

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
	drive,
	drive2,
	drive3,
	reverse
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
	bool   leftCloser;
	bool   rightCloser;
	int    heading_state;
	int    motor_speed;
}status_t;


typedef struct order_t {
	int speed_order;
	int steer_order;
}order_t;


#endif
