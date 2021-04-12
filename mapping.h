#ifndef MBED_MAPPING_H
#define MBED_MAPPING_H

#include "mbed.h"

//time pin
#define APP_TIME_PIN                    PA_5

//init led pin
#define APP_LED                         PB_3                        

//debug uart pin
#define DEBUG_TX                        PA_9
#define DEBUG_RX                        PA_10

//DHT22 pin
#define DHT22_DATA_PIN                  PB_4

//DHT11 pin
#define DHT11_DATA_PIN                  PB_5

//sigfox module uart pin
#define SIGFOX_TX                       PA_9
#define SIGFOX_RX                       PA_10


//STRAIN_GAUGE pin
#define STRAIN_GAUGE_CALIBRATION_PIN    PA_0
#define STRAIN_GAUGE_CALIBRATION_LED    PA_5//PB_3
#define HX711_CLK_PIN                   PB_0
#define HX711_DATA_PIN                  PB_6

#endif