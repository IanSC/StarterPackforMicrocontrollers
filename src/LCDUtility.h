//  2022/01 - isc
//
//  LCD Utilities
//  -------------
//
//      chooser
//      

#pragma once
#include <LCDInterface.h>
#include <spUtility.h>
#include <Throttle.h>

#include <UserInput.h>
namespace ui = StarterPack::UserInput;

namespace StarterPack {

namespace LCDUtility {

//
// CHOOSER
//

    class chooser {

        // void a() {
        //     LCD_i2c lcd = LCD_i2c();
        //     uint8_t r = choose( lcd, 1, { "Yes", "No" } );
        // }

        // ROLL YOUR OWN:
        //     chooser p = chooser( lcd, 1, { "Yes", "No" } );
        //     p.display( true );
        //     uint8_t input;
        //     while( true ) {
        //         input = p.poll();
        //         if ( input != 0 ) break;
        //         // ... do something
        //     }

        private:

            LCDInterface *lcd;

            uint8_t    lineNo;            // line# to use
            const char **options;         // pointer to array of string
            int8_t     optionCount;       // number of items
            bool       crossover = false; // allow crossover between 1st/last items

            uint8_t    *itemLen;          // length of each option
            uint8_t    *posInLayout;      // position of each option if arranged as one long string
                                          // eg. [apple][banana][carrots]
                                          //     ^      ^       ^          starting positions

            uint8_t visibleStart = 0;     // char position in layout that is dispayed on leftmost of screen

            bool (*backgroundProcess)(void) = nullptr;
            Throttle *throttler = nullptr;

        public:

            uint8_t selectedItem          = 0; // item chosen 1..N
            uint8_t selectedLeftLocation  = 0; // left position of selected item on screen
            uint8_t selectedRightLocation = 0; // right position of selected item on screen

            ~chooser() {
                delete itemLen;
                delete posInLayout;
            }

        public:

            template<size_t optCount>
            chooser( LCDInterface &lcd, uint8_t lineNo, const char* (&options)[optCount],
            bool crossover = false ) {
                this->lcd = &lcd;
                this->lineNo = lineNo;
                this->options = options;
                this->optionCount = optCount;
                this->crossover = crossover;

                // limit to 255 items or 255 characters total
                if ( optionCount > UINT8_MAX )
                    optionCount = UINT8_MAX;
                itemLen = new uint8_t[optionCount];
                posInLayout = new uint8_t[optionCount];
                int16_t pos = 0;
                for( uint8_t i = 0; i < optionCount ; i++ ) {
                    size_t len = strlen( options[i] );
                    if ( len > UINT8_MAX ) {
                        // too long, length storage uint8_t
                        optionCount = i;
                        break;
                    }
                    itemLen[i] = len;
                    posInLayout[i] = pos;
                    pos += itemLen[i] + 1;
                    if ( pos > UINT8_MAX ) {
                        // using uint8_t only for computations
                        optionCount = i;
                        break;
                    }
                    pos++;
                }
            }

            template<size_t optionCount>
            chooser( LCDInterface &lcd, uint8_t lineNo, const char* (&options)[optionCount],
            bool (*backgroundProcess)(void), bool crossover = false )
            : chooser( lcd, lineNo, options, crossover ) {
                this->backgroundProcess = backgroundProcess;
            }

            template<size_t optionCount>
            chooser( LCDInterface &lcd, uint8_t lineNo, const char* (&options)[optionCount],
            Throttle &throttler, bool crossover = false )
            : chooser( lcd, lineNo, options, crossover ) {
                this->throttler = &throttler;
            }

        //
        // OLD METHOD
        //
        // - build layout and use it for display
        // - need more memory as original strings!
        /*
        private:

            char *layout; // string with all options positioned

            inline void freeBuffer() {
                free layout;
            }
            void buildBuffer() {
                // limit to 255 items or 255 characters total
                if ( optionCount > UINT8_MAX )
                    optionCount = UINT8_MAX;
                itemLen = new uint8_t[optionCount];
                posInLayout = new uint8_t[optionCount];
                int16_t pos = 0;
                for( uint8_t i = 0; i < optionCount ; i++ ) {
                    size_t len = strlen( options[i] );
                    if ( len > UINT8_MAX ) {
                        // too long, length storage uint8_t
                        optionCount = i;
                        break;
                    }
                    itemLen[i] = len;
                    posInLayout[i] = pos;                
                    pos += itemLen[i] + 1;
                    if ( pos > UINT8_MAX ) {
                        // using uint8_t only for computations
                        optionCount = i;
                        break;
                    }
                    pos++;
                }
                uint16_t layoutSize = posInLayout[optionCount-1] + itemLen[optionCount-1] + 1;
                layout = new char[layoutSize];
                pos = 0;
                for( uint8_t i = 0; i < optionCount ; i++ ) {
                    layout[pos++] = ' ';
                    strcpy( layout+pos, options[i] );
                    pos += itemLen[i];
                    layout[pos++] = ' ';
                }            
                layout[pos] = 0;
                // Serial.println( layout );
            }

            void displayUsingBuffer( bool showSelection = true ) {
                bool overflowRight = false;
                lcd->setCursor( 0, lineNo );

                uint8_t chToDisplay = strlen( layout + visibleStart );
                if ( chToDisplay > lcd->maxColumns ) {
                    chToDisplay = lcd->maxColumns - 1;
                    overflowRight = true;
                }

                #if defined(ESP32)
                    lcd->printf( "%.*s", chToDisplay, layout + visibleStart );
                #else
                    char *ptr = layout + visibleStart;
                    for( int i = 0; i < chToDisplay ; i++ )
                        lcd->write( *ptr++ );
                #endif

                // clear space
                if ( chToDisplay < lcd->maxColumns )
                    lcd->printCharsN( ' ', lcd->maxColumns - chToDisplay );

                // overflow markers
                if ( layout != 0 )
                    lcd->writeAt( 0, lineNo, 0x7F );
                if ( overflowRight )
                    lcd->writeAt( lcd->maxColumns-1, lineNo, 0x7E );

                if ( showSelection )
                    this->showSelection();
                lcd->displayAll();
            }*/

