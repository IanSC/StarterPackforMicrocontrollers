//===============================
//  StarterPack I2C LCD Example
//  2022-01 isc
//===============================
//
//  LCD I2C
//  -------
//  - replacement for <LiquidCrystal_I2C.h>
//    same interface for: wired, i2c and buffered
//    had to replace to catch i2c errors and
//    remove the hardcode to "Wire"
//  - additional helper functions

#include <StarterPack.h>     // include all
#include <LCD_i2c.h>         // ... or only those used in this project

using namespace StarterPack;

//
// TEST CASE
//

// change case# to 1, 2 or 3 to test different condidtions
#define CASE 1

#if CASE == 1

    // default to "Wire", address 0x27
    LCD_i2c lcd; // or lcd( 0x27 );

#elif CASE == 2

    // for ESP32 only, use Wire1
    LCD_i2c lcd( Wire1 );

#elif CASE == 3

    // for ESP32 only, use Wire1
    // SDA=16, SCL=17
    LCD_i2c lcd( Wire1 );

#endif

//
// SETUP
//

uint8_t heartBitmap[8] = {
    0B00000,
    0B01010,
    0B11111,
    0B11111,
    0B11111,
    0B01110,
    0B00100,
    0B00000
};

void lcdGreetings() {
    lcd.createChar( 0, heartBitmap );
    lcd.printStrAtLine( 0, "hello" );
    lcd.setCursor( 3, 1 );
    lcd.print( "there" );
    lcd.write( 0 );
}

void setup() {
    Serial.begin( 115200 );
    Serial.println( "hello" );

    #if CASE == 1
    
        Wire.begin();
    
    #elif CASE == 2

        // no default pins for Wire1
        // must set explicitly
        Wire1.begin( 21, 22 );
    
    #elif CASE == 3

        Wire1.begin( 16, 17 );
        
    #endif
    
    // i2c timeout and frequency
    // if set directly in Wire
    //    Arduino: in microsec (us)
    //    ESP32  : in millisec (ms)    
    lcd.setTimeoutInMs( 100 );
    lcd.setFrequency( 100000 );
    
    lcd.begin(16,2);
    lcdGreetings();
}

//
// LOOP
//

int counter = 0;

void loop() {
    lcd.printfAt( 6, 0, "%6d", counter );
    lcd.printAt( 10, 1, counter );
    lcd.print( "     " );

    // if this is added, possible to recover from i2c errors
    // try disconnecting and reconnecting any/all
    // i2c wires (even power cables)
    if ( lcd.recoverIfHasError() ) {
        // returns true if there was an error
        // and recovery was attempted
        Serial.println( "LCD recovered" );
        lcdGreetings(); // restore all contents
    }
    counter++;
}
