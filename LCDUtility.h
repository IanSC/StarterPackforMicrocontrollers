//  2022/01 - isc
//
//  LCD Utilities
//  -------------
//
//      chooser
//      


#pragma once
#include <LCDInterface.h>

#include <UserInput.h>
namespace ui = StarterPack::UserInput;

#include <Throttle.h>

namespace StarterPack {

namespace LCDUtility {

//
// CHOOSER
//

    class chooser {

        // void a() {
        //     zz_LCD zz;
        //     picker p = choose( zz, 1, { "Yes", "No" } );
        // }

        // to roll your own
        //     picker p = pick( &lcd, 1, { "Yes", "No" }, false );
        //     p.display( true );
        //     uint8_t input;
        //     while( true ) {
        //         input = p.poll();
        //         if ( input != 0 )
        //             break;
        //         // ... do something
        //     }

        private:

            LCDInterface *lcd;

            uint8_t    lineNo;            // line# to use
            const char **options;         // pointer to array of string
            int8_t     optionCount;       // number of items
            bool       crossover = false; // allow crossover between 1st/last items

            #define BUFFER_SIZE 200
            #define MAX_ITEMS   20

            char    buffer[BUFFER_SIZE];    // string with items positioned
            uint8_t itemLen[MAX_ITEMS];     // char length of each item
            uint8_t posInBuffer[MAX_ITEMS]; // position of item in buffer

            uint8_t bufferPosition = 0;     // char position in buffer that is dispayed on leftmost of screen

            bool (*backgroundProcess)(void) = nullptr;
            Throttle *throttler = nullptr;

        public:

            uint8_t selectedItem  = 0;    // item chosen 1..N
            uint8_t selectedLeft  = 0;    // left position of selected item on screen
            uint8_t selectedRight = 0;    // right position of selected item on screen

        public:

            template<size_t optionCount>
            chooser( LCDInterface &lcd, uint8_t lineNo, const char* (&options)[optionCount],
            bool crossover = false, bool (*backgroundProcess)(void) = nullptr ) {
                this->lcd = &lcd;
                this->lineNo = lineNo;
                this->options = options;
                this->optionCount = optionCount;
                this->crossover = crossover;
                this->backgroundProcess = backgroundProcess;
                initCore();
            }

            template<size_t optionCount>
            chooser( LCDInterface &lcd, uint8_t lineNo, const char* (&options)[optionCount],
            Throttle &throttler, bool crossover = false ) {
                this->lcd = &lcd;
                this->lineNo = lineNo;
                this->options = options;
                this->optionCount = optionCount;
                this->crossover = crossover;
                this->throttler = &throttler;
                initCore();
            }

        private:

            void initCore() {
                if ( optionCount > MAX_ITEMS )
                    this->optionCount = MAX_ITEMS;

                // build buffer
                uint8_t pos = 0;
                for( uint8_t i = 0; i < optionCount ; i++ ) {
                    itemLen[i] = strlen( options[i] );
                    if ( pos + itemLen[i] + 2 + 1 > BUFFER_SIZE ) {
                        // ignore if over the buffer
                        this->optionCount = i;
                        break;
                    }
                    posInBuffer[i] = pos;
                    buffer[pos++] = ' ';
                    strcpy( buffer+pos, options[i] );
                    pos += itemLen[i];
                    buffer[pos++] = ' ';
                }            
                buffer[pos] = 0;
            }
                    
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

            void display( bool showSelection ) {
                bool overflowRight = false;
                lcd->setCursor( 0, lineNo );
                uint8_t chToDisplay = strlen( buffer + bufferPosition );
                if ( chToDisplay > lcd->maxColumns ) {
                    chToDisplay = lcd->maxColumns - 1;
                    overflowRight = true;
                }
                #if defined(ESP32)
                    lcd->printf( "%.*s", chToDisplay, buffer+bufferPosition );
                #else
                    char *ptr = buffer + bufferPosition;
                    for( int i = 0; i < chToDisplay ; i++ )
                        lcd->write( *ptr++ );
                #endif

                // clear space
                if ( chToDisplay < lcd->maxColumns ) {
//                    chToDisplay = lcd->maxColumns - chToDisplay;
                    lcd->printCharsN( ' ', lcd->maxColumns - chToDisplay );
//                    lcd->print( std::string( chToDisplay, ' ' ).c_str() );
                }

                // overflow markers
                if ( bufferPosition != 0 )
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

                ui::Keys input = ui::getRepeatingInput( ui::Keys::LeftRightOkay );
                // iKeys::userKey input = iKeys::getUserInput();
                if ( input == ui::Keys::None ) return 0;
                if ( input == ui::Keys::Okay ) {
                    // inputButtonOkay.flagWaitForKeyup();
                    return selectedItem + 1;
                }
                hideSelection();

                if ( input == ui::Keys::Left ) {
                    // inputRotateLeft.flagWaitForKeyup();
                    if ( selectedItem <= 0 ) {
                        if ( crossover ) selectedItem = optionCount - 1;
                    } else
                        selectedItem--;
                } else if ( input == ui::Keys::Right ) {
                    // inputRotateRight.flagWaitForKeyup();
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
                uint8_t S = posInBuffer[ selectedItem ];
                uint8_t E = S + itemLen[ selectedItem ] + 1;
                S -= bufferPosition;
                E -= bufferPosition;
                if ( S >= 0 && S < lcd->maxColumns )
                lcd->printAt( S, lineNo, left );
                if ( E >= 0 && E < lcd->maxColumns )
                    lcd->printAt( E, lineNo, right );
                selectedLeft  = S;
                selectedRight = E;
                lcd->displayAll();
            }

            void computePositions() {
                // S/E = start/end position of selection in buffer
                uint8_t S = posInBuffer[ selectedItem ];
                uint8_t E = S + itemLen[ selectedItem ] + 1;
                if ( S < bufferPosition ) {
                    // item is beyond leftmost of screen
                    bufferPosition = S;
                    // for left overflow marker
                    if ( bufferPosition != 0 )
                        bufferPosition--;
                    display( true );
                } else if ( E > bufferPosition + lcd->maxColumns ) {
                    // item is beyond rightmost of screen
                    bufferPosition = E - lcd->maxColumns + 1;
                    if ( strlen( buffer+bufferPosition ) > lcd->maxColumns ) {
                        // still some items on the right
                        // adjust to display overflow marker
                        bufferPosition++;
                    }
                    if ( bufferPosition < 0 )
                        bufferPosition = 0;
                    display( true );
                }
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
        chooser c = chooser( lcd, lineNo, options, crossover, backgroundProcess );
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
    static void show( LCDInterface &lcd, const char* (&msg)[msgCount], void (*backgroundProcess)(void) = nullptr ) {

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

            ui::Keys input = ui::getRepeatingInput( ui::Keys::UpDownOkay );
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