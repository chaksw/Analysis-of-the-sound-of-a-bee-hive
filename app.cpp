/*
    Section : header files
*/
#include "app.h"
#include "mapping.h"
#include "sigfox.h" 
#include "DHTxx.h"
#include "STRAIN_GAUGE.h"

/*
    Section : global variables
*/

//
DigitalOut app_time_pin(APP_TIME_PIN);

//debug uart
#if defined(APP_DEBUG_MODE)
Serial _debug(APP_DEBUG_TX, APP_DEBUG_RX, APP_DEBUG_BAUDRATE);
#endif

//state machine variable
APP_STATE appState;

//state machine timer
Ticker SMticker;

//LED1 as DigitalOut object 
DigitalOut app_led(APP_LED);

//sigfox object
Sigfox sigfox(SIGFOX_TX, SIGFOX_RX);
//data to send struct
SIGFOX_Trame sigfoxTrame = {0};

//DHT22 object
DHTxx dht22Obj(DHT22, DHT22_DATA_PIN);

//DHT11 object
DHTxx dht11Obj(DHT11, DHT11_DATA_PIN);

//STRAINE_GAUGE object
HX711 hx711Obj(HX711_CLK_PIN, HX711_DATA_PIN);
STRAIN_GAUGE sgObj(&hx711Obj, STRAIN_GAUGE_CALIBRATION_LED);
DigitalIn sg_pin(STRAIN_GAUGE_CALIBRATION_PIN);

/*
    Section : call back functions
*/

void SMticker_Handler(){
    appState = APP_STATE_READ_SENSORS;
}

/*
    Section : local functions
*/

static void read_sensors(void){
    bool readOK(false);
    uint8_t i(0);
    
    /*read externe temperature & humidity (DHT22 sensor)*/
    
    #if defined(APP_DEBUG_MODE)
    _debug.printf(">read DHT22...\n\r");
    #endif
    do{
        readOK = dht22Obj.sample();
        i++;
    }while(!readOK && i < 4);
    if(readOK){
        sigfoxTrame.ext_humidity = dht22Obj.getHumidity_u8();
        sigfoxTrame.ext_temperature = dht22Obj.getTemperature_u8(); //MSB signed bit 
        //if(sigfoxTrame.ext_temperature & 0x80){ //negative temperature
        #if defined(APP_DEBUG_MODE)
        _debug.printf(">DHT22>%dC\t%dH%\n\r", sigfoxTrame.ext_temperature, sigfoxTrame.ext_humidity);
        #endif  
    }else{
        #if defined(APP_DEBUG_MODE)
        _debug.printf(">DHT22>error\n\r");
        #endif  
        sigfoxTrame.error |= 0b00000011; //set bit error
        sigfoxTrame.ext_humidity = 0;
        sigfoxTrame.ext_temperature = 0;
    }
    //dht22Obj.sample();

    /*read interne temperature & humidity (DHT11 sensor)*/

    #if defined(APP_DEBUG_MODE)
    _debug.printf(">read DHT11...\n\r");
    #endif
    do{
        readOK = dht11Obj.sample();
        i++;
    }while(!readOK && i < 4);
    if(readOK){
        sigfoxTrame.int_humidity = dht11Obj.getHumidity_u8();
        sigfoxTrame.int_temperature = dht11Obj.getTemperature_u8();
        #if defined(APP_DEBUG_MODE)
        _debug.printf(">DHT11>%dC\t%dH%\n\r", sigfoxTrame.int_temperature, sigfoxTrame.int_humidity);
        #endif  
    }else{
        #if defined(APP_DEBUG_MODE)
        _debug.printf(">DHT11>error\n\r");
        #endif  
        sigfoxTrame.error |= 0b00001100; //set bit error
        sigfoxTrame.int_humidity = 0;
        sigfoxTrame.int_temperature = 0;
    }

    /*read hive weight (DHT711)*/
    
    #if defined(APP_DEBUG_MODE)
    _debug.printf(">read STRAIN_GAUGE...\n\r");
    #endif  
    //
    #if defined(APP_DEBUG_MODE)
    _debug.printf(">power up HX711... \n\r");
    #endif  
    hx711Obj.powerUp(); //normal mode
    wait(1); //>400ms
    //    
    readOK = false;
    i = 0;
    do{
        readOK = sgObj.sample(); 
        i++;
    }while(!readOK && i < 4);
    //
    #if defined(APP_DEBUG_MODE)
    _debug.printf(">power down HX711... \n\r");
    #endif  
    hx711Obj.powerDown(); //standby mode
    //
    if(readOK){
        int weight = sgObj.get_weight();
        #if defined(APP_DEBUG_MODE)
        _debug.printf(">STRAIN_GAUGE>%dg\n\r", weight);
        #endif 
        if(weight < 0) sigfoxTrame.hive_weight = 0;
        else sigfoxTrame.hive_weight = (uint16_t)weight;
    }else{
        #if defined(APP_DEBUG_MODE)
        _debug.printf(">STRAIN_GAUGE>error\n\r");
        #endif  
        sigfoxTrame.error |= 0b00010000; //set bit error
        sigfoxTrame.hive_weight = 0;
    }
    
    //...
    
}

static void read_audio(void){
    //...
    sigfoxTrame.bee_state = 0;
}

static void check_bee_state(void){
    //...
}

