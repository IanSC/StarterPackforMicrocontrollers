//  2022/01 - isc
//
//  LCD Utilities
//  -------------
//
//      chooser
//      

#pragma once

#if defined(ESP8266) || defined(ESP32)
    #include <functional>
#endif

namespace StarterPack {

namespace spLCD {

    #if defined(ESP8266) || defined(ESP32)
        typedef std::function<bool(void)> backgroundProcessDelegate;
    #else
        typedef bool (*backgroundProcessDelegate)();
    #endif

};

}
