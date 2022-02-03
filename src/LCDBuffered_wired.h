#pragma once

#include <LCDBuffered.h>
#include <LCD_wired.h>

// #define LCD_READ_WRITE_MODE       // R/W pin of LCD is used, otherwise R/W pin is hardwired as write (low)
// #define LCD_USE_8BIT_PORT         // handle 8 bit data mode
// #define LCD_INCLUDE_READ_ROUTINES // even in R/W mode, option to include reading routines

namespace StarterPack {

class LCDBuffered_wired : public LCDBuffered {

    public:

        ~LCDBuffered_wired() {
            delete lcd;
        }

        #if defined( LCD_USE_8BIT_PORT )
            #if defined( LCD_READ_WRITE_MODE )
                inline LCDBuffered_wired( uint8_t rs, uint8_t rw, uint8_t enable,
                uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 ) {
                    lcd = new LCD_wired( rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7 );
                }
            #else
                inline LCDBuffered_wired( uint8_t rs, uint8_t enable,
                uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 ) {
                    lcd = new LCD_wired( rs, enable, d0, d1, d2, d3, d4, d5, d6, d7 );
                }
            #endif
        #else
            #if defined( LCD_READ_WRITE_MODE )
                inline LCDBuffered_wired( uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 ) {
                    lcd = new LCD_wired( rs, rw, enable, d4, d5, d6, d7 );
                }
            #else
                inline LCDBuffered_wired( uint8_t rs, uint8_t enable, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 ) {
                    lcd = new LCD_wired( rs, enable, d4, d5, d6, d7 );
                }
            #endif
        #endif

};

}
