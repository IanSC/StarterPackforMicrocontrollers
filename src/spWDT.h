#pragma once

#if defined(ESP32)
    // #include "hal/wdt_hal.h"
    // #include "soc/rtc_wdt.h" 
    #include <esp_task_wdt.h>
#endif

namespace StarterPack {

    void feedTheDog() {
        #if defined(ESP32)
        
        // rtc_wdt_feed();
        
        // https://github.com/espressif/esp-idf/issues/8855
        // wdt_hal_feed();

        // https://esp32.com/viewtopic.php?t=10560
        // yield();
        // vTaskDelay( 1 );

        // https://forum.arduino.cc/t/esp32-esp-wroom-32-and-wdt/1005179/3
        esp_task_wdt_reset();

        #endif
    }

}
