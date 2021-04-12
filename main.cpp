#include "app.h"

int main() {
    if(app_init() != APP_STATE_INIT_OK) while(1);
    
    while(1) {
        app_loop();
    }
}
