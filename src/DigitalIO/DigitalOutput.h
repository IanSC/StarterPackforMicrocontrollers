//  2023-07 isc

#pragma once

#include <Arduino.h>
#include <stdint.h>

namespace StarterPack {

class DigitalOutput {

    protected:

        int8_t PIN;

    public:
    
        DigitalOutput( const int8_t pin, bool reversedLogic = false ) {
            PIN = pin;
            pinMode( PIN, OUTPUT );
            this->reversedLogic = reversedLogic;
        }

        virtual ~DigitalOutput() {}

    //
    // RAW HIGH/LOW
    //
    public:

        inline void writeRaw( uint8_t value )  {
            // same as digital write
            digitalWrite( PIN, value );
        }

        inline void writeRawHigh() {            
            digitalWrite( PIN, HIGH );
        }

        inline void writeRawLow() {
            digitalWrite( PIN, LOW );
        }

    //
    // LOGICAL ON/OFF
    //
    protected:

        // easier to use, but cost 1 byte
        // if not reversedLogic: ON=HIGH, OFF=LOW
        // if     reversedLogic: ON=LOW , OFF=HIGH
        bool reversedLogic = false;

    public:

        inline void set( bool state ) {
            // set value with logic
            // set(true)  --> turn "on"
            // set(false) --> turn "off"
            if ( state )
                digitalWrite( PIN, !reversedLogic );
            else
                digitalWrite( PIN, reversedLogic );
        }

        inline void turnOn() {
            digitalWrite( PIN, !reversedLogic );
        }

        inline void turnOff() {
            digitalWrite( PIN, reversedLogic );
        }

};

}
