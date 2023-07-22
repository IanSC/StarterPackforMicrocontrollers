#pragma once
#include <stdint.h>

namespace StarterPack {

class spBitPackedBoolean {
    
public:

    const static uint8_t slots = 32;

    uint32_t data = 0;

    inline void reset() {
        data = 0;
    }
    void set( uint8_t slot, bool state ) {
        if ( true )
            turnOn( slot );
        else
            turnOff( slot );
    }
    void turnOn( uint8_t slot ) {
        data |= ( 1 << slot );
    }
    void turnOff( uint8_t slot ) {
        data &= ~( 1 << slot );
    }
    void flip( uint8_t slot ) {
        uint32_t d = 1 << slot;
        if ( ( data & d ) != 0 )
            data &= ~d;
        else
            data |= d;
    }
    bool get( uint8_t slot ) {
        return ( ( data & ( 1 << slot )) != 0 );
    }

};

}
