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

            uint8_t    row;               // lcd row to use
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

            void setSelectedItem( uint8_t selectedItem ) {
                if ( selectedItem >= optionCount )
                    selectedItem = optionCount;
                this->selectedItem = selectedItem;
                computePositions();
            }

        public:

            template<size_t optCount>
            inline chooser( LCDInterface &lcd, uint8_t lineNo, const char* (&options)[optCount],
            bool crossover, uint8_t initialSelection = 0 )
            : chooser( lcd, lineNo, options, initialSelection, crossover ) {
            }

            // template<size_t optCount>
            // inline chooser( LCDInterface &lcd, uint8_t lineNo, const char* (&options)[optCount],
            // uint8_t initialSelection, bool crossover = false )
            // : chooser( lcd, lineNo, options, initialSelection, crossover ) {
            // }

            chooser( LCDInterface &lcd, uint8_t row, uint8_t optCount, const char **options,
            uint8_t initialSelection = 0, bool crossover = false ) {

                this->lcd = &lcd;
                this->row = row;
                this->options = options;
                this->optionCount = optCount;
                this->crossover = crossover;

                // for( int i = 0 ; i < optCount ; i++ )
                //     Serial.println( options[i] );

                // limit to 255 items or 255 characters total
                if ( optionCount > UINT8_MAX )
                    optionCount = UINT8_MAX;
                itemLen = new uint8_t[optionCount];
                posInLayout = new uint8_t[optionCount];
                int16_t pos = 0;
                for( uint8_t i = 0; i < optionCount ; i++ ) {
                    size_t len = strlen( options[i] );
                    if ( len > UINT8_MAX ) {
                        // too long, length storage is only uint8_t
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

                setSelectedItem( initialSelection );
                // selectedItem = initialSelection;
                // if ( selectedItem >= optionCount )
                //     selectedItem = optionCount;
                // computePositions();
            }

            template<size_t optCount>
            chooser( LCDInterface &lcd, uint8_t lineNo, const char* (&options)[optCount],
            uint8_t initialSelection, bool crossover = false ) {

                this->lcd = &lcd;
                this->row = lineNo;
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

                setSelectedItem( initialSelection );
                // selectedItem = initialSelection;
                // if ( selectedItem >= optionCount )
                //     selectedItem = optionCount;
                // computePositions();
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
                lcd->setCursor( 0, row );

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
                    lcd->writeAt( 0, row, 0x7F );
                if ( overflowRight )
                    lcd->writeAt( lcd->maxColumns-1, row, 0x7E );

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

                if ( ui::KeyHandler == nullptr ) return 0;

                ui::Keys input = ui::getRepeatingKey( ui::Keys::LeftRightOkay );
                if ( input == ui::Keys::None ) return 0;
                if ( input == ui::Keys::Okay ) {
                    return selectedItem + 1;
                }

                /*
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
                display( true );
                */

                if ( input == ui::Keys::Left )
                    moveLeft();
                else if ( input == ui::Keys::Right )
                    moveRight();

                // showSelection();
                return 0;
            }

            void moveLeft() {
                hideSelection();
                if ( selectedItem <= 0 ) {
                    if ( crossover ) selectedItem = optionCount - 1;
                } else
                    selectedItem--;
                computePositions();
                display( true );
            }

            void moveRight() {
                hideSelection();
                if ( selectedItem >= optionCount - 1 ) {
                    if ( crossover ) selectedItem = 0;
                } else
                    selectedItem++;
                computePositions();
                display( true );
            }

        private:

            void showMarkersCore( char left, char right ) {
                uint8_t S = posInLayout[ selectedItem ];
                uint8_t E = S + itemLen[ selectedItem ] + 1;
                S -= visibleStart;
                E -= visibleStart;
                if ( S >= 0 && S < lcd->maxColumns )
                    lcd->printAt( S, row, left );
                if ( E >= 0 && E < lcd->maxColumns )
                    lcd->printAt( E, row, right );
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

    };

    // template<size_t optionCount>
    // static inline uint8_t choose( LCDInterface &lcd, uint8_t lineNo, const char* (&options)[optionCount],
    // bool crossover = false ) {
    //     chooser c = chooser( lcd, lineNo, options, crossover );
    //     return c.prompt();
    // }

    template<size_t optCount>
    static inline uint8_t choose( LCDInterface &lcd, uint8_t lineNo, const char* (&options)[optCount],
    bool crossover = false, uint8_t initialSelection = 0 ) {
        chooser c = chooser( lcd, lineNo, options, initialSelection, crossover );
        return c.prompt();
    }

    template<size_t optCount>
    static inline uint8_t choose( LCDInterface &lcd, uint8_t lineNo, const char* (&options)[optCount],
    uint8_t initialSelection, bool crossover = false ) {
        chooser c = chooser( lcd, lineNo, options, initialSelection, crossover );
        return c.prompt();
    }

    // template<size_t optionCount>
    // static inline uint8_t choose( LCDInterface &lcd, uint8_t lineNo, const char* (&options)[optionCount],
    // bool crossover = false ) {
    //     chooser c = chooser( lcd, lineNo, options, crossover );
    //     return c.prompt();
    // }

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

    static inline uint8_t chooseYesNo( LCDInterface &lcd, uint8_t lineNo,
    bool crossover = false, uint8_t initialSelection = 0 ) {
        const char *options[] = { "Yes", "No" };
        chooser c( lcd, lineNo, options, initialSelection, crossover );
        return c.prompt();
    }

    static inline uint8_t chooseYesNoCancel( LCDInterface &lcd, uint8_t lineNo,
    bool crossover = false, uint8_t initialSelection = 0 ) {
        const char *options[] = { "Yes", "No", "Cancel" };
        chooser c( lcd, lineNo, options, initialSelection, crossover );
        return c.prompt();
    }

    static inline uint8_t chooseNoYes( LCDInterface &lcd, uint8_t lineNo,
    bool crossover = false, uint8_t initialSelection = 0 ) {
        const char *options[] = { "No", "Yes" };
        chooser c( lcd, lineNo, options, initialSelection, crossover );
        return c.prompt();
    }

    static inline uint8_t chooseNoYesCancel( LCDInterface &lcd, uint8_t lineNo,
    bool crossover = false, uint8_t initialSelection = 0 ) {
        const char *options[] = { "No", "Yes", "Cancel" };
        chooser c( lcd, lineNo, options, initialSelection, crossover );
        return c.prompt();
    }

//
// MENU SYSTEM
//

/*
    class menuSet;

    class menuEntry { public:
        const char *option;
        const char *description;
        uint8_t result;
        menuEntry *next = nullptr;
        menuSet *submenu = nullptr;
        inline void assignSubmenu( menuSet &menu ) {
            this->submenu = &menu;
        }
    };

    class menuSet { 
        
        public:

            const char *header;
            menuEntry *options = nullptr;

        menuSet( const char *header ) {
            this->header = header;
        }
        menuEntry *add( char *option, char *description, uint8_t result ) {
            menuEntry *e = new menuEntry();
            e->option = option;
            e->description = description;
            e->result = result;
            if ( options == nullptr )
                options = e;
            else {
                menuEntry *ptr = options;
                while( ptr->next != nullptr )
                    ptr = ptr->next;
                ptr->next = e;
            }
            return e;
        }
        void display( LCDInterface &lcd, uint8_t lineNo ) {
            uint8_t count = 0;
            menuEntry *ptr = options;
            while ( ptr != nullptr ) {
                count++;
                ptr = ptr->next;
            }
            const char *o[count];
            ptr = options;
            uint8_t i = 0;
            while ( ptr != nullptr )
                o[i++] = ptr->option;
            // chooser c( lcd, lineNo, (int) count, o );
            // c.prompt();
            // choose( lcd, lineNo, o );
        }
        
    };
*/
    // class menuSystem {

    //     public:


    //         struct menuSet {
    //             char *description;
    //             menuEntry *options;

    //         };

    //         struct menuData {
    //             const char *header;        // top line to display (if applicable)
    //             const char *options[];     // menu choices
    //             uint8_t     optionCount;   // number of menu items
    //             const char *description[]; // top line to display per menu item (if applicable)
    //             int8_t      result[];      // next state if selected
    //             uint8_t     resultCount;
    //             uint8_t     backMenu;      // menu number to go back to (if applicable)
    //             uint8_t     backPos;       // menu position to go back to (if applicable)
    //         };

    //         // template<size_t optCount>
    //         // static inline uint8_t choose( LCDInterface &lcd, uint8_t lineNo, const char* (&options)[optCount],
    //         // uint8_t initialSelection, bool crossover = false ) {
    //         //     chooser c = chooser( lcd, lineNo, options, initialSelection, crossover );
    //         //     return c.prompt();
    //         // }

    //         template<size_t optionCnt, size_t descCnt, size_t resultCnt>
    //         menuData *create( const char *header, const char* (&options)[optionCnt],
    //         const char* (&description)[descCnt],
    //         const char* (&results)[resultCnt]  ) {
    //             if ( resultCnt != descCnt ) return null;
    //             menuData *m    = new menuData();
    //             m->header      = header;
    //             m->options     = options;
    //             m->optionCount = optCount;
    //             m->description = description;
    //             m->result      = results;
    //             m->resultCount = resultCount;
    //             return m;
    //         }

    // };




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
                    lcd.printStrAtRow( i, msg[tmp] );
                else
                    lcd.clearRow( i );
                tmp++;
            }
            lcd.displayAll();

            if ( ui::KeyHandler != nullptr ) {
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
