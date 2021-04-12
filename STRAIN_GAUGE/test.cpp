#include "mbed.h"
#include "STRAIN_GAUGE.h"

#define DEBUG_TX         PA_9
#define DEBUG_RX         PA_10
#define DEBUG_BAUDRATE   9600

#define HX711_CLK_PIN    PA_12
#define HX711_DATA_PIN   PB_7

void test() {
    HX711 hx711(HX711_CLK_PIN, HX711_DATA_PIN);
    STRAIN_GAUGE sg(&hx711, LED1);
    
    //sg_set_calibration(151175, 44);
    
    if(!sg.auto_calibration(100)){
        wait(1);
        sg.auto_calibration(100);
    }
    
    while(1) {
        if(sg.sample()) sg.get_weight();   
        
        wait(2);
    }
}
