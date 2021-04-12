#include "sigfox.h"

Sigfox::Sigfox(PinName tx, PinName rx){
    this->serial = new Serial(tx, rx, SIGFOX_BAUDRATE);
}


void Sigfox::flushSerialBuffer(void){
     while(this->serial->readable() == 1) this->serial->getc();
     return; 
}

void Sigfox::sleep(void){
    this->serial->printf("AT$P=1\n\r");
}

void Sigfox::getUp(void){
    this->serial->printf("AT$P=0\n\r");
}

bool Sigfox::waitOK(uint32_t timeOut){
    this->flushSerialBuffer();
    uint8_t response[2];
    uint32_t count(0);
    uint32_t wait(timeOut*100);
    while((this->serial->readable() == 0)&&(count++ < wait)) wait_us(10);
    if(count == wait) return(false); //timeOut ms
    response[0] = (uint8_t)this->serial->getc(); // block until a character is available
    wait = 0;
    while((this->serial->readable() == 0)&&(count++ < wait)) wait_us(10);
    if(count == wait) return(false); //timeOut ms
    response[1] = (uint8_t)this->serial->getc(); // block until a character is available
    if(response[0] == 'O' && response[1] == 'K') return(true);
    return(false); 
}

bool Sigfox::at(){
    //this->flushSerialBuffer();
    //send AT command
    this->serial->printf("AT\n\r");
    //check sigfox module response
    return(this->waitOK(1)); //time out = 1ms
}

bool Sigfox::sendTrame(uint8_t ext_temperature, uint8_t ext_humidity, uint8_t int_temperature, uint8_t int_humidty, uint16_t hive_weight, uint8_t bee_state, uint8_t error){
    //this->flushSerialBuffer();
    //send AT$SF command with data
    this->serial->printf("AT$SF=%02X%02X%02X%02X%04X%02X%02X\n\r", ext_temperature, ext_humidity, int_temperature, int_humidty, hive_weight, bee_state, error);
    //check sigfox module response
    return(this->waitOK(1)); //time out = 1ms
}

bool Sigfox::getId(uint8_t *id){
    //send command
    this->serial->printf("AT$I=10\n\r");
    //read id
    uint8_t count(0);
    for(uint8_t i(0); i < 8; i++){
        while((this->serial->readable() == 0)&&(count++ < 100)) wait_us(10);
        if(count == 100) return(false); //time out~1ms
        id[i] = (uint8_t)this->serial->getc(); // block until a character is available
    }
    return(true);
}

/*
bool Sigfox::send(uint8_t* data, uint8_t size){
    Serial serial(this->tx, this->rx, this->baudrate);
    
    //send data with AT$SF command 
    //serial.printf("AT$SF=99\n\r");
    serial.printf("AT$SF=");
    for(unsigned i(0); i<size; ++i){
        serial.putc(data[i]);
    }
    serial.putc('\n');
    serial.putc('\r');
    
    //check sigfox module response
    //if(serial.readable() == 1)
    uint8_t response[2];
    response[0] = (uint8_t)serial.getc(); // block until a character is available
    response[1] = (uint8_t)serial.getc(); // block until a character is available
    if(response[0] == 'O' && response[1] == 'K') return(true);
    return(false);
}
*/
