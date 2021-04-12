#ifndef MBED_HX711_H
#define MBED_HX711_H

#include "mbed.h"

class HX711 {
public:
    HX711(PinName clk_pin, PinName data_pin, uint8_t gain = 128);
 
    bool set_gain(uint8_t gain);
    uint8_t get_gain(){return(this->gain);}
       
    bool isReady();
    //A/D Conversion 
    bool sample();
    int getData(){return(this->_data);}
    
    //turn to power down mode
    void powerDown();  
    //reset and turn to normal mode 
    void powerUp();     
    
private:
    int _data;
    uint8_t gain;
    DigitalOut *clk_pin;
    DigitalIn *data_pin;
    
    uint8_t readByte();
    
};

#endif