        public:

            uint8_t prompt() {
                display( true );
                uint8_t input;
                while( true ) {
                    input = poll();
                    if ( input != 0 )
                        return input;
                    if ( backgroundProcess != nullptr ) {
                        if ( !backgroundProcess() ) {
                            // terminated, no selection
                            selectedItem  = 0;
                            return selectedItem;
                        }
                    }
                    if ( throttler != nullptr )
                        throttler->trigger();
                }
            }

            void display( bool showSelection = true ) {
                bool overflowRight = false;
                lcd->setCursor( 0, lineNo );

                // print contents of layout from [visibleStart] to [visibleEnd]
                // determine which options are visible
                uint8_t visibleEnd = visibleStart + lcd->maxColumns - 1;
                uint8_t itemStart = 0, itemEnd;
                int8_t lcdSpace = lcd->maxColumns;
                // Serial.println( "*** START ***" );
                for( uint8_t i = 0 ; i < optionCount ; i++ ) {
                    if ( lcdSpace < 2 ) {
                        // no need to continue, need at least 2 characters
                        // free for an option to be visible by 1 char
                        overflowRight = true;
                        break;
                    }
                    itemEnd = itemStart + itemLen[i] + 1;
                    // SerialPrintf( "%2d/%2d - %2d/%2d - %s\n", visibleStart, visibleEnd,
                    //     itemStart, itemEnd, options[i] );
                    // print if there is overlap between:
                    //    visibleStart/visibleEnd and itemStart/itemEnd
                    if ( itemEnd < visibleStart ) {        // before start
                        // Serial.println( "   not visible - way before");
                    } else if ( itemStart > visibleEnd ) { // after end
                        // Serial.println( "   not visible - way after");
                    } else {
                        // SerialPrintf( "   lcd space = %d\n", lcdSpace );
                        // Serial.print( "   #" );
                        if ( itemStart >= visibleStart ) {
                            // Serial.print( " " );
                            lcd->write( ' ' );
                            lcdSpace--;
                        }
                        int8_t chOffset = visibleStart - itemStart - 1;
                        if ( chOffset < 0 ) chOffset = 0;
                        lcd->printStrN( options[i] + chOffset, lcdSpace );
                        // SerialPrintStrN( options[i] + chOffset, lcdSpace );
                        lcdSpace -= ( itemLen[i] - chOffset );
                        if ( lcdSpace <= 0 ) {
                            // Serial.println( "(!)" );
                            overflowRight = true;
                            break;
                        }
                        // Serial.print( " " );
                        lcd->write( ' ' );
                        lcdSpace--;
                        // Serial.println( "#" );
                    }
                    itemStart = itemEnd + 1;
                }
                if ( lcdSpace > 0 )
                    lcd->printCharsN( ' ', lcdSpace );

                // overflow markers
                if ( visibleStart != 0 )
                    lcd->writeAt( 0, lineNo, 0x7F );
                if ( overflowRight )
                    lcd->writeAt( lcd->maxColumns-1, lineNo, 0x7E );

                if ( showSelection )
                    this->showSelection();
                lcd->displayAll();
            }

            void showSelection() { showMarkersCore( '[', ']' ); }
            void hideSelection() { showMarkersCore( ' ', ' ' ); }

            uint8_t poll() {

                // return:
                // 0    - no selection
                // 1..N - option selected

                ui::Keys input = ui::getRepeatingKey( ui::Keys::LeftRightOkay );
                if ( input == ui::Keys::None ) return 0;
                if ( input == ui::Keys::Okay ) {
                    return selectedItem + 1;
                }
                hideSelection();

                if ( input == ui::Keys::Left ) {
                    if ( selectedItem <= 0 ) {
                        if ( crossover ) selectedItem = optionCount - 1;
                    } else
                        selectedItem--;
                } else if ( input == ui::Keys::Right ) {
                    if ( selectedItem >= optionCount - 1 ) {
                        if ( crossover ) selectedItem = 0;
                    } else
                        selectedItem++;
                }

                computePositions();
                showSelection();
                return 0;
            }

