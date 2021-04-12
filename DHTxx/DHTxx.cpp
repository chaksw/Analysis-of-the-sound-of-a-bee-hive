#include "DHTxx.h"


DHTxx::DHTxx(DHT_DEVICE dht_device, PinName pin) : device(dht_device){
    this->data_pin = new DigitalInOut(pin);
   this->data_pin->mode(OpenDrain);
}

// uint16_t DHTxx::getTemperature() {
//     if(this->device == DHT22) return(this->temperature);
//     //else DHT211
//     uint8_t temperature_int = this->temperature >> 8;
//     uint8_t temperature_dec = (uint8_t) this->temperature;
//     return((temperature_int*10)+temperature_dec);
// }

// uint16_t DHTxx::getHumidity() {
//     if(this->device == DHT22) return(this->humidity);
//     //else DHT211
//     uint8_t humidity_int = this->humidity >> 8;
//     uint8_t humidity_dec = (uint8_t) this->humidity;
//     return((humidity_int*10)+humidity_dec);
// }

uint8_t DHTxx::getTemperature_u8() {
    if(this->device == DHT22) return((uint8_t)(this->temperature/10));
    //else DHT211
    return((uint8_t)(this->temperature >> 8));
}

uint8_t DHTxx::getHumidity_u8() {
    if(this->device == DHT22) return((uint8_t)(this->humidity/10));
    //else DHT211
    return((uint8_t)(this->humidity >> 8));
}

bool DHTxx::sample() {
    uint8_t wait(0);
    uint8_t result(0);
    uint8_t dht_dat[5];
    this->data_pin->output();
    this->data_pin->write(0);
    //Tbe 
    if(this->device == DHT22) wait_ms(2); 
    else wait_ms(20);
    
    this->data_pin->write(1);
    this->data_pin->input();
    //Tgo
    while (this->data_pin->read() && (wait++ < 100)) wait_us(1);
    if(wait == 100) return(false); //TimeOut 
    //Trel
    wait = 0;
    while (!this->data_pin->read() && (wait++ < 100)) wait_us(1);
    if(wait == 100) return(false); //TimeOut 

    for(uint8_t i(0); i<5; i++) {
        result=0;
        for (uint8_t j(0); j<8; j++) {
            wait = 0;
            while (this->data_pin->read() && (wait++ < 100)) wait_us(1);
            if(wait == 100) return(false); //TimeOut
            wait = 0;
            while (!this->data_pin->read() && (wait++ < 100)) wait_us(1);
            if(wait == 100) return(false); //TimeOut
            wait_us(40);
            uint8_t p;
            p = this->data_pin->read();
            p = p << (7-j);
            result = result|p;
        }
        dht_dat[i] = result;
    }
    
    unsigned dht_check_sum;
    dht_check_sum = dht_dat[0]+dht_dat[1]+dht_dat[2]+dht_dat[3];
    dht_check_sum = dht_check_sum%256;
    if (dht_check_sum == dht_dat[4]) {
        this->humidity = dht_dat[0]*256+dht_dat[1];
        this->temperature=dht_dat[2]*256+dht_dat[3];
        return true;
    }

    return false;
}
