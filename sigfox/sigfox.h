#ifndef MBED_SIGFOX_H
#define MBED_SIGFOX_H

#include "mbed.h"

#define SIGFOX_BAUDRATE     9600

class Sigfox{
 
public:
    Sigfox(PinName tx, PinName rx);
    
    bool at();
    void sleep(void);
    void getUp(void);
    //bool send(uint8_t* data, uint8_t size);
    bool sendTrame(uint8_t ext_temperature, uint8_t ext_humidity, uint8_t int_temperature, uint8_t int_humidty, uint16_t hive_weight, uint8_t bee_state, uint8_t error);
    
    
    bool getId(uint8_t *id); //a teester

protected:
    void flushSerialBuffer(void);
    
private: 
    Serial *serial;
    int baudrate;
    
    bool waitOK(uint32_t timeOut); //ms
    
    
};

#endif