        private:

            void showMarkersCore( char left, char right ) {
                uint8_t S = posInLayout[ selectedItem ];
                uint8_t E = S + itemLen[ selectedItem ] + 1;
                S -= visibleStart;
                E -= visibleStart;
                if ( S >= 0 && S < lcd->maxColumns )
                    lcd->printAt( S, lineNo, left );
                if ( E >= 0 && E < lcd->maxColumns )
                    lcd->printAt( E, lineNo, right );
                selectedLeftLocation  = S;
                selectedRightLocation = E;
                lcd->displayAll();
            }

            void computePositions() {
                // S/E = start/end position of selection in buffer
                uint8_t S = posInLayout[ selectedItem ];
                uint8_t E = S + itemLen[ selectedItem ] + 2;

                if ( E > visibleStart + lcd->maxColumns - 1 ) {
                    // item is beyond rightmost of screen
                    // Serial.println( "End Beyond" );
                    visibleStart = E - lcd->maxColumns;
                    if ( visibleStart < 0 )
                        visibleStart = 0;
                }
                if ( E == visibleStart + lcd->maxColumns ) {
                    // exactly at end of screen
                    // Serial.println( "End Exact" );
                    if ( selectedItem < optionCount - 1 ) {
                        // not last item, adjust for overflow marker
                        visibleStart++;
                    }
                }
                if ( S < visibleStart ) {
                    // item is beyond leftmost of screen
                    // Serial.println( "Start Before" );
                    visibleStart = S;
                    // for left overflow marker
                    if ( visibleStart != 0 )
                        visibleStart--;
                }
                display( true );
            }

    };

    template<size_t optionCount>
    static inline uint8_t choose( LCDInterface &lcd, uint8_t lineNo, const char* (&options)[optionCount],
    bool crossover = false ) {
        chooser c = chooser( lcd, lineNo, options, crossover );
        return c.prompt();
    }

    template<size_t optionCount>
    static inline uint8_t choose( LCDInterface &lcd, uint8_t lineNo, const char* (&options)[optionCount],
    bool (*backgroundProcess)(void), bool crossover = false ) {
        chooser c = chooser( lcd, lineNo, options, backgroundProcess, crossover );
        return c.prompt();
    }

    template<size_t optionCount>
    static inline uint8_t choose( LCDInterface &lcd, uint8_t lineNo, const char* (&options)[optionCount],
    Throttle &throttler, bool crossover = false ) {
        chooser c = chooser( lcd, lineNo, options, throttler, crossover );
        return c.prompt();
    }

//
// LONG MESSAGE
//

    // https://solarianprogrammer.com/2016/11/28/cpp-passing-c-style-array-with-size-information-to-function/
    template<size_t msgCount>
    static void showMultiLineMsg( LCDInterface &lcd, const char* (&msg)[msgCount], void (*backgroundProcess)(void) = nullptr ) {

        lcd.clear();
        int lineNo = 0;
        
        while ( true ) {

            // display text
            int tmp = lineNo;
            for( int i = 0 ; i < lcd.maxRows ; i++ ) {
                if ( tmp < msgCount )
                    lcd.printStrAtLine( i, msg[tmp] );
                else
                    lcd.clearLine( i );
                tmp++;
            }
            lcd.displayAll();

            ui::Keys input = ui::getRepeatingKey( ui::Keys::UpDownOkay );
            switch( input ) {
            case ui::Keys::Okay:
                return;
            case ui::Keys::Up:
                if ( lineNo > 0 ) lineNo--;
                break;
            case ui::Keys::Down:
                if ( lineNo < msgCount - lcd.maxRows ) lineNo++;
                break;
            default:
                break;
            }

            if ( backgroundProcess != nullptr )
                backgroundProcess();
        }
    }

//
// BLINK
//

    class blink {

            uint32_t blinkTime = millis();
            bool blinkMode = false;

            LCDInterface *lcd;
            uint8_t x;
            uint8_t y;

        public:

            blink( LCDInterface &lcd, uint8_t col, uint8_t row ) {
                this->lcd = &lcd;
                this->x = col;
                this->y = row;
            }

            void update() {
                uint32_t now = millis();
                if ( now - blinkTime >= 1000 ) {
                    lcd->writeAt( x, y, blinkMode ? 0xA5 : ' ' );
                    blinkMode = !blinkMode;
                    blinkTime = now;
                    lcd->displayAll();
                }
            }

            void setPosition( uint8_t col, uint8_t row ) {
                if ( !blinkMode ) {
                    // false means dot is currently displayed
                    lcd->writeAt( x, y, blinkMode ? 0xA5 : ' ' );
                    blinkMode = !blinkMode;
                    lcd->displayAll();
                }
                this->x = col;
                this->y = row;
            }

    };

}

}