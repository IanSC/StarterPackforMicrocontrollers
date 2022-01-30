#pragma once

#include <LCDBuffered.h>
#include <LCD_wired.h>

namespace StarterPack {

class LCDBuffered_wired : public LCDBuffered {

    public:

        ~LCDBuffered_wired() {
            delete lcd;
        }

        inline LCDBuffered_wired( uint8_t rs, uint8_t rw, uint8_t enable,
        uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 ) {
            lcd = new LCD_wired( rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7 );
        }
        inline LCDBuffered_wired( uint8_t rs, uint8_t enable,
        uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 ) {
            lcd = new LCD_wired( rs, enable, d0, d1, d2, d3, d4, d5, d6, d7 );
        }
        inline LCDBuffered_wired( uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3 ) {
            lcd = new LCD_wired( rs, rw, enable, d0, d1, d2, d3 );
        }
        inline LCDBuffered_wired( uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3 ) {
            lcd = new LCD_wired( rs, enable, d0, d1, d2, d3 );
        }

};

}
