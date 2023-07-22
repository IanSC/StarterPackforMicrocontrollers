#pragma once
#include <UserInterface.h>
#include <LCD/LCDInterface.h>
#include <LCD/LCDBuffered.h>
#include <LCDSupport/LCDEditor.h>
#include <Utility/WindowedText.h>

// #define DEBUG_TRACE(x)   x;
#define DEBUG_TRACE(x)   ;

namespace StarterPack {

    class alphanumericEditor {

        protected:

            bool upperCase = true;
            editorSettings *s = nullptr;
            char *buffer = nullptr;

        public:

            bool modified = false;
            WindowedText *wText = nullptr; // give access

            alphanumericEditor( char *buffer, editorSettings &s ) {
                this->buffer = buffer;
                this->s = &s;
                wText = new WindowedText( buffer, s.bufferLength, s.windowSize );
                // wText.cursorRange = LCDEditor::WindowedText::cursorRangeOptions::WithinBuffer;
            }

            ~alphanumericEditor() {
                delete wText;
            }

            void reset() {
                wText->reset();
            }

            uint8_t prompt() {
                namespace ui = StarterPack::UserInterface;

                if ( !ui::hasScreen() ) return ui::kESCAPE;
                LCDInterface *lcd = ui::LCD;
                LCDBuffered *lcdBuffered = nullptr;
                if ( lcd->isBuffered() )
                    lcdBuffered = (LCDBuffered*) lcd;

                bool updateDisplay = true;

                ui::waitUntilNothingIsPressed();
                lcd->cursorBlinkOn();
                // lcd->cursorOn();

                enum jumpMode {
                    none,
                    start,
                    upBig, downBig,
                    small
                };
                
                // end of string, ready to put new char
                jumpMode jMode = start;

                while( true ) {

                    //
                    // DISPLAY
                    //
                    if ( updateDisplay ) {
                        updateDisplay = false;

                        // cursor on/off will reset blinking
                        // it will appear to be permanently displayed
                        //    if keys are held down
                        lcd->cursorOff();

                        lcd->setCursor( s->col, s->row );
                        lcd->printStrN( buffer+wText->startPos, s->windowSize, true );
                        // lcd->printCharsN( ' ', wText->spaceToClear() );

                        if ( wText->isLeftObscured() )
                            lcd->writeAt( s->col, s->row, 0x7F );
                        if ( wText->isRightObscured() )
                            lcd->writeAt( s->col+s->windowSize-1, s->row, 0x7E );

                        if ( lcdBuffered != nullptr )
                            lcdBuffered->setVirtualCursor( s->col + wText->cursorPositionOnWindow(), s->row );
                        else
                            lcd->setCursor( s->col + wText->cursorPositionOnWindow(), s->row );
                        lcd->cursorOn();
                    }
                    if ( lcdBuffered != nullptr )
                        lcdBuffered->update();

                    //
                    // KEY
                    //
                    uint8_t key = ui::getRepeatingKey();

                    if ( key == ui::kENTER || key == ui::kESCAPE ) {
                        if ( modified ) {
                            // remove ' ' at the end
                            wText->trimEnd();
                        }
                        lcd->cursorOff();
                        //ui::flagWaitForKeyup();
                        ui::waitUntilNothingIsPressed();
                        return key;
                    }
                    
                    if ( key == ui::kNONE ) {

                        // ...

                    } else if ( key == ui::kLEFT ) {

                        if ( wText->cursorBackward() ) {
                            updateDisplay = true;

                            bool enableJumpingChars;
                            // enable if last character
                            enableJumpingChars = ( wText->evalPosition() == WindowedText::cPos::rightAfterLast );
                            // enable if blank
                            // enableJumpingChars = ( buffer[cursorPosition] == ' ' );
                            if ( enableJumpingChars )
                                jMode = jumpMode::start;
                            else
                                jMode = jumpMode::none;
                        }

                    } else if ( key == ui::kRIGHT ) {

                        // might add spaces at end,
                        // but don't consider as modified
                        if ( wText->cursorForward() ) {
                            updateDisplay = true;

                            bool enableJumpingChars;
                            // enable if last character
                            enableJumpingChars = ( wText->evalPosition() == WindowedText::cPos::rightAfterLast );
                            // enable if blank
                            // enableJumpingChars = ( buffer[cursorPosition] == ' ' );
                            if ( enableJumpingChars )
                                jMode = jumpMode::start;
                            else
                                jMode = jumpMode::none;
                        }

                    } else if ( key == ui::kUP ) {

                        DEBUG_TRACE( SerialPrintfln( "UP-1 = %d", jMode ) )
                        if ( wText->modifyCharAtCursorIfNull( ' ' ) ) {
                        // if ( wText->insertAtCursor( ' ' ) ) {
                            switch( jMode ) {
                            case jumpMode::start:
                            case jumpMode::upBig:
                                jumpCharacters( true, wText->charPtrAtCursor() );
                                jMode = jumpMode::upBig;
                                break;
                            case jumpMode::downBig:
                            default:
                                cycleCharUp( wText->charPtrAtCursor(), upperCase );
                                jMode = jumpMode::none;
                                break;
                            }
                            changeCase( upperCase, wText->charPtrAtCursor() );
                            modified = true;
                            updateDisplay = true;
                        }
                        DEBUG_TRACE( SerialPrintfln( "UP-2 = %d", jMode ) )

                    } else if ( key == ui::kDOWN ) {

                        DEBUG_TRACE( SerialPrintfln( "DN-1 = %d", jMode ) )
                        if ( wText->modifyCharAtCursorIfNull( ' ' ) ) {
                        // if ( wText->insertAtCursor( ' ' ) ) {
                            switch( jMode ) {
                            case jumpMode::start:
                            case jumpMode::downBig:
                                jumpCharacters( false, wText->charPtrAtCursor() );
                                jMode = jumpMode::downBig;
                                break;
                            case jumpMode::upBig:
                            default:
                                cycleCharDown( wText->charPtrAtCursor(), upperCase );
                                jMode = jumpMode::none;
                            }
                            changeCase( upperCase, wText->charPtrAtCursor() );
                            modified = true;
                            updateDisplay = true;
                        }
                        DEBUG_TRACE( SerialPrintfln( "DN-2 = %d", jMode ) )

                    } else if ( key == ui::kBACKSPACE ) {

                        // delete char before cursor
                        if ( wText->backspace() ) {
                            modified = true;
                            updateDisplay = true;
                        }

                    } else if ( key == ui::kDELETE ) {

                        // delete char at cursor
                        if ( wText->deleteAtCursor() ) {
                            modified = true;
                            updateDisplay = true;
                        }

                    } else if ( key == ui::kINSERT ) {

                        // insert char on cursor
                        if ( wText->insertAtCursor( ' ' ) ) {
                            modified = true;
                            updateDisplay = true;
                        }

                    } else if ( key == ui::kCHG_CASE ) { // '1' ) {

                        // toggle case
                        bool afterLast = ( wText->evalPosition() == WindowedText::cPos::rightAfterLast );
                        if ( !afterLast ) {
                            char *p = wText->charPtrAtCursor();
                            if ( *p >= 'A' && *p <= 'Z' ) {
                                upperCase = false;
                                *p = tolower(*p);
                                modified = true;
                                updateDisplay = true;
                            } else if ( *p >= 'a' && *p <= 'z' ) {
                                upperCase = true;
                                *p = toupper(*p);
                                modified = true;
                                updateDisplay = true;
                            } else {
                                upperCase = !upperCase;
                            }
                        } else {
                            upperCase = !upperCase;
                        }

                    } else if ( key == ui::kDECIMAL ) {

                        // if ( wText->insertAtCursor( ' ' ) ) {
                        if ( wText->modifyCharAtCursorIfNull( ' ' ) ) {
                            cycleSymbols( true, wText->charPtrAtCursor() );
                            modified = true;
                            updateDisplay = true;
                        }

                    } else if ( key == ui::kNUMBERS ) { // '0' ) {

                        // if ( wText->insertAtCursor( ' ' ) ) {
                        if ( wText->modifyCharAtCursorIfNull( ' ' ) ) {
                            // char *ptr = wText->charAtCursor();
                            // if ( ptr != nullptr )
                            cycleNumbers( wText->charPtrAtCursor() );
                            modified = true;
                            updateDisplay = true;
                        }

                    } else if ( key == ui::kQUESTION ) {
                        lcd->cursorOff();
                        lcd->printStrAtRow( 0, "\x7F \x7E to move cursor" );
                        lcd->printStrAtRow( 1, "^ v cycle letters" );
                        lcd->printStrAtRow( 2, "  . symbols" );
                        lcd->printStrAtRow( 3, "  0 numbers" );
                        lcd->displayAll();
                        if ( ui::waitForAnyKeyPressed() == ui::kQUESTION ) {
                            lcd->printStrAtRow( 0, "[1] change case" );
                            lcd->printStrAtRow( 1, "[U] insert" );
                            lcd->printStrAtRow( 2, "BS  delete" );
                            lcd->clearRow( 3 );
                            lcd->displayAll();
                            if ( ui::waitForAnyKeyPressed() == ui::kQUESTION ) {
                                lcd->printStrAtRow( 0, "Ok accept" );
                                lcd->printStrAtRow( 1, "X  go back" );
                                lcd->clearRow( 2 );
                                lcd->displayAll();
                                ui::waitForAnyKeyPressed();
                            }
                        }
                        lcd->cursorOn();
                        updateDisplay = true;
                    } else {
                        // unknown key, pass back for caller to handle
                        return key;
                    }
                }
            }

