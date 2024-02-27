#pragma once

// #include <Utility/cSupport.h>
#include <Utility/spNum.h>
#include <spWDT.h>

#include <UserInterface.h>
#include <LCD/LCDInterface.h>
#include <LCD/LCDBuffered.h>
#include <Utility/WindowedText.h>

#include <LCDSupport/LCDEditor.h>

namespace StarterPack {

//
// EDITOR
//

    class numericEditor {

        protected:

            numericEditorSettings *s = nullptr;
            char *buffer = nullptr;

        public:

            bool modified = false;
            WindowedText *wText = nullptr; // give access

            // insert vs overwrite character
            bool insertText = false;

            numericEditor( char *buffer, numericEditorSettings &s ) {
                this->buffer = buffer;
                this->s = &s;
                wText = new WindowedText( buffer, s.bufferLength, s.windowSize,
                    s.cursorRangeOptions );
            }

            ~numericEditor() {
                delete wText;
            }

            void reset() {
                wText->reset();
            }

            uint8_t prompt() {
                namespace ui = StarterPack::UserInterface;

                // returns:
                //    kENTER  - enter pressed, accepted
                //    kESCAPE - escape pressed, cancelled
                //    <key>   - unknown key detected
                // automatically handles:
                //    kBACKSPACE, kDELETE
                //    kLEFT, kRIGHT
                //    kDECIMAL
                //    kMINUS
                //    '0' .. '9'

                // scenario:
                // - user does not need decimal entry
                // - decimal key is reused for another purpose
                //   while editing
                //   ex. goto current value, or check validity
                // - but if detected here and ignored
                //   caller will not get key press
                //
                // solution 1:
                // - map ui::kDECIMAL to dummy value
                // - call this function, eg. prompt()
                // - decimal will not get detected
                //   hence passed to caller
                // - restore
                // - too much code to reuse keys
                //
                // solution 2:
                // - pass all keys to caller if not handled
                // - if not reused, simply ignored by caller
                //
                // so, pass key to caller if:
                // - unknown key
                // - decimal key and not allowDecimal
                // - minus key and not allowNegative

                if ( !ui::hasScreen() ) return ui::kESCAPE;
                LCDInterface *lcd = ui::LCD;
                LCDBuffered *lcdBuffered = nullptr;
                if ( lcd->isBuffered() )
                    lcdBuffered = (LCDBuffered*) lcd;

                bool updateDisplay = true;

                ui::waitUntilNothingIsPressed();
                lcd->cursorBlinkOn();

                // position 0 = '-', '.', 0..9
                CharCycle cycleNegative( CharCycle::CharSet::Numbers );
                if ( s->allowNegative && s->allowDecimal )
                    cycleNegative.addPrefixes( "-." );
                else if ( s->allowNegative )
                    cycleNegative.addPrefixes( "-" );
                else if ( s->allowDecimal )
                    cycleNegative.addPrefixes( "." );
                CharCycle cycleNumberAndDecimal(CharCycle::CharSet::Numbers | CharCycle::CharSet::Space);
                if ( s->allowDecimal )
                    cycleNumberAndDecimal.addSymbols( "." );
                CharCycle cycleNumber(CharCycle::CharSet::Numbers | CharCycle::CharSet::Space);

                while( true ) {

                    feedTheDog();
                    // delay( 10 );

                    //
                    // UPDATE DISPLAY
                    //
                    if ( updateDisplay ) {
                        updateDisplay = false;

                        lcd->cursorOff();
                        lcd->setCursor( s->col, s->row );
                        lcd->printStrN( wText->stringToDisplay(), s->windowSize, true );

                        if ( wText->isLeftObscured() )
                            lcd->writeAt( s->col, s->row, 0x7F );
                        
                        if ( lcdBuffered != nullptr )
                            lcdBuffered->setVirtualCursor( s->col + wText->cursorPositionOnWindow(), s->row );
                        else
                            lcd->setCursor( s->col+wText->cursorPositionOnWindow(), s->row );
                        lcd->cursorOn();
                    }
                    lcd->refreshPartial();
                    // if ( lcdBuffered != nullptr )
                    //     lcdBuffered->update();

                    //
                    // KEYSTROKES
                    //

                    uint8_t key = ui::getRepeatingKey();

                    if ( key == ui::kENTER || key == ui::kESCAPE ) {
                        lcd->cursorOff();
                        // ui::flagWaitForKeyup();
                        ui::skipRepeatingCurrentKey();
                        // ui::waitUntilNothingIsPressed();
                        return key;
                    }
                    
                    char charToAdd = 0;

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
                    } else if ( key == ui::kLEFT && globalEditorSettings.useLeftRightArrowKeys ) {
                        if ( wText->cursorBackward() ) {
                            // modified = true;
                            updateDisplay = true;
                        }
                    } else if ( key == ui::kRIGHT && globalEditorSettings.useLeftRightArrowKeys ) {
                        if ( wText->cursorForward() ) {
                            modified = true;
                            updateDisplay = true;
                        }
                    } else if ( key == ui::kUP && globalEditorSettings.cycleUpDown ) {
                        if ( wText->modifyCharAtCursor( ' ' ) ) {
                            if ( wText->getCursorNumericPosition() == 0 )
                                cycleNegative.cycleOneUp( wText->charPtrAtCursor(), wText->getCursorPosition() );
                            else if ( !wText->findCharacterBeforeCursor( s->decimalPoint ) )
                                cycleNumberAndDecimal.cycleOneDown( wText->charPtrAtCursor(), wText->getCursorPosition() );
                            else
                                cycleNumber.cycleOneUp( wText->charPtrAtCursor(), wText->getCursorPosition() );
                            modified = true;
                            updateDisplay = true;
                        }
                    } else if ( key == ui::kDOWN && globalEditorSettings.cycleUpDown ) {
                        if ( wText->modifyCharAtCursor( ' ' ) ) {
                            if ( wText->getCursorNumericPosition() == 0 )
                                cycleNegative.cycleOneDown( wText->charPtrAtCursor(), wText->getCursorPosition() );
                            else if ( !wText->findCharacterBeforeCursor( s->decimalPoint ) )
                                cycleNumberAndDecimal.cycleOneDown( wText->charPtrAtCursor(), wText->getCursorPosition() );
                            else
                                cycleNumber.cycleOneDown( wText->charPtrAtCursor(), wText->getCursorPosition() );                            
                            modified = true;
                            updateDisplay = true;
                        }

                    } else if ( key == ui::kDECIMAL && s->allowDecimal ) {

                        if ( !Str::findCharacter(s->decimalPoint,buffer) ) {
                            charToAdd = s->decimalPoint; // '.';

                            // CASE (1): INSERT addCharacter() CONTENTS HERE
                            // eg. if (insertText) { if ( wText->canAddChar() ) { ... } }
                            // RAM:   [====      ]  41.7% (used 854 bytes from 2048 bytes)
                            // Flash: [====      ]  40.8% (used 12532 bytes from 30720 bytes)

                            // CASE (2): CALL addCharacter(), ADDS ALMOST 100 BYTES ???
                            // RAM:   [====      ]  41.7% (used 854 bytes from 2048 bytes)
                            // Flash: [====      ]  41.1% (used 12620 bytes from 30720 bytes)
                            // if ( addCharacter( '.' ) ) {
                            //     modified = true;
                            //     updateDisplay = true;
                            // }

                            // CASE (3): ADD ROUTINES AFTER IFs
                            // RAM:   [====      ]  41.7% (used 854 bytes from 2048 bytes)
                            // Flash: [====      ]  40.6% (used 12482 bytes from 30720 bytes)

                        }
                        ui::skipRepeatingCurrentKey();
                        // ui::flagWaitForKeyup();

                    } else if ( key == ui::kMINUS && s->allowNegative ) {

                        char *ptr = wText->charPtrAt( 0 );
                        if ( ptr != nullptr ) {
                            if ( *ptr == '-' ) {
                                if ( wText->deleteAt( 0 ) ) {
                                    modified = true;
                                    updateDisplay = true;
                                }
                            } else {
                                if (wText->canAddChar()) {
                                    if ( wText->insertAt( '-', 0 ) ) {
                                        modified = true;
                                        updateDisplay = true;
                                    }
                                }
                            }
                        }
                        ui::skipRepeatingCurrentKey();
                        // ui::flagWaitForKeyup();
                        // ui::skipRepeatingCurrentKey();
                        
                        // updateDisplay = true;
                    } else if ( key >= '0' && key <= '9' ) {
                        charToAdd = key;

                        // CASE (1): INSERT addCharacter() CONTENTS HERE
                        // eg. if (insertText) { if ( wText->canAddChar() ) { ... } }
                        // RAM:   [====      ]  41.7% (used 854 bytes from 2048 bytes)
                        // Flash: [====      ]  40.8% (used 12532 bytes from 30720 bytes)

                        // CASE (2): CALL addCharacter(), ADDS ALMOST 100 BYTES ???
                        // RAM:   [====      ]  41.7% (used 854 bytes from 2048 bytes)
                        // Flash: [====      ]  41.1% (used 12620 bytes from 30720 bytes)
                        // if ( addCharacter( '.' ) ) {
                        //     modified = true;
                        //     updateDisplay = true;
                        // }

                        // CASE (3): ADD ROUTINES AFTER IFs
                        // RAM:   [====      ]  41.7% (used 854 bytes from 2048 bytes)
                        // Flash: [====      ]  40.6% (used 12482 bytes from 30720 bytes)

                    } else if ( key == ui::kQUESTION ) {

                        // pass back to user for now
                        return key;

                        /*
                        // specific to cutter app
                        lcd->cursorOff();
                        lcd->printStrAtRow( 0, "use \x7F \x7E to highlight" );
                        lcd->printStrAtRow( 1, "  cut position" );
                        lcd->printStrAtRow( 2, "Ok - edit" );
                        lcd->printStrAtRow( 3, "X  - go back" );
                        lcd->refresh();
                        // lcd->displayAll();
                        if ( ui::waitForAnyKeyPressed() == ui::kQUESTION ) {
                            lcd->printStrAtRow( 0, ". or \x03 - insert" );
                            lcd->printStrAtRow( 1, "  cut on cursor" );
                            lcd->printStrAtRow( 2, "BS or \x04 - delete" );
                            lcd->printStrAtRow( 3, "  cut on cursor" );
                            lcd->refresh();
                            // lcd->displayAll();
                            ui::waitForAnyKeyPressed();
                        }
                        lcd->clearRow( 2 );
                        lcd->clearRow( 3 );
                        ui::waitUntilNothingIsPressed();
                        updateDisplay = true;
                        */

                    } else {
                        // unknown key, pass back for caller to handle
                        // possibly includes kDECIMAL, kMINUS if not allowed
                        return key;
                    }

                    // CASE (3-A): ADD ROUTINES AFTER IFs
                    // RAM:   [====      ]  41.7% (used 854 bytes from 2048 bytes)
                    // Flash: [====      ]  40.6% (used 12482 bytes from 30720 bytes)
                    if (charToAdd != 0) {
                        if (insertText) {
                            if ( wText->canAddChar() ) {
                                if ( wText->insertAtCursor( charToAdd ) ) {
                                    wText->cursorForward();
                                    modified = true;
                                    updateDisplay = true;
                                }
                            }
                        } else {
                            if ( wText->canModifyCharAtCursor() ) {
                                if ( wText->setCharAtCursor( charToAdd ) ) {
                                    wText->cursorForward();
                                    modified = true;
                                    updateDisplay = true;
                                }
                            }
                        }
                    }
                }
            }

