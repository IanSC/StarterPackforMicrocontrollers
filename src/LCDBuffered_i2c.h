#pragma once

#include <LCDBuffered.h>
#include <LCD_i2c.h>

namespace StarterPack {

class LCDBuffered_i2c : public LCDBuffered {

    public:

        LCDBuffered_i2c( int16_t i2cAddress = -1 ) {
            // use default global "Wire"
            lcd = new LCD_i2c( i2cAddress );
        }
        LCDBuffered_i2c( TwoWire &wire, int16_t i2cAddress = -1 ) {
            lcd = new LCD_i2c( wire, i2cAddress );
        }
        LCDBuffered_i2c( TwoWireHelper &wireHelper ) {
            lcd = new LCD_i2c( wireHelper );
        }

};

}
