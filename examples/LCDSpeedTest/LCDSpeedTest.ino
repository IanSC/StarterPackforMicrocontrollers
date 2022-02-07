//==============================
//  StarterPack LCD Speed Test
//  2022-01 isc
//==============================
//
//  compare speed of:
//  - buffered display
//  - throttled output
//  - regular lcd
//
//  Results: Arduino Uno + LCD Keypad Shield
//
//        lcd update time = 10, lcd throttle = 50
//        buffered trottle = 100, direct throttle = 100
//           Buffered           1 = 15.50
//           Throttled          1 = 0.43
//           Buffered/Throttled 1 = 0.79
//        lcd update time = 30, lcd throttle = 100
//        buffered trottle = 100, direct throttle = 100
//           Buffered           2 = 15.40
//           Throttled          2 = 0.43
//           Buffered/Throttled 2 = 0.79
//        lcd update time = 30, lcd throttle = 500
//        buffered trottle = 1, direct throttle = 1
//           Buffered           3 = 15.31
//           Throttled          3 = 1.92
//           Buffered/Throttled 3 = 1.01
//        
//        Buffered              4 = 1.27
//        Direct (c u tom)        = 51.32
//        Increment Only          = 0.00
//
//  Buffered is 4x faster than non-buffered.
//  It is slowed down due to conversion from number to string,
//     even though not all were transferred to screen. 
//  For regular use, say pots and basic info, buffered is good
//     enough. It can give a boost without any change to existing
//     sketches. Change tracking can be eliminated in most cases.
//  If buffered is not good enough, then use throttled.
//  In this demo, there are no time consuming operations, so the
//     counter incremented so fast, that only a handful of 
//     writes were throttled to screen.

#include <StarterPack.h>       // include all
#include <LCD_wired.h>         // ... or only those used in this project
#include <LCDBuffered_wired.h>
#include <LCD_i2c.h>
#include <LCDBuffered_i2c.h>
#include <Throttle.h>

using namespace StarterPack;

// choose wired or i2c
#define LCD_Wired // use wired version
//#define LCD_I2C // use i2c version

    #if defined( LCD_Wired )

        const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
        LCD_wired lcdDirect( rs, en, d4, d5, d6, d7 );
            
    #elif defined( LCD_I2C )
    
        LCD_i2c lcdDirect;
        
    #endif

    LCDBuffered lcd( lcdDirect );

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
    }

//
// LOOP
//
    
    inline void showResultInBufferedLCD( uint16_t value ) {
        lcd.printAt( 0, 1, value );
    }
    
    inline void showResultInDirectLCD( uint16_t value ) {
        lcdDirect.printAt( 0, 1, value );
    }
    
    Throttle ttBuffered = Throttle( &showResultInBufferedLCD );
    Throttle ttDirect = Throttle( &showResultInDirectLCD );
    
    void runTest( uint8_t testNo ) {
    
        uint16_t counter;
        uint32_t start;
    
        //
        // BUFFERED
        //
        counter = 0;
        lcdDirect.clear();
        lcdDirect.printStrAtRow( 0, "Buffered" );
        SerialPrintf( "   Buffered           %d = ", testNo );
        start = millis();
        while( counter < 0xFFFF ) {
            lcd.printAt( 0, 1, counter );
            counter++;
            lcd.update();
        }
        Serial.println( ( millis() - start ) / 1000.0 );
    
        //
        // THROTTLED
        //
        counter = 0;
        lcdDirect.clear();
        lcdDirect.printStrAtRow( 0, "Throttled" );
        SerialPrintf( "   Throttled          %d = ", testNo );
        start = millis();
        while( counter < 0xFFFF ) {
            ttDirect.trigger( counter );
            counter++;
        }
        Serial.println( ( millis() - start ) / 1000.0 );
    
        //
        // BUFFERED/THROTTLED
        //
        counter = 0;
        lcdDirect.clear();
        lcdDirect.printStrAtRow( 0, "Buffer/Throttled" );
        SerialPrintf( "   Buffered/Throttled %d = ", testNo );
        start = millis();
        while( counter < 0xFFFF ) {
            ttBuffered.trigger( counter );
            counter++;
            lcd.update();
        }
        Serial.println( ( millis() - start ) / 1000.0 );
    
    }
    
    void showSettings() {
        SerialPrintf( "lcd update time = %d, lcd throttle = %d\n", lcd.updateDurationInMs, lcd.getUpdateThrottleInMs() );
        SerialPrintf( "buffered trottle = %d, direct throttle = %d\n", ttBuffered.throttleInMs, ttDirect.throttleInMs );   
    }
    
    void loop() {
    
        Serial.println();
        Serial.println( "*** START ***" );
    
        //
        // 1
        //
        
        // same as defaults
        lcd.updateDurationInMs = 10;
        lcd.setUpdateThrottleInMs( 50 );
        ttBuffered.throttleInMs = 100;
        ttDirect.throttleInMs = 100;    
    
        showSettings();
        runTest(1);
    
        //
        // 2
        //
        
        lcd.updateDurationInMs = 30;
        lcd.setUpdateThrottleInMs( 100 );
        
        showSettings();
        runTest(2);
    
        //
        // 3
        //
        
        ttBuffered.throttleInMs = 1;
        ttDirect.throttleInMs = 1;
        lcd.updateDurationInMs = 30;
        lcd.setUpdateThrottleInMs( 500 );
    
        showSettings();
        runTest(3);
    
        Serial.println();
    
        uint16_t counter;
        uint32_t start;
    
        //
        // BUFFERED 4
        //
        counter = 0;
        lcdDirect.clear();
        lcdDirect.printStrAtRow( 0, "Buffered" );
        SerialPrintf( "Buffered              %d = ", 4 );
        start = millis();
        while( counter < 0xFFFF ) {
            lcd.printAt( 0, 1, "." );
            counter++;
            lcd.update();
        }
        Serial.println( ( millis() - start ) / 1000.0 );
    
        //
        // DIRECT
        //
        counter = 0;
        lcdDirect.clear();
        lcdDirect.printStrAtRow( 0, "Direct" );
        Serial.print( "Direct                  = " );
        start = millis();
        while( counter < 0xFFFF ) {
            lcdDirect.printAt( 0, 1, counter );
            counter++;
        }
        Serial.println( ( millis() - start ) / 1000.0 );
    
        //
        // INCREMENT ONLY
        //
        lcdDirect.clear();
        SerialPrintf( "Increment Only          = " );
        start = millis();
        uint32_t i = 0, j = 0;
        for( i = 0; i < 0xFFFFFFFF ; i++ ) {
            for( j = 0; j < 0xFFFFFFFF ; j++ ) {
                counter = 0;
                while( counter < 0xFFFF ) {
                    counter++;
                }
            }
        }
        Serial.print( millis() - start ); Serial.println( " ms" );
        Serial.println();
        // did it run?
        Serial.println( i );
        Serial.println( j );
        Serial.println( counter );
    }
