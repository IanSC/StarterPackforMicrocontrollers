//==========================================
//  StarterPack Buffered Wired LCD Example
//  2022-01 isc
//==========================================
//
//  LCD Wired Buffered
//  ------------------
//  - buffered display for wired LCD
//    eg. LCD Keypad Shield
//  - user updates are kept
//  - actual updates to screen are trickled
//  - additional helper functions

#include <StarterPack.h>       // include all
#include <LCDBuffered_wired.h> // ... or only those used in this project
using namespace StarterPack;

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LCDBuffered_wired lcd = LCDBuffered_wired( rs, en, d4, d5, d6, d7 );

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

void setup() {
    Serial.begin( 115200 );
    Serial.println( "hello" );

    lcd.begin(16,2);
    lcd.createChar( 0, heartBitmap );

    lcd.printStrAtLine( 0, "ABCDEFGHIJKLMNOP" );
    lcd.printStrAtLine( 1, "abcdefghijklmnop" );

    // see trickle in action
    // use only a bit of processor time
    // to update the screen on each call

    // slow down time spent in updating
    // to see trickle, otherwise wired
    // is so fast, cannot see trickle
    lcd.updateDurationInMs = 3;

    lcd.update();
    delay(1000);
    lcd.update();
    delay(1000);
    lcd.update();
    delay(1000);

    // force to fully flush all changes
    lcd.updateAllNow();
    delay(1000);

    lcd.updateDurationInMs = 10; // restore default

    lcd.clear();
    lcd.printStrAtLine( 0, "hello" );
    lcd.setCursor( 3, 1 );
    lcd.print( "there" );
    lcd.write( 0 );
}

int counter = 0;

void loop() {
    lcd.printfAt( 6, 0, "%6d", counter );
    lcd.printAt( 10, 1, counter );
    lcd.print( "     " );
    counter++;

    // call update() to trickle changes
    lcd.update();
}
