/// DBC file: ../_can_dbc/243.dbc    Self node: 'GPS'  (ALL = 0)
/// This file can be included by a source file, for example: #include "generated.h"
#ifndef __GENEARTED_DBC_PARSER
#define __GENERATED_DBC_PARSER
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>



/// Extern function needed for dbc_encode_and_send()
extern bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8]);

/// Missing in Action structure
typedef struct {
    uint32_t is_mia : 1;          ///< Missing in action flag
    uint32_t mia_counter_ms : 31; ///< Missing in action counter
} dbc_mia_info_t;

/// CAN message header structure
typedef struct { 
    uint32_t mid; ///< Message ID of the message
    uint8_t  dlc; ///< Data length of the message
} dbc_msg_hdr_t; 

// static const dbc_msg_hdr_t DBC_TEST1_HDR =                        {  500, 8 };
// static const dbc_msg_hdr_t DBC_TEST2_HDR =                        {  501, 8 };
// static const dbc_msg_hdr_t DBC_TEST3_HDR =                        {  502, 8 };
static const dbc_msg_hdr_t DRIVER_HEARTBEAT_HDR =                 {  100, 1 };
// static const dbc_msg_hdr_t MOTOR_CMD_HDR =                        {  101, 1 };
// static const dbc_msg_hdr_t MOTOR_STATUS_HDR =                     {  400, 3 };
// static const dbc_msg_hdr_t SENSOR_SONARS_HDR =                    {  200, 8 };
static const dbc_msg_hdr_t GPS_READOUT_HDR =                      {   64, 8 };
// static const dbc_msg_hdr_t DBC_TEST4_HDR =                        {  503, 8 };

/// Enumeration(s) for Message: 'DRIVER_HEARTBEAT' from 'DRIVER'
typedef enum {
    DRIVER_HEARTBEAT_cmd_NOOP = 0,
    DRIVER_HEARTBEAT_cmd_SYNC = 1,
    DRIVER_HEARTBEAT_cmd_REBOOT = 2,
} DRIVER_HEARTBEAT_cmd_E ;




/// Message: DRIVER_HEARTBEAT from 'DRIVER', DLC: 1 byte(s), MID: 100
typedef struct {
    DRIVER_HEARTBEAT_cmd_E DRIVER_HEARTBEAT_cmd; ///< B7:0   Destination: SENSOR,MOTOR,GPS

    dbc_mia_info_t mia_info;
} DRIVER_HEARTBEAT_t;


/// Message: GPS_READOUT from 'GPS', DLC: 8 byte(s), MID: 64
typedef struct {
    uint8_t GPS_READOUT_valid_bit : 1;        ///< B0:0  Min: 0 Max: 1   Destination: DRIVER,MOTOR
    uint16_t GPS_READOUT_read_counter;        ///< B11:1  Min: 0 Max: 2000   Destination: DRIVER,MOTOR
    float GPS_READOUT_latitude;               ///< B37:12   Destination: DRIVER,MOTOR
    float GPS_READOUT_longitude;              ///< B63:38   Destination: DRIVER,MOTOR

    // No dbc_mia_info_t for a message that we will send
} GPS_READOUT_t;


/// @{ These 'externs' need to be defined in a source file of your project
extern const uint32_t                             DRIVER_HEARTBEAT__MIA_MS;
extern const DRIVER_HEARTBEAT_t                   DRIVER_HEARTBEAT__MIA_MSG;
/// @}


/// Not generating code for dbc_encode_DBC_TEST1() since the sender is IO and we are GPS

/// Not generating code for dbc_encode_DBC_TEST2() since the sender is IO and we are GPS

/// Not generating code for dbc_encode_DBC_TEST3() since the sender is IO and we are GPS

/// Not generating code for dbc_encode_DRIVER_HEARTBEAT() since the sender is DRIVER and we are GPS

/// Not generating code for dbc_encode_MOTOR_CMD() since the sender is DRIVER and we are GPS

/// Not generating code for dbc_encode_MOTOR_STATUS() since the sender is MOTOR and we are GPS

/// Not generating code for dbc_encode_SENSOR_SONARS() since the sender is SENSOR and we are GPS

