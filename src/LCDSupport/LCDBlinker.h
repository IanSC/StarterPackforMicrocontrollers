#pragma once

#include <UserInterface.h>
#include <spWDT.h>

namespace StarterPack::spLCD {

    class charBlinker {

        //  usage:
        //      LCD_blink blinker(0,0,0xA5);
        //      void loop() {
        //          blinker.update();    
        //      }

        private:

            uint32_t blinkTime = millis();
            bool blinkMode = false;

            // LCDInterface *lcd;
            uint8_t col = 0;
            uint8_t row = 0;

        public:

            // 0xA5 - bullet Japanese version LCD, 0xCD - bullet Europe version LCD
            uint8_t symbol = '.'; // 0xA5 - bullet Japanese version LCD, 0xCD - bullet Europe version LCD
            uint16_t blinkRateMs = 1000;

            charBlinker() {}

            charBlinker( uint8_t col, uint8_t row, uint8_t symbol='.', uint16_t blinkRateMs=1000 ) {
                this->col = col;
                this->row = row;
                this->symbol = symbol;
            }

            void update() {
                namespace ui = StarterPack::UserInterface;
                if ( !ui::hasScreen() ) return;
                uint32_t now = millis();
                if ( now - blinkTime >= blinkRateMs ) {
                    ui::LCD->writeAt( col, row, blinkMode ? symbol : ' ' );
                    blinkMode = !blinkMode;
                    blinkTime = now;
                    // ui::LCD->displayAll();
                }
            }

            void setPosition( uint8_t col, uint8_t row ) {
                // hide previous, before moving
                namespace ui = StarterPack::UserInterface;
                if ( !ui::hasScreen() ) return;
                ui::LCD->writeAt( col, row, ' ' );
                blinkMode = true; // next is show
                // if ( !blinkMode ) {
                //     // false means dot is currently displayed, hide it
                //     ui::LCD->writeAt( col, row, ' ' );
                //     blinkMode = !blinkMode;
                //     // ui::LCD->displayAll();
                // }
                this->col = col;
                this->row = row;
            }

    };

//
// BLINK SOS
//
/*
    class blinkSOS {

            uint8_t col; uint8_t row;
            char ch;
            
            uint8_t counter = 0;
            bool blinkOn = false;
            uint32_t lastAction;

        public:

            uint8_t blinkCount = 3;
            uint16_t blinkDuration = 1000;
    
            blinkSOS( uint8_t col, uint8_t row, char ch, uint8_t blinkCount = 3 ) {
                this->col = col; this->row = row; this->ch = ch;
                this->blinkCount = blinkCount;
            }

            void setPosition( uint8_t col, uint8_t row ) { this->col = col; this->row = row; }
            void setCharacter( char ch ) { this->ch = ch; }
            
            void start() {
                if ( counter != 0 ) return;
                counter = blinkCount;
                blinkOn = false;
                lastAction = millis();
            }
            void stop() {
                if ( !ui::hasScreen() ) return;
                lcd.setCursor( col, row );
                lcd.print( ' ' );
                counter = 0;
            }

            void loop() {
                if ( counter == 0 ) return;
                uint32_t now = millis();
                if ( now - lastAction < blinkDuration ) return;

                lcd.setCursor( col, row );
                if ( blinkOn ) {
                    lcd.write( ' ' );
                    counter--;
                } else {
                    lcd.write( ch );
                }
                blinkOn = !blinkOn;
                lastAction = now;
            }

    };
*/

};
