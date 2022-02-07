#ifndef LCD_SOS_H
#define LCD_SOS_H

class lcdSOSclass {
        
    public:
            
        lcdSOSclass( byte x, byte y, char ch ) { _x=x; _y=y; _ch=ch; }

        void setCharacter( char ch ) { _ch=ch; }
        void setPosition( byte x, byte y ) { _x=x; _y=y; }
        
        uint16_t blinkDuration = 1000;

        void start() {
            if ( blinkCount != 0 ) return;
            blinkCount = 3;
            blinkOn = false;
            lastAction = millis();
        }
        void stop() {
            lcd.setCursor( _x, _y );
            lcd.print( " " );
            blinkCount = 0;
        }

        void loop() {
            if ( blinkCount == 0 ) return;
            uint32_t now = millis();
            if ( now - lastAction < blinkDuration ) return;

            lcd.setCursor( _x, _y );
            if ( blinkOn ) {
                lcd.print( " " );
                blinkCount--;
            } else {
                lcd.print( _ch );
            }
            blinkOn = !blinkOn;
            lastAction = now;
        }

    private:

        byte _x; byte _y;
        char _ch;
        
        byte blinkCount = 0;
        bool blinkOn = false;
        uint32_t lastAction;

};

//lcdSOSclass lcdSOS = lcdSOSclass();

#endif