        //
        // CHARACTERS
        //
        protected:

            //
            // JUMP
            //

            bool jumpCore( bool upDown, const char *lookup, char *ch ) {
                // look for character in lookup
                //    up:   get next char, with rollover
                //    down: get prev char, with rollover
                *ch = tolower(*ch);
                char *p = strchr( lookup, *ch );
                if ( p == nullptr ) return false;
                if ( upDown ) {
                    if ( p >= lookup+strlen(lookup)-1 )
                        *ch = lookup[0];
                    else
                        *ch = *(p+1);
                } else {
                    if ( p <= lookup )
                        *ch = *(lookup+strlen(lookup)-1);
                    else
                        *ch = *(p-1);
                }
                return true;
            }

            /*
            // while user keep pressing up... jump big
            // if switch direction jump: med, sml then single character
            // faster to reach destination, similar to binary tree
            // but confusing

            //        _abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_
            // big    ||       |        |        |       |        |        |        ||
            // med    |    |       |        |        |       |        |         |     
            //        _abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_
            // sml     | |   |   |   |    |   |    |   |   |   |   |    |     |   | |
            //        ||-x-|-x-|-x-|-x--|-x-|-x--|-x-|-x-|-x-|-x--|-x-|-x--|--x-|-x-|

            static const char big0[] = " airAIR09";
            //              | | | | | | | |
            static const char med0[] = " aeimrvAEIMRV059";
            //             «|»«|»«|»«|»«|»«|»«|»«|»
            static const char sml0[] = "9 acegkmotvxCEGKMOSVX357";

            void jumpBigOld( bool upDown, char *ch ) {
                if ( *ch == 0 ) *ch = ' ';
                jumpCore( upDown, big0, ch );
            }
            void jumpMedOld( bool upDown, char *ch ) {
                jumpCore( upDown, med0, ch );
            }
            void jumpSmlOld( bool upDown, char *ch ) {
                jumpCore( upDown, sml0, ch );
            }
            */

