#include "STRAIN_GAUGE.h"


STRAIN_GAUGE::STRAIN_GAUGE(HX711 *hx711, PinName pin_led) : hx711(hx711){   
    #ifdef STRAIN_GAUGE_DEBUG_MODE
    this->_debug = new Serial(STRAIN_GAUGE_DEBUG_TX, STRAIN_GAUGE_DEBUG_RX, STRAIN_GAUGE_DEBUG_BAUDRATE);
    #endif    
    this->led = new DigitalOut(pin_led);
    this->led->write(0);
    this->isCalibrated = false;
}

bool STRAIN_GAUGE::set_calibration(int zero_code, int delta){
    this->isCalibrated = false;
    
    this->zero_code = zero_code;
    this->delta = delta;
    
    this->isCalibrated = true;
    return(true);
}

int STRAIN_GAUGE::get_weight(){
    return(this->weight);
}

bool STRAIN_GAUGE::auto_calibration(int weight_ref){
    this->isCalibrated = false;
    #ifdef STRAIN_GAUGE_DEBUG_MODE
    this->_debug->printf(">STRAIN_GAUGE>start calibration...\n\r");
    this->_debug->printf(">STRAIN_GAUGE>get zero code...\n\r");
    #endif
    this->led->write(1);
    wait(5);
    /*get zero code*/
    int zeroCode(0);
    for(uint8_t i(0); i<10; i++){
        if(this->hx711->sample()){ 
            zeroCode = this->hx711->getData();
            #ifdef STRAIN_GAUGE_DEBUG_MODE
            this->_debug->printf(">HX711>%d\n\r", zeroCode);
            #endif           
        }
        else{
            #ifdef STRAIN_GAUGE_DEBUG_MODE
            this->_debug->printf(">HX711>error\n\r");
            #endif 
        }
        wait_ms(100);
    }
    this->led->write(0);
    wait(1);
    if(zeroCode <= 0){
        #ifdef STRAIN_GAUGE_DEBUG_MODE
        this->_debug->printf(">STRAIN_GAUGE>error>zero code=0\n\r");
        this->_debug->printf(">STRAIN_GAUGE>calibration ko\n\r");
        #endif
        return(false);
    }
    this->zero_code = zeroCode;
    #ifdef STRAIN_GAUGE_DEBUG_MODE
    this->_debug->printf(">STRAIN_GAUGE>zero code=%d\n\r", this->zero_code);
    #endif

    /*get code for WEIGHT_REF*/
    #ifdef STRAIN_GAUGE_DEBUG_MODE
    this->_debug->printf(">STRAIN_GAUGE>get code for weight ref...\n\r");
    #endif
    int codeRef(0);
    int code(0);
    //~20 sec to put the weight ref
    for(uint8_t i(0); i < 20; i++){
        if(this->hx711->sample()){
            code = this->hx711->getData();
            codeRef = code - this->zero_code;
            #ifdef STRAIN_GAUGE_DEBUG_MODE
            this->_debug->printf(">HX711>%d\n\r", code);
            #endif   
        }else{
            #ifdef CALIBRATION_DEBUG_MODE
            this->_debug->printf(">HX711>error\n\r");
            #endif 
        }
        this->led->write(1);
        wait_ms(200);
        this->led->write(0);
        wait_ms(700);
    }
    if(codeRef <= 0){
        #ifdef STRAIN_GAUGE_DEBUG_MODE
        this->_debug->printf(">STRAIN_GAUGE>error>code ref=%d\n\r", codeRef);
        this->_debug->printf(">STRAIN_GAUGE>calibration ko\n\r");
        #endif 
        return(false);
    }
    #ifdef STRAIN_GAUGE_DEBUG_MODE
    this->_debug->printf(">STRAIN_GAUGE>code ref=%d\n\r", codeRef);
    #endif 
    
    /*delta*/
    this->delta = codeRef/weight_ref;
    if(this->delta == 0){
        #ifdef STRAIN_GAUGE_DEBUG_MODE
        this->_debug->printf(">STRAIN_GAUGE>error>delta=0\n\r");
        this->_debug->printf(">STRAIN_GAUGE>calibration ko\n\r");
        #endif 
        return(false);     
    }
    #ifdef STRAIN_GAUGE_DEBUG_MODE
    this->_debug->printf(">STRAIN_GAUGE>delta=%d\n\r", this->delta);
    this->_debug->printf(">STRAIN_GAUGE>calibration ok\n\r");
    #endif 
    this->isCalibrated = true;
    this->led->write(1);
    return(true);    
}

bool STRAIN_GAUGE::sample(){
    if(!this->is_calibrated()){
        #ifdef STRAIN_GAUGE_DEBUG_MODE
        this->_debug->printf(">STRAIN_GAUGE>error>not calibrated\n\r");
        #endif 
        return(false);
    }
    int code;
    if(this->hx711->sample()){
        code = this->hx711->getData();
        #ifdef STRAIN_GAUGE_DEBUG_MODE
        this->_debug->printf(">HX711>%d\n\r", code);
        #endif   
    }else{
        #ifdef STRAIN_GAUGE_DEBUG_MODE
        this->_debug->printf(">HX711>error\n\r");
        #endif 
        return(false);
    }
    if(code < 0) this->weight = 0;
    else this->weight = (code - this->zero_code)/this->delta;
    
    #ifdef STRAIN_GAUGE_DEBUG_MODE
    this->_debug->printf(">STRAIN_GAUGE>%dg\n\r", this->weight);
    #endif 
    return(true);
}
