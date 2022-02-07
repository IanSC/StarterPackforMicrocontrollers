//=================================
//  StarterPack Wired LCD Example
//  2022-01 isc
//=================================
//
//  LCD Wired
//  ---------
//  - wrapper to LCD library <LiquidCrystal.h>
//    same interface for: wired, i2c and buffered
//  - for wired LCD (not i2c)
//    eg. LCD Keypad Shield
//  - additional helper functions

#include <StarterPack.h>       // include all
#include <LCD_wired.h>         // ... or only those used in this project
using namespace StarterPack;

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LCD_wired lcd( rs, en, d4, d5, d6, d7 );

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

    lcd.printStrAtRow( 0, "hello" );
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
}