            // jump several steps if single direction
            // if reversed direction cycle only 1 character at a time

            //        _abcdefghijklmnopqrstuvwxyz_
            // jumps  ||   |   |  |  |  |  |  |
            static constexpr const char* jumps = " aeilorux";

            void jumpCharacters( bool upDown, char *ch ) {
                if ( *ch == 0 ) *ch = ' ';
                jumpCore( upDown, jumps, ch );
            }

            //
            // SYMBOLS
            //

            void cycleSymbols( bool upDown, char *ch ) {
                // based on available LCD characters
                static const char symbols[] = " !\"#$%&'()*+,-./" ":;<=>?@" "[\\]^_`" "{|}";
                if ( *ch == 0 ) {
                    *ch = '!';
                    return;
                }
                if ( !jumpCore( upDown, symbols, ch ) )
                    *ch = '!';
            }

            //
            // NUMBERS
            //

            void cycleNumbers( char *ch ) {
                // 0..9 _
                if ( *ch >='0' && *ch <= '8' )
                    (*ch)++;
                else if ( *ch == '9' )
                    *ch = ' ';
                else
                    *ch = '0';
            }

            //
            // CHARACTER CYCLE
            //

            void cycleCharUp( char *ch, bool upperCase ) {
                if ( *ch == 0 || *ch == ' ' ) {
                    if ( upperCase )
                        *ch = 'A';
                    else
                        *ch = 'a';
                } else if ( *ch >= 'a' && *ch <= 'y' )
                    (*ch)++;
                else if ( *ch == 'z' )
                    *ch = ' ';
                else if ( *ch >= 'A' && *ch <= 'Y' )
                    (*ch)++;
                else if ( *ch == 'Z' )
                    *ch = ' ';
                else if ( *ch >= '0' && *ch <= '8' )
                    (*ch)++;
                else if ( *ch == '9' )
                    *ch = ' ';
                else
                    cycleSymbols( true, ch );
            }

