//===========================================
//  StarterPack LCD Buffered Cursor Example
//  2022-01 isc
//===========================================
//
//  LCD Buffered Virtual Cursor
//  ---------------------------
//  demonstrate virtual cursor functionality
//  if tested on LCD Keypad Shield
//     use Left, Right, Up, Down to move virtual cursor
//     use Select to change

#include <StarterPack.h>       // include all
#include <LCDBuffered_wired.h> // ... or only those used in this project
#include <LCDBuffered_i2c.h>

using namespace StarterPack;

// choose wired or i2c
#define LCD_Wired // use wired version
//#define LCD_I2C // use i2c version

    #if defined( LCD_Wired )
            
        const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
        LCDBuffered_wired lcd( rs, en, d4, d5, d6, d7 );
            
    #elif defined( LCD_I2C )
    
        LCDBuffered_i2c lcd;
        
    #endif

AnalogIO aIO( A0 );

//
// SETUP
//

uint32_t lastUpdate = millis();
int8_t col = 0, row = 0;
int counter = 0;

void setup() {
    Serial.begin( 115200 );
    while( !Serial );

    #if defined( LCD_I2C )

        Wire.begin();
        lcd.setTimeoutInMs( 100 );
        lcd.setFrequency( 800000 );
                        
    #endif

    lcd.begin( 16, 2 );

    lcd.cursorOn();

    // if printing over the end of the line, move to start of next line
    lcd.cursorAutoCarriageReturn = true;

    // if printing over end of screen, move to start of screen
    lcd.cursorAutoJumpToStart = true;

    // in case no keypad
    while( true ) {
        uint32_t now = millis();
        if ( now - lastUpdate > 2000 ) {
            lastUpdate = now;
            col++;
            lcd.setVirtualCursor( col, 0 );
            if ( col == 2 ) lcd.cursorBlinkOn();
            if ( col == 4 ) break;
        }
        lcd.print( counter++ );
        lcd.update();
    }
    lcd.cursorBlinkOff();

    // lcd.setUpdateThrottleInMs( 100 );

    // see other examples regarding this
    aIO.initButtons( 1022, 834, 642, 14, 228, 430 );
}

//
// LOOP
//

int8_t mode = 0;

void loop() {

    lcd.print( counter++ );

    byte buttonNo = aIO.getRepeatingKey();
    
    switch( buttonNo ) {
    case 1:
        mode++;
        switch( mode ) {
        case 0:
            lcd.cursorBlinkOff();
            lcd.virtualCursorChar = 0xFF;
            break;
        case 1:
            lcd.cursorBlinkOn();
            break;
        case 2:
            lcd.cursorBlinkOff();
            lcd.virtualCursorChar = '#';
            break;
        case 3:
            lcd.cursorBlinkOn();
            mode = -1; // cycle
            break;
        }
        break;
    case 2:
        col--; if ( col < 0 ) col = lcd.maxColumns-1;
        lcd.setVirtualCursor( col, row );
        break;
    case 3:
        col++; if ( col >= lcd.maxColumns ) col = 0;
        lcd.setVirtualCursor( col, row );
        break;
    case 4:
        row++; if ( row >= lcd.maxRows ) row = 0;
        lcd.setVirtualCursor( col, row );
        break;
    case 5:
        row--; if ( row < 0 ) row = lcd.maxRows-1;
        lcd.setVirtualCursor( col, row );
        break;
    }
    lcd.update();
}
