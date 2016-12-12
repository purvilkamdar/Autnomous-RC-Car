/*
 * Geo_Manager.cpp
 *
 *  Created on: Dec 2, 2016
 *      Author: ythao
 */

#include <string.h>
#include <stdio.h>
#include "Geo_Manager.h"
#include "../_can_dbc/generated_can.h"
#include "can.h"
#include "uart2.hpp"
#include "uart3.hpp"
#include "io.hpp"

#define APPLY_FILTER 1

COMPASS_Data_t COMPASS_Value = {0};
GPS_Data_t gps_rx_data = {0};
GEO_Header_t geo_canbus_data = {0};
MASTER_HB_t master_hb = {0};

const uint32_t         MASTER_HB__MIA_MS = 1000;
const MASTER_HB_t      MASTER_HB__MIA_MSG = {0};


GeoManager::GeoManager(){
	my_GPS_module = new GPSModule();
	my_Compass_module = new CompassModule();
	my_GeoCalculator_engine = new Geo_CalculationEngine();

	my_GPS_module->setGPS_Offset(0.000000,0.000000);
	my_Compass_module->setCompass_Offset(13);

}

GeoManager::~GeoManager(){
//	delete[] my_GPS_module;
//	delete[] my_Compass_module;
}

bool GeoManager::run_GeoNode(){

	retrieve_Geo_Data();

#if APPLY_FILTER

	filterAndcompute_Geo_Data();

	can_msg_t can_msg;

	while(CAN_rx(can1, &can_msg,0))
		{
			dbc_msg_hdr_t can_msg_hdr;
			can_msg_hdr.dlc = can_msg.frame_fields.data_len;
			can_msg_hdr.mid = can_msg.msg_id;

			if(dbc_decode_MASTER_HB(&master_hb, can_msg.data.bytes, &can_msg_hdr))
			{
//				printf("From Master Lat = %f\n", master_hb.MASTER_LAT_cmd);
//				printf("From Master Long = %f\n", master_hb.MASTER_LONG_cmd);
				master_data_.m_latitude = master_hb.MASTER_LAT_cmd;
				master_data_.m_longitude = master_hb.MASTER_LONG_cmd;

				getFilter_DistanceAndAngle();

				/*---Assigns calculated angle and distance values to GEO dbc header---*/
				geo_canbus_data.GPS_ANGLE_degree = calculated_geo_data_.c_errorAngle;
				geo_canbus_data.GPS_DISTANCE_meter = calculated_geo_data_.c_distanceToLocation;

				/*---Sends angle and distance data onto CAN bus---*/
				dbc_encode_and_send_GEO_Header(&geo_canbus_data);

				/*---Assigns GPS data to GPS_READOUT header---*/
				gps_rx_data.GPS_READOUT_valid_bit = geo_data_.GPS_message_status;
				gps_rx_data.GPS_READOUT_read_counter = geo_data_.GPS_message_counter;
				gps_rx_data.GPS_READOUT_latitude = geo_data_.latitude;
				gps_rx_data.GPS_READOUT_longitude = geo_data_.longitude;
				/*---Sends GPS Data: Long, Lat, Counter, Valid bit---*/
				dbc_encode_and_send_GPS_Data(&gps_rx_data);

				/*--Sends Compass Header from Compass---*/
				COMPASS_Value.COMPASS_Heading = geo_data_.compass_heading;
				dbc_encode_and_send_COMPASS_Data(&COMPASS_Value);

				printf("\nGPS module Latitude  = %f\n",geo_data_.latitude);
				printf("Calculated Latitude  = %f\n", calculated_geo_data_.c_latitude);
				printf("GPS module Longitude = %f\n", geo_data_.longitude);
				printf("Calculated Longitude = %f\n", calculated_geo_data_.c_longitude);
			}
		}
#else

	can_msg_t can_msg;

	while(CAN_rx(can1, &can_msg,0))
		{
			dbc_msg_hdr_t can_msg_hdr;
			can_msg_hdr.dlc = can_msg.frame_fields.data_len;
			can_msg_hdr.mid = can_msg.msg_id;

			if(dbc_decode_MASTER_HB(&master_hb, can_msg.data.bytes, &can_msg_hdr))
			{
//				printf("From Master Lat = %f\n", master_hb.MASTER_LAT_cmd);
//				printf("From Master Long = %f\n", master_hb.MASTER_LONG_cmd);
				master_data_.m_latitude = master_hb.MASTER_LAT_cmd;
				master_data_.m_longitude = master_hb.MASTER_LONG_cmd;

				double error_angle = my_GeoCalculator_engine->angleOfError(geo_data_, master_data_);
				//printf("\nAngle of approach is: %f\n", error_angle);
				double distance_to_checkpoint = my_GeoCalculator_engine->distanceToTargetLocation(geo_data_, master_data_);
				//printf("Distance away from destination: %f meter(s) \n\n", distance_to_checkpoint);

				/*---Assigns calculated angle and distance values to GEO dbc header---*/
				geo_canbus_data.GPS_ANGLE_degree = error_angle;
				geo_canbus_data.GPS_DISTANCE_meter = distance_to_checkpoint;

				/*---Sends angle and distance data onto CAN bus---*/
				dbc_encode_and_send_GEO_Header(&geo_canbus_data);

				/*---Assigns GPS data to GPS_READOUT header---*/
				gps_rx_data.GPS_READOUT_valid_bit = geo_data_.GPS_message_status;
				gps_rx_data.GPS_READOUT_read_counter = geo_data_.GPS_message_counter;
				gps_rx_data.GPS_READOUT_latitude = geo_data_.latitude;
				gps_rx_data.GPS_READOUT_longitude = geo_data_.longitude;

				/*---Sends GPS Data: Long, Lat, Counter, Valid bit---*/
				dbc_encode_and_send_GPS_Data(&gps_rx_data);

				/*--Assign & Send Compass Header from Compass---*/
				COMPASS_Value.COMPASS_Heading = geo_data_.compass_heading;
				dbc_encode_and_send_COMPASS_Data(&COMPASS_Value);
			}
		}

#endif

	/*----MIA handler for MASTER HB signal. Toggles 1 & 4 LEDs on board when in MIA state---*/
	if(dbc_handle_mia_MASTER_HB(&master_hb,100))
	{
		//printf("In MIA State.\n");
		LE.toggle(2);
		LE.toggle(3);
	}


	return true;
}

