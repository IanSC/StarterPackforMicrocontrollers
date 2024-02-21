#pragma once

#if defined(ESP32)
    #include "soc/rtc_wdt.h"
#endif

namespace StarterPack {

    void feedTheDog() {
        #if defined(ESP32)
        rtc_wdt_feed();
        #endif
    }

}
