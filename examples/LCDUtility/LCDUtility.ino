//=====================================
//  StarterPack LCD Utility Example
//  2022-01 isc
//=====================================
//
//  LCD Utility
//  -----------
//  - prompt user to choose
//  - show long instructions/story
//  - blinking light to indicate mpu is alive
//
//  hardware: LCD Keypad Shield

#include <StarterPack.h> // include all
#include <LCDUtility.h>  // ... or only those used in this project
using namespace StarterPack;

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LCD_wired lcd( rs, en, d4, d5, d6, d7 );

AnalogIO aIO( A0 );

//
// SETUP
//

    Throttle tt( &backgroundProcess, 1000 );
    
    void setup() {
        Serial.begin( 115200 );
        while( !Serial );

        lcd.begin( 16, 2 );
        lcd.print( "hello" );
        lcd.printAt( 0, 1, "there" );

        // assign lcd to be used by LCDUtility
        UserInterface::assignLCD( lcd );

        // see AnalogIO Buttons Example
        // see UserInput Example
        aIO.initButtons( 1022, 834, 642, 14, 228, 430 );
        UserInterface::assignKeyHandler( aIO );
    }

//
// LOOP
//

    LCDUtility::blink imAlive( 15, 0 );

    int counter = 0;
    bool backgroundProcess() {
        lcd.printfAt( 0, 0, "%6d", counter++ );
        imAlive.update();
        // return false, to cancel choosing
        return true;
    }
    
    void loop() {

        lcd.clear();
        imAlive.setPosition( 15, 0 );

        const char *options[] = { "Yes", "No", "Maybe", "Definitely", "Sure", "Okay", "Never" };

        //
        // CHOOSE
        //
        uint8_t p = LCDUtility::choose( 
            1,                  // line 1 
            options,            // choices
            &backgroundProcess, // optional background process to run
            false               // cross from first/last entry
            );
        Serial.print( "choosen: " );
        Serial.println( p );

        //
        // STORY
        //
        const char *story[] = {
            //234567890123456
            "press up/down to", 
            "  scroll, okay", 
            "  to exit", 
            "once upon a time",
            "there was a yang",
            "princess and a",
            "ying prince",
            "haha..." };
        LCDUtility::showMultiLineMsg( story );

        //
        // BLINK
        //
        lcd.clear();
        imAlive.setPosition( 0, 1 );
        uint32_t start = millis();
        LCDUtility::blink dot2 = LCDUtility::blink( 3, 1 );
        while( true ) {
            if ( millis() - start >= 10000 )
                break;
            lcd.printfAt( 0, 0, "%6d", counter++ );
            imAlive.update();
            dot2.update();
        }
    }
    