            void cycleCharDown( char *ch, bool upperCase ) {
                if ( *ch == 0 || *ch == ' ' ) {
                    if ( upperCase )
                        *ch = 'Z';
                    else
                        *ch = 'z';
                } else if ( *ch >= 'b' && *ch <= 'z' )
                    (*ch)--;
                else if ( *ch == 'a' )
                    *ch = ' ';
                else if ( *ch >= 'B' && *ch <= 'Z' )
                    (*ch)--;
                else if ( *ch == 'A' )
                    *ch = ' ';
                else if ( *ch >= '1' && *ch <= '9' )
                    (*ch)++;
                else if ( *ch == '0' )
                    *ch = ' ';
                else
                    cycleSymbols( false, ch );
            }

            void changeCase( bool upperCase, char *ch ) {
                if ( upperCase )
                    *ch = toupper(*ch);
                else
                    *ch = tolower(*ch);
            }

    };

    void TEST_editorAlpha() {
        namespace ui = StarterPack::UserInterface;
        
        editorSettings s;
        s.setPosition( 1, 0, 10 );
        // s.setUnit( 11, 1 );
        s.bufferLength = 15;
        s.allowNegative = true;
        s.allowDecimal = true;

        char buffer[21];
        //strcpy( buffer, "app" );

        // set values and check if wrong buffer editing
        buffer[s.bufferLength-1] = 100; // will be null entered chars till end
        buffer[s.bufferLength  ] = 123; // must not be overwritten

        ui::LCD->printAt(  0, 1, "[" );
        ui::LCD->printAt( 11, 1, "]" );

        alphanumericEditor alpha( buffer, s );
        while( alpha.prompt() != ui::kESCAPE );

        Serial.println( (uint8_t) buffer[s.bufferLength-1] );
        Serial.println( (uint8_t) buffer[s.bufferLength  ] );

        Serial.print( buffer );
        Serial.println( "}}}" );
    }

}

#undef DEBUG_TRACE
