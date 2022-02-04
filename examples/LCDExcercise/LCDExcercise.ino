//==============================
//  StarterPack LCD Excercise
//  2022-01 isc
//==============================
//
//  test most functionalities of LCD

#include <StarterPack.h>       // include all
#include <LCD_wired.h>         // ... or only those used in this project
#include <LCDBuffered_wired.h>
#include <LCD_i2c.h>
#include <LCDBuffered_i2c.h>

using namespace StarterPack;

//
// CHOOSE WIRED OR I2C
//
// #define LCD_Wired // use wired version
#define LCD_I2C   // use i2c version

    #if defined( LCD_Wired )
    
        const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
        LCD_wired lcd( rs, en, d4, d5, d6, d7 );
                        
    #elif defined( LCD_I2C )
    
        LCD_i2c lcd;

    #endif

//
// EXCERCISE
//

    void lcdExcercise() {
        
        // commands that moves the origin of the cursor
        // eg. cursor (0,0) is no longer the on (0,0) of physical screen
        //    scrollDisplayLeft()
        //    scrollDisplayRight()
        //    autoscroll()
        //
        // home()  - reset cursor origin, move cursor to (0,0)
        // clear() - home() then clear screen data

        uint16_t stepDelay = 3000;
        uint16_t testDelay = 5000;

        bool T_specialChars = false;
        bool T_display = false;
        bool T_leftToRight = false;
        bool T_autoscroll = false;
        bool T_autoRightToLeft1 = false;
        bool T_autoRightToLeft2 = false;
        bool T_shiftCursor = false;
        bool T_shiftTest = false;
        
        T_specialChars     = true;
        T_display          = true;
        T_leftToRight      = true;
        T_autoscroll       = true;
        T_autoRightToLeft1 = true;
        T_autoRightToLeft2 = true;
        T_shiftCursor      = true;
        T_shiftTest        = true;
        
        //
        // SPECIAL CHARACTERS
        //
        if ( T_specialChars ) {
            lcd.clear(); lcd.home(); lcd.cursor(); lcd.blink();
            lcd.noAutoscroll();
            lcd.leftToRight();

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
            lcd.createChar( 0, heartBitmap );
            lcd.createChar( 1, "\037\033\035\040\035\033\037\037" );
            // cursor has moved

            lcd.setCursor( 0, 0 );
            lcd.print( "hello" );
            lcd.write( 0 );
            lcd.write( 1 );
            delay(testDelay);
        }

        //
        // DISPLAY
        //
        if ( T_display ) {
            lcd.clear(); lcd.home(); lcd.cursor(); lcd.blink(); //lcd.setCursor(0,0);

            lcd.noAutoscroll();
            lcd.leftToRight();

            lcd.print( "hello" );
            //    0123456789012345
            //   +----------------+
            // 0 |hello_          |
            // 1 |                |
            //   +----------------+
            delay(stepDelay);
            
            // display OFF / ON
            lcd.displayOff();
            delay(stepDelay);
            lcd.cursorBlinkOn(); // change cursor
            lcd.displayOn();
            delay(stepDelay);

            // backlight OFF / ON
            lcd.backlightOff();
            lcd.setCursor(0,1);
            lcd.print( "peek-a-boo" );
            delay(stepDelay);
            lcd.backlightOn();
            delay(stepDelay);

            lcd.cursorOff();
            lcd.cursorBlinkOff();
            lcd.setCursor(0,1);
            lcd.cursorOn();
            //    0123456789012345
            //   +----------------+
            // 0 |hello_          |
            // 1 |_               |
            //   +----------------+
            delay(testDelay);
        }
        
        //
        // RIGHT-TO-LEFT
        //
        if ( T_leftToRight ) {
            lcd.clear(); lcd.home(); lcd.cursor(); lcd.blink(); //lcd.setCursor(0,0);

            lcd.noAutoscroll();
            lcd.rightToLeft();

            lcd.setCursor(7,0);
            lcd.print("hello");
            //    X C
            //    0123456789012345
            //   +----------------+
            // 0 |  _olleh        |
            // 1 |                |
            //   +----------------+
            delay(stepDelay);

            lcd.print("123");
            //   CX
            //    0123456789012345
            //   +----------------+
            // 0 _321olleh        |
            // 1 |                |
            //   +----------------+
            delay(stepDelay);

            lcd.setCursor(7,1);
            lcd.print("456");
            //    X   C
            //    0123456789012345
            //   +----------------+
            // 0 |321olleh        |
            // 1 |    _654        |
            //   +----------------+
            delay(testDelay);
        }

        //
        // AUTOSCROLL
        //
        if ( T_autoscroll ) {
            lcd.clear(); lcd.home(); lcd.cursor(); lcd.blink(); //lcd.setCursor(0,0);

            lcd.autoscroll();
            lcd.leftToRight();

            lcd.setCursor(15,0);
            //    X              C
            //    0123456789012345
            //   +----------------+
            // 0 |               _|
            // 1 |                |
            //   +----------------+
            delay(stepDelay);
            
            lcd.print( "hello" );
            //    X=0 (origin moved)
            //    543210123456789012345
            //        +----------------+
            // 0      |          hello_|
            // 1      |                |
            //        +----------------+
            delay(stepDelay);

            lcd.setCursor(15,1);
            //    X              C
            //    543210123456789012345
            //        +----------------+
            // 0      |          hello |
            // 1      |          _     |
            //        +----------------+
            delay(stepDelay);
            
            lcd.print( "there" );
            //    X                   C
            //    09876543210123456789012345
            //             +----------------+
            // 0           |     hello      |
            // 1           |     there_     |
            //             +----------------+
            delay(stepDelay);

            lcd.setCursor(11,0);
            //    X          C
            //    ----------++++++++++++++++
            //    09876543210123456789012345
            //             +----------------+
            // 0           | _   hello      |
            // 1           |     there      |
            //             +----------------+
            delay(stepDelay);
            
            lcd.print( "X" );
            //    X           C
            //    109876543210123456789012345
            //              +----------------+
            // 0            |X_  hello       |
            // 1            |    there       |
            //              +----------------+
            delay(testDelay);
        }

        //
        // AUTOSCROLL / RIGHT-TO-LEFT [1]
        //
        if ( T_autoRightToLeft1 ) {
            lcd.clear(); lcd.home(); lcd.cursor(); lcd.blink(); //lcd.setCursor(0,0);

            lcd.autoscroll();
            lcd.rightToLeft();

            lcd.setCursor(13,0);
            //    0123456789012345
            //   +----------------+
            // 0 |             _  |
            // 1 |                |
            //   +----------------+
            delay(stepDelay);
            
            lcd.print("h"); // printed at 13, but scrolled right
            //     X=0 (origin moved)
            //    0123456789012345
            //   +----------------+
            // 0 |             _h |
            // 1 |                |
            //   +----------------+
            delay(stepDelay);

            lcd.setCursor(0,0);
            //     X
            //    0123456789012345
            //   +----------------+
            // 0 | _           _h |
            // 1 |                |
            //   +----------------+
            delay(stepDelay);
            
            lcd.print("H");
            //      X
            //    0123456789012345
            //   +----------------+
            // 0 |  H            h|
            // 1 | _              | why cursor moved down?
            //   +----------------+
            delay(stepDelay);

            lcd.print("I");
            //     C X
            //    0123456789012345
            //   +----------------+
            // 0 |   H            h
            // 1 | _I             |
            //   +----------------+
            delay(stepDelay);

            lcd.home(); // reset cursor origin to (0,0)
            //    X=0 (origin reset)
            //    C
            //    0123456789012345
            //   +----------------+
            //    -
            // 0 |H            h  |
            // 1 I                |
            //   +----------------+
            delay(stepDelay);

            lcd.print( "O" );
            //    CX=0 (origin moved)
            //    0123456789012345
            //   +----------------+
            // 0 | O            h |
            // 1 |I               |
            //    -
            //   +----------------+
            delay(testDelay);
        }

        //
        // AUTOSCROLL / RIGHT-TO-LEFT [2]
        //
        if ( T_autoRightToLeft2 ) {
            lcd.clear(); lcd.home(); lcd.cursor(); lcd.blink(); //lcd.setCursor(0,0);

            lcd.autoscroll();
            lcd.rightToLeft();

            lcd.setCursor(7,0);
            //    X      C
            //    0123456789012345
            //   +----------------+
            // 0 |       _        |
            // 1 |                |
            //   +----------------+
            delay(stepDelay);
            
            lcd.print("hi");
            //      X    C
            //    0123456789012345
            //   +----------------+
            // 0 |       _ih      |
            // 1 |                |
            //   +----------------+
            delay(stepDelay);

            lcd.print("123");
            //         X C
            //    0123456789012345
            //   +----------------+
            // 0 |       _321ih   |
            // 1 |                |
            //   +----------------+
            delay(stepDelay);

            lcd.setCursor(7,1); // get displayed below "hi"
            //         X      C
            //    0123456789012345
            //   +----------------+
            // 0 |        321ih   |
            // 1 |            _   |
            //   +----------------+
            delay(stepDelay);

            lcd.print("456");
            //            X   C
            //    0123456789012345
            //   +----------------+
            // 0 |           321ih|
            // 1 |            _654|
            //   +----------------+
            delay(testDelay);
        }

        //
        // CURSOR SHIFT
        //
        if ( T_shiftCursor ) {
            lcd.clear(); lcd.home(); lcd.cursor(); lcd.blink(); //lcd.setCursor(0,0);

            lcd.noAutoscroll();
            lcd.leftToRight();

            lcd.setCursor(1,0);
            //    XC
            //    0123456789012345
            //   +----------------+
            // 0 | _              |
            // 1 |                |
            //   +----------------+
            delay(stepDelay);
            
            lcd.moveCursorRight();
            //    X C
            //    0123456789012345
            //   +----------------+
            // 0 |  _             |
            // 1 |                |
            //   +----------------+
            delay(stepDelay);
            
            lcd.print("A");
            //    X  C
            //    0123456789012345
            //   +----------------+
            // 0 |  A_            |
            // 1 |                |
            //   +----------------+
            delay(stepDelay);
        
            lcd.moveCursorRight();
            //    X   C
            //    0123456789012345
            //   +----------------+
            // 0 |  A _           |
            // 1 |                |
            //   +----------------+
            delay(stepDelay);
            
            lcd.print("B");
            //    X    C
            //    0123456789012345
            //   +----------------+
            // 0 |  A B_          |
            // 1 |                |
            //   +----------------+
            delay(stepDelay);

            lcd.setCursor(2,1);
            //    X C
            //    0123456789012345
            //   +----------------+
            // 0 |  A B           |
            // 1 |  _             |
            //   +----------------+
            delay(stepDelay);
   
            lcd.print("C");
            //    X  C
            //    0123456789012345
            //   +----------------+
            // 0 |  B             |
            // 1 |  C_            |
            //   +----------------+
            delay(stepDelay);

            // conclusion:
            //    moveCursorLeft() / moveCursorRight()
            //    does not move cursor origin
            lcd.home();
            //    X
            //    0123456789012345
            //   +----------------+
            // 0 |_ A B           |
            // 1 |  C             |
            //   +----------------+
            delay(stepDelay);

            lcd.setCursor(7,0);
            lcd.moveCursorLeft();
            lcd.moveCursorLeft();
            lcd.print("D");
            //    X
            //    0123456789012345
            //   +----------------+
            // 0 |  A BD_         |
            // 1 |  C             |
            //   +----------------+
            
            delay(testDelay);
        }
        
        //
        // SCROLL
        //
        if ( T_shiftTest ) {
            lcd.clear(); lcd.home(); lcd.cursor(); lcd.blink(); //lcd.setCursor(0,0);

            lcd.noAutoscroll();
            lcd.leftToRight();

            //          0123456789012345678901234567890123456789
            lcd.setCursor(0,0);
            lcd.print( "abcdefghijklmnop-qrstuvwxyz1234567890@#$" );
            lcd.setCursor(0,1);
            lcd.print( "ABCDEFGHIJKLMNOP-QRSTUVWXYZ1234567890@#$ETHANETHAN" );
            delay(stepDelay);
            //    X
            //    0123456789012345678901234567890123456789
            //   +----------------+
            // 0 |abcdefghijklmnop|qrstuvwxyz1234567890@#$
            // 1 |ABCDEFGHIJKLMNOP|QRSTUVWXYZ1234567890@#$ ... ETHANETHAN
            //   +----------------+
            //   +----------------+
            // 0 |ETHANETHANklmnop|qrstuvwxyz1234567890@#$
            // 1 |ABCDEFGHIJKLMNOP|QRSTUVWXYZ1234567890@#$ ... ETHANETHAN
            //   +----------------+
            
            for( int i=0; i<30; i++ ) {
                delay(stepDelay/2);
                lcd.scrollDisplayLeft();
                lcd.setCursor(15,0);
                lcd.print(i);
                lcd.setCursor(15,1);
                lcd.print(i);
            }
            for( int i=0; i<30; i++ ) {
                delay(stepDelay/2);
                lcd.scrollDisplayRight();
            }
            delay(testDelay);
        }
    }

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
        lcdExcercise();
        lcd.clear();
        lcd.print( "hello" );
    }

//
// LOOP
//

    int counter = 0;
    
    void loop() {
        lcd.printfAt( 0, 1, "%6d", counter++ );
    }
