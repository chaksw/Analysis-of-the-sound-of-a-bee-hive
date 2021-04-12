#ifndef MBED_STRAIN_GAUGE_H
#define MBED_STRAIN_GAUGE_H

#include "mbed.h"
#include "HX711.h" 

//
#define STRAIN_GAUGE_DEBUG_MODE
//
#define STRAIN_GAUGE_DEBUG_TX         PA_9
#define STRAIN_GAUGE_DEBUG_RX         PA_10
#define STRAIN_GAUGE_DEBUG_BAUDRATE   9600

#define STRAIN_GAUGE_WEIGHT_REF       100 //gram

class STRAIN_GAUGE {
public:
    STRAIN_GAUGE(HX711 *hx711, PinName pin_led);
    
    bool set_calibration(int zero_code, int delta);
    bool auto_calibration(int weight_ref=STRAIN_GAUGE_WEIGHT_REF); 
    bool is_calibrated(){return(this->isCalibrated);}
    bool sample();
    int get_weight(); //ignore first acquisition
    
private:
    Serial *_debug;
    HX711 *hx711;
    DigitalOut *led;
    bool isCalibrated;
    int zero_code;
    int delta;
    int weight;
    
    //int __get_weight()
    //void blink_led();
    
};

#endif