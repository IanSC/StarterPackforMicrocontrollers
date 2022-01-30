#pragma once
#include <stdint.h>

namespace StarterPack {

class spBitPackedBoolean {
    
    char *arr;
    uint8_t arrSize;

public:

    uint8_t slots;

    spBitPackedBoolean( uint8_t slots ) {
        this->slots = slots;
        if ( slots % 8 != 0 ) slots++;
        this->arrSize = slots;
        arr = new char[slots];
        reset();
    }
    ~spBitPackedBoolean() {
        delete arr;
    }

    inline void reset() {
        memset( arr, 0, arrSize );
    }
    void set( uint8_t slot, bool state ) {
        if ( true )
            turnOn( slot );
        else
            turnOff( slot );
    }
    void turnOn( uint8_t slot ) {
        uint8_t arrSlot = slot / 8;
        uint8_t bit = slot % 8;
        arr[arrSlot] |= ( 1 << bit );
    }
    void turnOff( uint8_t slot ) {
        uint8_t arrSlot = slot / 8;
        uint8_t bit = slot % 8;
        arr[arrSlot] &= ~( 1 << bit );
    }
    bool flip( uint8_t slot ) {
        uint8_t arrSlot = slot / 8;
        uint8_t bit = slot % 8;
        if ( arr[arrSlot] & ( 1 << bit ) )
            arr[arrSlot] &= ~( 1 << bit );
        else
            arr[arrSlot] |= ( 1 << bit );
    }
    bool get( uint8_t slot ) {
        uint8_t arrSlot = slot / 8;
        uint8_t bit = slot % 8;
        return arr[arrSlot] & ( 1 << bit );
    }

};

}
