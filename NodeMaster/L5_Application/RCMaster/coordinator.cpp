/*
 * coordinator.cpp
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#include "coordinator.h"
#include "../_can_dbc/generated_can.h"
#include "can.h"
#include "io.hpp"
const uint32_t            	SENSOR_DATA__MIA_MS = 3000;
const SENSOR_DATA_t         SENSOR_DATA__MIA_MSG = { 125,125,125 };
const uint32_t              APP_START_STOP__MIA_MS = 3000;
const APP_START_STOP_t      APP_START_STOP__MIA_MSG = { 0 };
const uint32_t              APP_ROUTE_DATA__MIA_MS=3000;
const APP_ROUTE_DATA_t      APP_ROUTE_DATA__MIA_MSG = {0.000,0.000};
const uint32_t              GPS_Data__MIA_MS = 3000;
const GPS_Data_t            GPS_Data__MIA_MSG = {0};
const uint32_t              COMPASS_Data__MIA_MS = 3000;
const COMPASS_Data_t        COMPASS_Data__MIA_MSG = {0};
const uint32_t              MOTOR_STATUS__MIA_MS=3000;
const MOTOR_STATUS_t        MOTOR_STATUS__MIA_MSG = {0};
// For the sake of example, we use global data storage for messages that we receive
SENSOR_DATA_t sensor_can_msg = { 0 };
MOTOR_STATUS_t motor_can_msg = { 0 };
GPS_Data_t gps_can_msg = { 0 };
APP_START_STOP_t app_can_msg = { 0 };
APP_ROUTE_DATA_t app_route_msg = { 0 };
COMPASS_Data_t compass_msg = { 0 };

coordinator::coordinator() {
	// TODO Auto-generated constructor stub
	itsMotorNode  = new MotorControl();
	itsGeoNode    = new GeoControl();
	itsSensorNode = new SensorsControl();
	itsMobileNode = new MobileControl();
	itsTrajectoryEngine = new TrajectoryEngine();

	itsMotorNode->setName();
	itsGeoNode->setName();
	itsSensorNode->setName();
	itsMobileNode->setName();

	itsSensorNode->set_thresholds(MIN_DISTANCE_INCHES, MED_DISTANCE_INCHES);
	itsGeoNode->set_thresholds(MAX_HEADING_ERR_DEG, MED_HEADING_ERR_DEG);


}



void coordinator::on1HzHearbeat(){
	getNodeStatus();

}
bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
    can_msg_t can_msg = { 0 };
    can_msg.msg_id                = mid;
    can_msg.frame_fields.data_len = dlc;
    memcpy(can_msg.data.bytes, bytes, dlc);

    return CAN_tx(can1, &can_msg, 0);
}
bool coordinator::sendHeartbeat(void){
	MASTER_HB_t master_cmd ={0};
		master_cmd.MASTER_SPEED_cmd = order.speed_order;
		master_cmd.MASTER_STEER_cmd = order.steer_order;

			    // This function will encode the CAN data bytes, and send the CAN msg using dbc_app_send_can_msg()
		dbc_encode_and_send_MASTER_HB(&master_cmd);
return true;
}
bool coordinator::getNodeStatus(){

	bool status_received = false;
	sendHeartbeat();
    //if(sendHeartbeat(motor_cmd,steer_cmd))
	//printf("Hearbeat sent!\n");
    can_msg_t can_msg;
    uint32_t node_status_counter=0;
    while (CAN_rx(can1, &can_msg, 0))
       {
           // Form the message header from the metadata of the arriving message
    	  node_status_counter++;
           dbc_msg_hdr_t can_msg_hdr;
           can_msg_hdr.dlc = can_msg.frame_fields.data_len;
           can_msg_hdr.mid = can_msg.msg_id;

           // Attempt to decode the message (brute force, but should use switch/case with MID)
           switch (can_msg_hdr.mid)
           {
             case MOTOR_STATUS:
                 dbc_decode_MOTOR_STATUS(&motor_can_msg, can_msg.data.bytes, &can_msg_hdr);
                 status.motor_speed = motor_can_msg.MOTOR_STATUS_speed_mph;

                // printf("Received status from Motor!:\n");
               //  printf("Motor Data : %d\n",motor_can_msg.MOTOR_STATUS_speed_kph);

        	   break;
             case SENSOR_DATA:
                 dbc_decode_SENSOR_DATA(&sensor_can_msg, can_msg.data.bytes, &can_msg_hdr);

                 status.sensor_center = sensor_can_msg.SENSOR_middle_sensor;
                 status.sensor_right = sensor_can_msg.SENSOR_right_sensor;
                 status.sensor_left = sensor_can_msg.SENSOR_left_sensor;

              break;
             case GPS_Data:
                 dbc_decode_GPS_Data(&gps_can_msg, can_msg.data.bytes, &can_msg_hdr);
                 status.gps_lat = gps_can_msg.GPS_READOUT_latitude;
                 status.gps_long = gps_can_msg.GPS_READOUT_longitude;
//                 printf("GPS_READOUT_latitude : %d\n",gps_can_msg.GPS_READOUT_latitude);
//                 printf("GPS_READOUT_longitude : %d\n",gps_can_msg.GPS_READOUT_longitude);
            	 break;
             case APP_START_STOP:
                 dbc_decode_APP_START_STOP(&app_can_msg, can_msg.data.bytes, &can_msg_hdr);
                // printf("APP_START_STOP_cmd : %d\n",app_can_msg.APP_START_STOP_cmd);
                 status.app_cmd = app_can_msg.APP_START_STOP_cmd;

            	 break;

             default:
            	 printf("Received unknown message ID:%d\n", can_msg_hdr.mid);
            	 break;
           }


       }
    dbc_handle_mia_APP_START_STOP(&app_can_msg,100);
    dbc_handle_mia_SENSOR_DATA(&sensor_can_msg,100);
    dbc_handle_mia_MOTOR_STATUS(&motor_can_msg,100);
    dbc_handle_mia_APP_ROUTE_DATA(&app_route_msg,100);
    dbc_handle_mia_COMPASS_Data(&compass_msg,100);
    dbc_handle_mia_GPS_Data(&gps_can_msg,100);

	return (node_status_counter > 0);// status_received;   TODO restore testing only
}

void coordinator::onStatusReceived(void){

	processAndSendOrder(status, order);
}
void coordinator::processAndSendOrder(status_t& status, order_t& order){

	itsSensorNode->check_sensors(status);
	itsGeoNode->checkHeading(status);
    itsTrajectoryEngine->run_trajectory(status, order);
   /* printf("Steer Order : %d\n",order.steer_order);
    printf("Speed Order : %d\n",order.speed_order);*/
}
coordinator::~coordinator() {
	// TODO Auto-generated destructor stub
}