        protected:

            bool addCharacter( char ch ) {
                if (insertText) {
                    if ( wText->canAddChar() ) {
                        if ( wText->insertAtCursor( ch ) ) {
                            wText->cursorForward();
                            return true;
                        }
                    }
                } else {
                    if ( wText->canModifyCharAtCursor() ) {
                        if ( wText->setCharAtCursor( ch ) ) {
                            wText->cursorForward();
                            return true;
                        }
                    }
                }
                return false;
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

        numericEditorSettings s;
        s.setWindow( col, row, windowSize );
        s.allowDecimal = false;
        s.allowNegative = NEG;
        s.bufferLength = BUFFERSIZE;

        char buffer[BUFFERSIZE];
        if ( value == 0 )
            buffer[0] = 0;
        else
            Num::mySnprintf(buffer, BUFFERSIZE, value);
            // snprintf( buffer, BUFFERSIZE, printfToken, value );
            // snprintf( buffer, BUFFERSIZE, "%d", value );

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
                // int64_t tmp = Num::myAtoll( buffer );
                int64_t tmp;
                Num::StrToNum( buffer, tmp );
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

        numericEditorSettings s;
        s.setWindow( 1, 1, 10 );
        // s.setUnit( 11, 1 );
        s.bufferLength = 5;
        s.allowNegative = true;
        s.allowDecimal = true;

        char buffer[21] = "";
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