static void send_data(void){
    /*send sigfox trame*/
    //
    #if defined(APP_DEBUG_MODE)
    _debug.printf(">get up sigfox module... \n\r");
    #endif  
    sigfox.getUp(); //standby mode
    wait(2);
    //
    #if defined(APP_DEBUG_MODE)
    _debug.printf(">send data with sigfox module... \n\r");
    #endif  
    if(sigfox.sendTrame(sigfoxTrame.ext_temperature, sigfoxTrame.ext_humidity, sigfoxTrame.int_temperature, sigfoxTrame.int_humidity, sigfoxTrame.hive_weight, sigfoxTrame.bee_state, sigfoxTrame.error)){
        #if defined(APP_DEBUG_MODE)
        _debug.printf(">SIGFOX>send data OK\n\r");
        #endif     
    }else{
        #if defined(APP_DEBUG_MODE)
        _debug.printf(">SIGFOX>error\n\r");
        #endif  
    }
    #if defined(APP_DEBUG_MODE)
    _debug.printf(">turn sigfox module to sleep mode...\n\r");
    #endif
    //
    sigfox.sleep();
    //
}

/*
    Section : interface functions
*/
APP_STATE app_init(){
    app_led.write(0);
    appState = APP_STATE_WAIT_INIT;
    #if defined(APP_DEBUG_MODE)
    _debug.printf(">app init...\n\r");
    #endif
    
    //...
    
    /*STRAINE_GAUGE calibration*/
    
    if(sg_pin.read() == 1){ //auto calibration 
        #if defined(APP_DEBUG_MODE)
        _debug.printf(">STRAINE_GAUGE auto calibration...\n\r");
        #endif       
        if(!sgObj.auto_calibration(APP_STRAINE_GAUGE_WEIGHT_REF)){
            #if defined(APP_DEBUG_MODE)
            _debug.printf(">STRAINE_GAUGE>error calibration\n\r");
            #endif            
            return(APP_STATE_ERROR);
        }else{
            #if defined(APP_DEBUG_MODE)
            _debug.printf(">STRAINE_GAUGE>calibration OK\n\r");
            #endif  
        }
    }else{ //static calibration
         #if defined(APP_DEBUG_MODE)
         _debug.printf(">STRAINE_GAUGE static calibration...\n\r");
         #endif
         if(!sgObj.set_calibration(APP_STRAINE_GAUGE_ZERO_CODE, APP_STRAINE_GAUGE_DELTA)){
            #if defined(APP_DEBUG_MODE)
            _debug.printf(">STRAINE_GAUGE>error calibration\n\r");
            #endif            
            return(APP_STATE_ERROR);
        }else{
            #if defined(APP_DEBUG_MODE)
            _debug.printf(">STRAINE_GAUGE>calibration OK\n\r");
            #endif  
            //sgObj.sample(); 
        }      
    }
    
    /*Turn sensors to sleep mode*/
    
    #if defined(APP_DEBUG_MODE)
    _debug.printf(">turn sensors to sleep mode... \n\r");
    #endif  
    //DHT22 is in standby mode
    //DS18B20 is in standby mode
    hx711Obj.powerDown(); //power down mode
    sigfox.sleep(); //sleep mode
    
    /*attach call back for sleep state*/
    
    #if defined(APP_TEST_MODE)
    SMticker.attach(&SMticker_Handler, APP_SM_TEST_MODE); 
    #elif defined(APP_WINTER_MODE) 
    SMticker.attach(&SMticker_Handler, APP_SM_WINTER_MODE ); //30min
    #else
    SMticker.attach(&SMticker_Handler, APP_SM_NORMAL_MODE ); //10min
    #endif
    
    //
    wait(2);
    //

    #if defined(APP_DEBUG_MODE)
    _debug.printf(">app init OK\n\r");
    #endif
    appState = APP_STATE_INIT_OK;
    app_led.write(1);

    return(appState);
}

void app_loop(){
    /*app state machine*/
    switch(appState){
        case APP_STATE_INIT_OK:
            appState = APP_STATE_READ_SENSORS;
            break;
        case APP_STATE_READ_SENSORS:
//
app_time_pin.write(1);
//
            #if defined(APP_DEBUG_MODE)
            _debug.printf(">read sensors...\n\r");
            #endif
            sigfoxTrame.error = 0;
            read_sensors();
            appState = APP_STATE_READ_AUDIO;
            break; 
         case APP_STATE_READ_AUDIO:
            #if defined(APP_DEBUG_MODE)
            _debug.printf(">read audio...\n\r");
            #endif
            read_audio();
            appState = APP_STATE_CHECK_BEE_STATE;
            break;     
         case APP_STATE_CHECK_BEE_STATE:
            #if defined(APP_DEBUG_MODE)
            _debug.printf(">check to bee state...\n\r");
            #endif
            check_bee_state();
            appState = APP_STATE_SEND_DATA;
            break;  
         case APP_STATE_SEND_DATA:
            #if defined(APP_DEBUG_MODE)
            _debug.printf(">send data...\n\r");
            #endif
            send_data();
            appState = APP_STATE_SLEEP;
            break; 
         case APP_STATE_SLEEP:
//
app_time_pin.write(0);
//
            #if defined(APP_DEBUG_MODE)
            _debug.printf(">sleep...\n\r");
            #endif
            //  
            sleep(); //https://os.mbed.com/docs/mbed-os/v5.15/apis/power-management-sleep.html
            //
            //NOP
            //get up with timer interruption (SMticker time out)
            #if defined(APP_DEBUG_MODE)
            _debug.printf(">get up...\n\r");
            #endif          
            break; 
         default:
            break;
    }
}
