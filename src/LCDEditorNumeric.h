#pragma once
#include <UserInterface.h>
#include <LCDInterface.h>
#include <LCDBuffered.h>
#include <LCDEditor.h>
#include <WindowedText.h>

namespace StarterPack {

//
// EDITOR
//

    class numericEditor {

        protected:

            editorSettings *s = nullptr;
            char *buffer = nullptr;

        public:

            bool modified = false;
            WindowedText *wText = nullptr; // give access

            numericEditor( char *buffer, editorSettings &s ) {
                this->buffer = buffer;
                this->s = &s;
                wText = new WindowedText( buffer, s.bufferLength, s.windowSize, WindowedText::cursorRangeOptions::EndOfDataPlusOne );
            }

            ~numericEditor() {
                delete wText;
            }

            void reset() {
                wText->reset();
            }

            // uint8_t editNumeric( char *buffer, editorSettings &s ) {
            uint8_t prompt() {
                namespace ui = StarterPack::UserInterface;

                // returns:
                //    kENTER  - enter pressed, accepted
                //    kESCAPE - escape pressed, cancelled
                //    <key>   - unknown key detected
                // automatically handles:
                //    kDECIMAL
                //    kMINUS
                //    '0' .. '9'

                if ( !ui::hasScreen() ) return ui::kESCAPE;
                LCDInterface *lcd = ui::LCD;
                LCDBuffered *lcdBuffered = nullptr;
                if ( lcd->isBuffered() )
                    lcdBuffered = (LCDBuffered*) lcd;

                bool updateDisplay = true;

                ui::waitUntilNothingIsPressed();
                lcd->cursorBlinkOn();

                while( true ) {

                    if ( updateDisplay ) {
                        updateDisplay = false;

                        lcd->cursorOff();
                        lcd->setCursor( s->col, s->row );
                        lcd->printStrN( wText->stringToDisplay(), s->windowSize, true );

                        if (wText->isLeftObscured() )
                            lcd->writeAt( s->col, s->row, 0x7F );
                        
                        if ( lcdBuffered != nullptr )
                            lcdBuffered->setVirtualCursor( s->col+wText->cursorPositionOnWindow(), s->row );
                        else
                            lcd->setCursor( s->col+wText->cursorPositionOnWindow(), s->row );
                        lcd->cursorOn();
                    }
                    if ( lcdBuffered != nullptr )
                        lcdBuffered->update();

                    uint8_t key = ui::getRepeatingKey();

                    if ( key == ui::kENTER || key == ui::kESCAPE ) {
                        lcd->cursorOff();
                        // ui::flagWaitForKeyup();
                        ui::waitUntilNothingIsPressed();
                        return key;
                    }
                    
                    // scanario:
                    // - user does not need decimal entry
                    // - decimal key is reused for another purpose
                    //   while editing
                    //   ex. goto current value, or check validity
                    // - but if detected here and ignored here
                    //   caller will not get key press
                    //
                    // solution 1:
                    // - map ui::kDECIMAL to dummy value
                    // - call this function
                    // - decimal will not get detected
                    //   hence passed to caller
                    // - restore
                    // - too much code to reuse keys
                    //
                    // solution 2:
                    // - pass all keys to caller if not handled
                    // - if not reused, simply ignore by caller
                    //
                    // so, pass key to caller if:
                    // - unknown key
                    // - decimal key and not allowedDecimal
                    // - minus key and not allowedNegative

                    if ( key == ui::kNONE ) {
                        // ...
                    } else if ( key == ui::kBACKSPACE ) {
                        if ( wText->backspace() ) {
                            modified = true;
                            updateDisplay = true;
                        }
                    } else if ( key == ui::kDELETE ) {
                        if ( wText->deleteAtCursor() ) {
                            modified = true;
                            updateDisplay = true;
                        }
                    } else if ( key == ui::kDECIMAL && s->allowDecimal ) {                
                        if ( !isCharInString(s->decimalPoint,buffer) ) {
                            if ( wText->insertAtCursor('.') ) {
                                modified = true;
                                updateDisplay = true;
                            }
                        }
                        ui::flagWaitForKeyup();
                    } else if ( key == ui::kMINUS && s->allowNegative ) {
                        char *ptr = wText->charPtrAt( 0 );
                        if ( ptr != nullptr ) {
                            if ( *ptr == '-' )
                                wText->deleteAt( 0 );
                            else
                                wText->insertAt( '-', 0 );
                        }
                        ui::flagWaitForKeyup();
                        modified = true;
                        updateDisplay = true;
                    } else if ( key >= '0' && key <= '9' ) {
                        if ( wText->insertAtCursor( key ) ) {
                            modified = true;
                            updateDisplay = true;
                        }
                    // } else if ( key == ui::kQUESTION ) {
                    //     lcd->cursorOff();
                    //     lcd->printStrAtRow( 0, "use \x7F \x7E to highlight" );
                    //     lcd->printStrAtRow( 1, "  cut position" );
                    //     lcd->printStrAtRow( 2, "Ok - edit" );
                    //     lcd->printStrAtRow( 3, "X  - go back" );
                    //     lcd->displayAll();
                    //     if ( ui::waitForAnyKey() == ui::kQUESTION ) {
                    //         lcd->printStrAtRow( 0, ". or \x03 - insert" );
                    //         lcd->printStrAtRow( 1, "  cut on cursor" );
                    //         lcd->printStrAtRow( 2, "BS or \x04 - delete" );
                    //         lcd->printStrAtRow( 3, "  cut on cursor" );
                    //         lcd->displayAll();
                    //         ui::waitForAnyKey();
                    //     }
                    //     lcd->clearRow( 2 );
                    //     lcd->clearRow( 3 );
                    //     ui::waitUntilNothingIsPressed();
                    //     updateDisplay = true;
                    } else {
                        // unknown key, pass back for caller to handle
                        // possible include kDECIMAL, kMINUS if not handled
                        return key;
                    }
                }
            }

    };

//
// SHORTCUTS
//

