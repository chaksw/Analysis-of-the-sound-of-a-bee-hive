#ifndef MBED_APP_H
#define MBED_APP_H

/*
    Section : header files
*/
#include "mbed/mbed.h"

/*
    Section : definitions
*/

//
//!!!!disconnect uart sigfox bus for debug app
//#define APP_DEBUG_MODE
//
#define APP_DEBUG_TX                    PA_9
#define APP_DEBUG_RX                    PA_10
#define APP_DEBUG_BAUDRATE              9600

//
#define APP_TEST_MODE 
//
#define APP_SM_TEST_MODE                10   //in sec > 10sec
#define APP_SM_NORMAL_MODE              600  //SM period 10min
#define APP_SM_WINTER_MODE              1800 //SM period 30min

#define APP_STRAINE_GAUGE_ZERO_CODE     1286689
#define APP_STRAINE_GAUGE_DELTA         11
#define APP_STRAINE_GAUGE_WEIGHT_REF    174 //gram

//enum app state machine
typedef enum{
    APP_STATE_WAIT_INIT = 0,
    APP_STATE_INIT_OK,
    APP_STATE_READ_SENSORS,
    APP_STATE_READ_AUDIO,
    APP_STATE_CHECK_BEE_STATE,
    APP_STATE_SEND_DATA,
    APP_STATE_SLEEP,
    APP_STATE_ERROR
}APP_STATE; 

typedef struct{
    uint8_t ext_temperature;
    uint8_t ext_humidity;
    uint8_t int_temperature;
    uint8_t int_humidity;
    uint16_t hive_weight;
    uint8_t bee_state;
    uint8_t error;
}SIGFOX_Trame;

/*
    Section : interface functions
*/
APP_STATE app_init();

void app_loop();



#endif