bool GeoManager::retrieve_Geo_Data(){
	int received = 1;

	if(!(my_GPS_module->updateToManager(geo_data_)))
		{
		printf("Not able to retrieve GPS Data.\n");
		received--;
		}

	if(!(my_Compass_module->updateToManager(geo_data_)))
		{
		printf("Not able to retrieve Compass Data.\n");
		received--;
		}

	if(received > 0)
		return true;	//GPS & Compass Data are received/valid from hardware
	else
		return false;	//Only one or no Data are received/valid from hardware
}

bool GeoManager::filterAndcompute_Geo_Data(){
	my_GeoCalculator_engine->deviation_filter_controller(geo_data_, calculated_geo_data_);
	my_GeoCalculator_engine->linear_regression_computer(calculated_geo_data_);
	return true;
}

bool GeoManager::getFilter_DistanceAndAngle(){
	my_GeoCalculator_engine->distanceToTargetLocation_wCalculatedGeoData(calculated_geo_data_, master_data_);
	my_GeoCalculator_engine->angleOfError_wCalculatedGeoData(calculated_geo_data_, master_data_, geo_data_);
	return true;
}

void GeoManager::serialInit(void){

	I2C2::getInstance().init(100);
	Uart3::getInstance().init(57600,1,0);
	Uart2::getInstance().init(115200,75,0);
	CAN_init(can1,100,5,5,NULL,NULL);
	CAN_init(can2,100,5,5,NULL,NULL);
	const can_std_id_t slist[]  = { CAN_gen_sid(can1, 0x020),  // Acknowledgment from the nodes that received sensor reading
									CAN_gen_sid(can1, 0x021)}; // Only 1 ID is expected, hence small range
	CAN_setup_filter(slist, 2, NULL, 0, NULL, 0, NULL, 0);
	CAN_reset_bus(can1);
	CAN_reset_bus(can2);
}

void GeoManager::check_reset_canbus(void){

	if(CAN_is_bus_off(can1))
		CAN_reset_bus(can1);
	if(CAN_is_bus_off(can2))
		CAN_reset_bus(can2);
}

bool GeoManager::dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8]){

    can_msg_t can_msg = { 0 };
    can_msg.msg_id                = mid;
    can_msg.frame_fields.data_len = dlc;
    memcpy(can_msg.data.bytes, bytes, dlc);

    return CAN_tx(can1, &can_msg, 0);
}



