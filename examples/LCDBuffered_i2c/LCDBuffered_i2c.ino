//========================================
//  StarterPack Buffered I2C LCD Example
//  2022-01 isc
//========================================
//
//  LCD Buffered I2C
//  ----------------
//  - buffered display for i2c LCD
//  - user updates are kept
//  - actual updates to screen are trickled
//  - additional helper functions

#include <StarterPack.h>     // include all
#include <LCDBuffered_i2c.h> // ... or only those used in this project

using namespace StarterPack;

//
// TEST CASE
//

// change case# to 1, 2 or 3 to test different condidtions
#define CASE 1

#if CASE == 1

    // default to "Wire", address 0x27
    LCDBuffered_i2c lcd; // or lcd( 0x27 );

#elif CASE == 2

    // for ESP32 only, use Wire1
    LCDBuffered_i2c lcd( Wire1 );

#elif CASE == 3

    // for ESP32 only, use Wire1
    // SDA=16, SCL=17
    LCDBuffered_i2c lcd( Wire1 );

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
    // must send special characters here
    // in case power is disconnected and recovered
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
    lcd.printStrAtLine( 0, "ABCDEFGHIJKLMNOP" );
    lcd.printStrAtLine( 1, "abcdefghijklmnop" );
    
    // see trickle in action
    // use only a bit of processor time
    // to update the screen on each call
    lcd.update();
    delay(1000);
    lcd.update();
    delay(1000);
    lcd.update();
    delay(1000);

    // force to fully flush all changes
    lcd.updateAllNow();
    delay(1000);

    lcd.clear();    
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

    lcd.update();

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
