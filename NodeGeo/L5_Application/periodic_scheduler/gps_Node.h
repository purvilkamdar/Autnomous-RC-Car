/*
 * gps_Node.h
 *
 *  Created on: Oct 13, 2016
 *      Author: ythao
 */
#ifndef L5_APPLICATION_PERIODIC_SCHEDULER_GPS_NODE_H_
#define L5_APPLICATION_PERIODIC_SCHEDULER_GPS_NODE_H_

/*
 * Defines all GPS Address types.
 * Converts these types into string so that strcpy() function can take them in as a parameter
 */
#define GPS C(GNVTG)C(GNGGA)C(GNGSA)C(GPGSV)C(GLGSV)C(GNGLL)C(GNRMC)C(GPRMC)
#define C(x) x,
enum gps_name { GPS CODE };
#undef C
#define C(x) #x,
const char * const gps_addr[] = { GPS };


/* Defines each 0.000001 decimal degree equals to the meter value meterPerDecimalDegree produces at provided latitude.
 * @meterPerDecimalDegree This equation is calculated from "Degree precision versus length" table from wikipedia.org
 * 		At different latitude, the amount of distance per 0.000001 decimal degree are different
 * These values are used to calculate the distance between the car and its targeted destination or way point.
 */
#define decimalDegrees 0.000001
#define meterPerDecimalDegree(latitude) ((0.00005*pow(latitude,3) - 0.01912*pow(latitude,2) + 0.02642*latitude + 111.32)*0.001)

/* Performs an offset to the coordinates received from GPS module to match closer to what google map gives */
#define LATTIDUE_OFFSET 0.000011;
#define LONGITUDE_OFFSET 0.000000;

#define LCD_ADDR 0x3F

typedef struct{
	int valid_bit;
	uint16_t counter;
	double latitude;
	double longitude;
}GPS_DATA;

void serialInit(void);

void check_reset_canbus(void);

float floatToDecimalDegree(float strDegree);

double angleOfError(GPS_DATA *gps_data, double destination_lat, double destination_long, double compass_angle);

double distanceToTargetLocation(GPS_DATA *gps_data, double destination_lat, double destination_long);

void get_GPS(gps_name addr, GPS_DATA *data_r);
/*----- GPS Address and their Types of Data-----*/
/*
GNVTG - 	Course and speed relative to the ground.

GNGGA - 	Time, position, and fix related data of the receiver.

GNGSA x2 - 	Used to represent the ID’s of satellites which are used for position fix. When both GPS and GLONASS
			satellites are used in position solution, a $GNGSA sentence is used for GPS satellites and another
			$GNGSA sentence is used for GLONASS satellites. When only GPS satellites are used for position fix, a
			single $GPGSA sentence is output. When only GLONASS satellites are used, a single $GLGSA sentence
			is output.

GPGSV x3 -	Satellite information about elevation, azimuth and CNR, $GPGSV is used for GPS satellites, while
			$GLGSV is used for GLONASS satellites

GLGSV -		Satellite information about elevation, azimuth and CNR, $GPGSV is used for GPS satellites, while
			$GLGSV is used for GLONASS satellites

GNGLL - 	Position, time and fix status.

GNRMC - 	Time, date, position, course and speed data.

GPRMC - 	Time, date, position, course and speed data.
 */

bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8]);

void sendCan1_Any_Message(uint32_t id, uint32_t frame, uint32_t data_len, uint64_t data);


#endif /* L5_APPLICATION_PERIODIC_SCHEDULER_GPS_NODE_H_ */
