#include "HX711.h"

HX711::HX711(PinName clk_pin, PinName data_pin, uint8_t gain){
    this->data_pin = new DigitalIn(data_pin);
    this->clk_pin = new DigitalOut(clk_pin);
    
    this->clk_pin->write(0);
    this->set_gain(gain);
}

bool HX711::set_gain(uint8_t gain) {
    switch (gain) {
        case 128:       // channel A, gain factor 128
            this->gain = 1;
            break;
        case 64:        // channel A, gain factor 64
            this->gain = 3;
            break;
        case 32:        // channel B, gain factor 32
            this->gain = 2;
            break;
    }
    //set gain for next conversion 
    return(this->sample());
}

bool HX711::isReady(){
    return(this->data_pin->read() == 0);
}

uint8_t HX711::readByte(){
    uint8_t byte(0);
    for(uint8_t i(0); i < 8; ++i) {
        this->clk_pin->write(1);
        wait_us(10); //sck high time (T3): 0.2<T3<50 us, osc : 13us
        byte |= this->data_pin->read() << (7 - i);
        this->clk_pin->write(0);
        wait_us(10); //sck low time (T4): 0.2<T4 us, osc : 13us
    }
    return(byte);  
}

bool HX711::sample(){
    uint8_t wait(0);
    while (!this->isReady() && (wait++ < 10)); wait_us(10);
    if(wait == 10) return(false);

    uint8_t data[3] = { 0 };
    uint8_t filler = 0x00;

    // pulse the clock pin 24 times to read the data
    data[2] = this->readByte();
    data[1] = this->readByte();
    data[0] = this->readByte();

    // set the channel and the gain factor for the next reading using the clock pin
    for (uint8_t i(0); i<this->gain; i++) {
        this->clk_pin->write(1);
        wait_us(10); //sck high time (T3): 0.2<T3<50 us, osc : 13us
        this->clk_pin->write(0);
        wait_us(10); //sck low time (T4): 0.2<T4 us, osc : 13us
    }

    // Datasheet indicates the value is returned as a two's complement value
    // Flip all the bits
    data[2] = ~data[2];
    data[1] = ~data[1];
    data[0] = ~data[0];

    // Replicate the most significant bit to pad out a 32-bit signed integer
    if ( data[2] & 0x80 ) {
        filler = 0xFF;
    } else if ((0x7F == data[2]) && (0xFF == data[1]) && (0xFF == data[0])) {
        filler = 0xFF;
    } else {
        filler = 0x00;
    }
    
    // Construct a 32-bit signed integer
    this->_data = ( static_cast<uint32_t>(filler)  << 24
                  | static_cast<uint32_t>(data[2]) << 16
                  | static_cast<uint32_t>(data[1]) << 8
                  | static_cast<uint32_t>(data[0]) );

    // ... and add 1
    static_cast<int>(++this->_data);

/*
    if(data[0] & 0x80){
        data[0] = ~data[0]; 
        data[1] = ~data[1]; 
        data[2] = ~data[2];   
    }
    this->_data = data[0];
    this->_data = this->_data << 8;
    this->_data |= data[1];
    this->_data = this->_data << 8;
    this->_data |= data[2];
    if(data[0] & 0x80) {
        ++this->_data;
        this->_data = -this->_data;
    }
*/
    
    return(true);
}

void HX711::powerDown(){
    this->clk_pin->write(0);
    wait_us(10);
    this->clk_pin->write(1);
}

/*
When SCK returns to low,chip will reset and enter normal operation mode.
After a reset or power-down event, inputselection is default to Channel A 
with a gain of 128.
*/
void HX711::powerUp(){
    this->clk_pin->write(0);
    //wait(1); //>400ms
    //this->set_gain(this->gain);
}