/// Encode GPS's 'GPS_READOUT' message
/// @returns the message header of this message
static inline dbc_msg_hdr_t dbc_encode_GPS_READOUT(uint8_t bytes[8], GPS_READOUT_t *from)
{
    uint32_t raw;
    bytes[0]=bytes[1]=bytes[2]=bytes[3]=bytes[4]=bytes[5]=bytes[6]=bytes[7]=0;

    // Not doing min value check since the signal is unsigned already
    if(from->GPS_READOUT_valid_bit > 1) { from->GPS_READOUT_valid_bit = 1; } // Max value: 1
    raw = ((uint32_t)(((from->GPS_READOUT_valid_bit)))) & 0x01;
    bytes[0] |= (((uint8_t)(raw) & 0x01)); ///< 1 bit(s) starting from B0

    // Not doing min value check since the signal is unsigned already
    if(from->GPS_READOUT_read_counter > 2000) { from->GPS_READOUT_read_counter = 2000; } // Max value: 2000
    raw = ((uint32_t)(((from->GPS_READOUT_read_counter)))) & 0x7ff;
    bytes[0] |= (((uint8_t)(raw) & 0x7f) << 1); ///< 7 bit(s) starting from B1
    bytes[1] |= (((uint8_t)(raw >> 7) & 0x0f)); ///< 4 bit(s) starting from B8

    raw = ((uint32_t)(((from->GPS_READOUT_latitude) / 1e-06) + 0.5)) & 0x3ffffff;
    bytes[1] |= (((uint8_t)(raw) & 0x0f) << 4); ///< 4 bit(s) starting from B12
    bytes[2] |= (((uint8_t)(raw >> 4) & 0xff)); ///< 8 bit(s) starting from B16
    bytes[3] |= (((uint8_t)(raw >> 12) & 0xff)); ///< 8 bit(s) starting from B24
    bytes[4] |= (((uint8_t)(raw >> 20) & 0x3f)); ///< 6 bit(s) starting from B32

    // Stuff a real signed number into the DBC 26-bit signal
    raw = ((uint32_t)(((from->GPS_READOUT_longitude) / 1e-06) + 0.5)) & 0x3ffffff;
    bytes[4] |= (((uint8_t)(raw) & 0x03) << 6); ///< 2 bit(s) starting from B38
    bytes[5] |= (((uint8_t)(raw >> 2) & 0xff)); ///< 8 bit(s) starting from B40
    bytes[6] |= (((uint8_t)(raw >> 10) & 0xff)); ///< 8 bit(s) starting from B48
    bytes[7] |= (((uint8_t)(raw >> 18) & 0xff)); ///< 8 bit(s) starting from B56

    return GPS_READOUT_HDR;
}

/// Encode and send for dbc_encode_GPS_READOUT() message
static inline bool dbc_encode_and_send_GPS_READOUT(GPS_READOUT_t *from)
{
    uint8_t bytes[8];
    const dbc_msg_hdr_t hdr = dbc_encode_GPS_READOUT(bytes, from);
    return dbc_app_send_can_msg(hdr.mid, hdr.dlc, bytes);
}



/// Not generating code for dbc_encode_DBC_TEST4() since the sender is IO and we are GPS

/// Not generating code for dbc_decode_DBC_TEST1() since 'GPS' is not the recipient of any of the signals

/// Not generating code for dbc_decode_DBC_TEST2() since 'GPS' is not the recipient of any of the signals

/// Not generating code for dbc_decode_DBC_TEST3() since 'GPS' is not the recipient of any of the signals

/// Decode DRIVER's 'DRIVER_HEARTBEAT' message
/// @param hdr  The header of the message to validate its DLC and MID; this can be NULL to skip this check
static inline bool dbc_decode_DRIVER_HEARTBEAT(DRIVER_HEARTBEAT_t *to, const uint8_t bytes[8], const dbc_msg_hdr_t *hdr)
{
    const bool success = true;
    // If msg header is provided, check if the DLC and the MID match
    if (NULL != hdr && (hdr->dlc != DRIVER_HEARTBEAT_HDR.dlc || hdr->mid != DRIVER_HEARTBEAT_HDR.mid)) {
        return !success;
    }

    uint32_t raw;
    raw  = ((uint32_t)((bytes[0]))); ///< 8 bit(s) from B0
    to->DRIVER_HEARTBEAT_cmd = (DRIVER_HEARTBEAT_cmd_E)((raw));

    to->mia_info.mia_counter_ms = 0; ///< Reset the MIA counter

    return success;
}


/// Not generating code for dbc_decode_MOTOR_CMD() since 'GPS' is not the recipient of any of the signals

/// Not generating code for dbc_decode_MOTOR_STATUS() since 'GPS' is not the recipient of any of the signals

/// Not generating code for dbc_decode_SENSOR_SONARS() since 'GPS' is not the recipient of any of the signals

/// Not generating code for dbc_decode_GPS_READOUT() since 'GPS' is not the recipient of any of the signals

/// Not generating code for dbc_decode_DBC_TEST4() since 'GPS' is not the recipient of any of the signals

/// Handle the MIA for DRIVER's DRIVER_HEARTBEAT message
/// @param   time_incr_ms  The time to increment the MIA counter with
/// @returns true if the MIA just occurred
/// @post    If the MIA counter reaches the MIA threshold, MIA struct will be copied to *msg
static inline bool dbc_handle_mia_DRIVER_HEARTBEAT(DRIVER_HEARTBEAT_t *msg, uint32_t time_incr_ms)
{
    bool mia_occurred = false;
    const dbc_mia_info_t old_mia = msg->mia_info;
    msg->mia_info.is_mia = (msg->mia_info.mia_counter_ms >= DRIVER_HEARTBEAT__MIA_MS);

    if (!msg->mia_info.is_mia) { // Not MIA yet, so keep incrementing the MIA counter
        msg->mia_info.mia_counter_ms += time_incr_ms;
    }
    else if(!old_mia.is_mia)   { // Previously not MIA, but it is MIA now
        // Copy MIA struct, then re-write the MIA counter and is_mia that is overwriten
        *msg = DRIVER_HEARTBEAT__MIA_MSG;
        msg->mia_info.mia_counter_ms = DRIVER_HEARTBEAT__MIA_MS;
        msg->mia_info.is_mia = true;
        mia_occurred = true;
    }

    return mia_occurred;
}

#endif
