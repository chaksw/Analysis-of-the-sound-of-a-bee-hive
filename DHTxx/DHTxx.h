#ifndef MBED_DHTXX_H
#define MBED_DHTXX_H

#include "mbed.h"

typedef enum{
    DHT11 = 0,
    DHT22
}DHT_DEVICE;
    
class DHTxx {    
public:
    DHTxx(DHT_DEVICE dht_device, PinName pin);
    bool sample();
    uint16_t getTemperature();
    uint16_t getHumidity();
    uint8_t getTemperature_u8();
    uint8_t getHumidity_u8();
    
private:
    DHT_DEVICE device;
    uint16_t temperature, humidity;
    DigitalInOut *data_pin;
    
};

#endif