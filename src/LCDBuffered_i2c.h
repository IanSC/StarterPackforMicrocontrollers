#pragma once

#include <LCDBuffered.h>
#include <LCD_i2c.h>

namespace StarterPack {

class LCDBuffered_i2c : public LCDBuffered {

    public:

        ~LCDBuffered_i2c() {
            delete lcd;
        }

        inline LCDBuffered_i2c( int16_t i2cAddress = -1 ) {
            // use default global "Wire"
            lcd = new LCD_i2c( i2cAddress );
        }
        inline LCDBuffered_i2c( TwoWire &wire, int16_t i2cAddress = -1 ) {
            lcd = new LCD_i2c( wire, i2cAddress );
        }
        inline LCDBuffered_i2c( i2cHelper &wireHelper ) {
            lcd = new LCD_i2c( wireHelper );
        }

};

}
