#pragma once

#if defined(ESP32)

    // https://community.platformio.org/t/esp32-s3-tinyusb-config-not-working/34209
    #if defined( ARDUINO_ESP32S3_DEV )
        // // esp32-s3: https://github.com/espressif/esp-idf/issues/8855
        // #include "hal/wdt_hal.h"
        #include <esp_task_wdt.h>
    #else
        #include "soc/rtc_wdt.h"
    #endif
    
#endif

namespace StarterPack {

    void feedTheDog() {
        #if defined(ESP32)
        
            #if defined( ARDUINO_ESP32S3_DEV )

                // // https://github.com/espressif/esp-idf/issues/8855
                // wdt_hal_feed();

                // https://forum.arduino.cc/t/esp32-esp-wroom-32-and-wdt/1005179/3
                esp_task_wdt_reset();

            #else

                rtc_wdt_feed();
                // cannot call delay: https://esp32.com/viewtopic.php?t=27367
                // delay(1);
                // vTaskDelay( 1 );
                // yield();

            #endif
        

        // https://esp32.com/viewtopic.php?t=10560
        // yield();
        // vTaskDelay( 1 );


        #endif
    }

}