    template <typename T, size_t BUFFERSIZE, bool NEG>
    bool promptForCore( uint8_t headerRow, const char *caption,
    uint8_t col, uint8_t row, uint8_t windowSize, T &value, T min, T max ) {
        namespace ui = StarterPack::UserInterface;

        if ( !ui::hasScreen() ) return false;
        LCDBuffered *bufferedLCD = nullptr;
        if ( ui::LCD->isBuffered() )
            bufferedLCD = (LCDBuffered*) ui::LCD;

        editorSettings s;
        s.setPosition( col, row, windowSize );
        s.allowDecimal = false;
        s.allowNegative = NEG;
        s.bufferLength = BUFFERSIZE;

        char buffer[BUFFERSIZE];
        if ( value == 0 )
            buffer[0] = 0;
        else
            snprintf( buffer, BUFFERSIZE, "%d", value );

        numericEditor nEdit( buffer, s );

        ui::LCD->printStrAtRow( headerRow, caption );
        // ui::LCD->displayAll();
        int8_t freeSpaceOnHeader = ui::LCD->maxColumns - strlen( caption ) - 1;
        if ( freeSpaceOnHeader < 0 ) freeSpaceOnHeader = 0;

        // Serial.println( value );
        // Serial.print( buffer );
        // Serial.println( "<<<" );
        // Serial.println( freeSpaceOnHeader );

        char origCursor = 0xFF;
        if ( bufferedLCD != nullptr )
            origCursor = bufferedLCD->virtualCursorChar;

        while( true ) {
            uint8_t key = nEdit.prompt();
            if ( key == ui::kESCAPE ) {
                // restore original cursor
                if ( bufferedLCD != nullptr )
                    bufferedLCD->virtualCursorChar = origCursor;
                return false;
            }
            if ( key == ui::kENTER ) {
                int64_t tmp = atoll( buffer );
                if ( tmp < min ) {
                    if ( bufferedLCD != nullptr )
                        bufferedLCD->virtualCursorChar = 'v';
                    if ( freeSpaceOnHeader > 0 ) {
                        ui::LCD->setCursor( strlen( caption ) + 1, headerRow );
                        ui::LCD->printStrN( "(low) ", freeSpaceOnHeader );
                    }
                } else if ( tmp > max ) {
                    if ( bufferedLCD != nullptr )
                        bufferedLCD->virtualCursorChar = '^';
                    if ( freeSpaceOnHeader > 0 ) {
                        ui::LCD->setCursor( strlen( caption ) + 1, headerRow );
                        ui::LCD->printStrN( "(over)", freeSpaceOnHeader );
                    }
                } else {
                    // restore original cursor
                    if ( bufferedLCD != nullptr )
                        bufferedLCD->virtualCursorChar = origCursor;
                    value = tmp;
                    return true;
                }
            }
        }
    }

    #define PROMPT(name,dType,LEN,NEG,MIN,MAX) \
        bool promptFor ## name( uint8_t headerRow, const char *caption, \
        uint8_t col, uint8_t row, uint8_t windowSize, dType &value, dType min=MIN, dType max=MAX ) { \
            return promptForCore<dType,LEN,NEG>( headerRow, caption, col, row, windowSize, value, min, max ); \
        }
    PROMPT( Int8,   int8_t,   4+1 , true ,  INT8_MIN, INT8_MAX   )  //        -127
    PROMPT( UInt8,  uint8_t,  3+1 , false,         0, UINT8_MAX  )  //         255
    PROMPT( Int16,  int16_t,  6+1 , true , INT16_MIN, INT16_MAX  )  //      -32768
    PROMPT( UInt16, uint16_t, 5+1 , false,         0, UINT16_MAX )  //       65535
    PROMPT( Int32,  int32_t,  11+1, true , INT32_MIN, INT32_MAX  )  // -2147483647
    PROMPT( UInt32, uint32_t, 10+1, false,         0, UINT32_MAX )  //  4294967295
    #undef PROMPT

//
// TEST
//

    void TEST_editorNumeric() {
        namespace ui = StarterPack::UserInterface;

        editorSettings s;
        s.setPosition( 1, 1, 10 );
        // s.setUnit( 11, 1 );
        s.bufferLength = 5;
        s.allowNegative = true;
        s.allowDecimal = true;

        char buffer[21];
        strcpy( buffer, "123" );

        // set values and check if wrong buffer editing
        buffer[s.bufferLength-1] = 100; // will be null entered chars till end
        buffer[s.bufferLength  ] = 123; // must not be overwritten

        ui::LCD->printAt(  0, 1, "[" );
        ui::LCD->printAt( 11, 1, "]" );

        numericEditor nEdit( buffer, s );
        while( nEdit.prompt() != ui::kESCAPE );
        // while( editNumeric( buffer, s ) != ui::kESCAPE );

        Serial.println( (uint8_t) buffer[s.bufferLength-1] );
        Serial.println( (uint8_t) buffer[s.bufferLength  ] );
    }

    void TEST_shortcuts() {
        namespace ui = StarterPack::UserInterface;

        uint8_t ui08 = 200;
        promptForUInt8( 0, "hello", 0, 1, 5, ui08, 50, 100 );

        ui08 = 200;
        promptForUInt8( 0, "hello there how r", 0, 1, 5, ui08, 50, 100 );

        int8_t si08 = -100;
        // 01234567890123456789
        // hello there how are
        promptForInt8( 0, "hello there how are", 0, 1, 5, si08, -50, 100 );
    }

}
