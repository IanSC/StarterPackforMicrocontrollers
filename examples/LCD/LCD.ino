//=====================================
//  StarterPack LCD Switching Example
//  2022-01 isc
//=====================================
//
//  LCD
//  ---
//  show similarities and differences
//     of wired, i2c and buffered LCD

#include <StarterPack.h>       // include all
#include <LCD_wired.h>         // ... or only those used in this project
#include <LCDBuffered_wired.h>
#include <LCD_i2c.h>
#include <LCDBuffered_i2c.h>

using namespace StarterPack;

// choose wired or i2c
#define LCD_Wired // use wired version
//#define LCD_I2C // use i2c version

// choose if buffered or not
#define LCD_Buffered // enable buffering

    #if defined( LCD_Wired )
    
        #if !defined( LCD_Buffered )
        
            const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
            LCD_wired lcd = LCD_wired( rs, en, d4, d5, d6, d7 );
            
        #else
        
            const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
            LCDBuffered_wired lcd = LCDBuffered_wired( rs, en, d4, d5, d6, d7 );
            
        #endif
            
    #elif defined( LCD_I2C )
    
        #if !defined( LCD_Buffered )
        
            LCD_i2c lcd = LCD_i2c();
        
        #else
        
            LCDBuffered_i2c lcd = LCDBuffered_i2c();
            
        #endif
    #endif

//
// SETUP
//
    
    void setup() {
        Serial.begin( 115200 );
        while( !Serial );

        #if defined( LCD_I2C )

            Wire.begin();
            lcd.setTimeoutInMs( 100 );
            lcd.setFrequency( 800000 );
                            
        #endif

        lcd.begin( 16, 2 );
        lcd.printAt( 0, 0, "hello" );

        #if defined( LCD_Buffered )
            // flush all changes to screen
            lcd.updateAllNow();
        #endif
    }

//
// LOOP
//

    int counter = 0;
    
    void loop() {

        lcd.printfAt( 0, 1, "%6d", counter++ );

        #if defined( LCD_Buffered )
            // if buffered, need to call update()
            // to transfer user updates to screen
            // it will use minimal processor time
            // on each call, but cannot guarantee that
            // all changes has been sent completely
            // in a single call
            lcd.update();
        #endif
    }
