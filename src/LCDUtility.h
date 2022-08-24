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
#include <UserInterface.h>

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

            // LCDInterface *lcd;

            uint8_t    row;               // lcd row to use
            const char **options;         // pointer to array of string
            int8_t     optionCount;       // number of items

            uint8_t    *itemLen;          // length of each option
            uint8_t    *posInLayout;      // position of each option if arranged as one long string
                                          // eg. [apple][banana][carrots]
                                          //     ^      ^       ^          starting positions

            uint8_t visibleStart = 0;     // char position in layout that is dispayed on leftmost of screen

                    // bool (*backgroundProcess)(void) = nullptr;
            Throttle *throttler = nullptr;

        public:

            uint8_t selectedItem          = 0; // item chosen 1..N
            uint8_t selectedLeftLocation  = 0; // left position of selected item on screen
            uint8_t selectedRightLocation = 0; // right position of selected item on screen

            bool    crossover = false;         // allow crossover between 1st/last items

            ~chooser() {
                delete[] itemLen;
                delete[] posInLayout;
            }

            void setSelectedItem( uint8_t selectedItem ) {
                if ( selectedItem >= optionCount )
                    selectedItem = optionCount;
                this->selectedItem = selectedItem;
                computePositions();
            }

        protected:
            typedef std::function<bool(void)> backgroundProcessDelegate;
            backgroundProcessDelegate backgroundProcess = nullptr;
        public:
            void assignBackgroundProcess( backgroundProcessDelegate backgroundProcess ) {
                this->backgroundProcess = backgroundProcess;
            }        
            void assignBackgroundProcess( bool (*backgroundProcessFunc)() ) {
                this->backgroundProcess = [backgroundProcessFunc]() { return backgroundProcessFunc(); };
            }

        public:

            chooser( uint8_t row, uint8_t optCount, const char **options,
            uint8_t initialSelection = 0, bool crossover = false ) {

                this->row = row;
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
            }

            template<size_t optCount>
            inline chooser( uint8_t row, const char* (&options)[optCount],
                bool crossover, uint8_t initialSelection = 0 )
            : chooser( row, optCount, options, initialSelection, crossover ) {}

            template<size_t optCount>
            inline chooser( uint8_t row, const char* (&options)[optCount],
                uint8_t initialSelection, bool crossover = false )
            : chooser( row, optCount, options, initialSelection, crossover ) {}

            // background process
            template<size_t optCount>
            inline chooser( uint8_t row, const char* (&options)[optCount],
                bool (*backgroundProcess)(void), uint8_t initialSelection = 0, bool crossover = false )
            : chooser( row, optCount, options, initialSelection, crossover ) {
                this->backgroundProcess = backgroundProcess;
            }

            // Throttle
            template<size_t optCount>
            inline chooser( uint8_t row, const char* (&options)[optCount],
                Throttle &throttler, uint8_t initialSelection = 0, bool crossover = false )
            : chooser( row, optCount, options, initialSelection, crossover ) {
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
                namespace ui = StarterPack::UserInterface;
                if ( !ui::hasScreen() ) return;
                LCDInterface *lcd = ui::LCD;

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
                namespace ui = StarterPack::UserInterface;

                // return:
                // 0    - no selection
                // 1..N - option selected

                if ( ui::KeyHandler == nullptr ) return 0;

                uint8_t key = ui::getRepeatingKey();
                if ( key == ui::kENTER ) {
                    ui::waitUntilNothingIsPressed();
                    return selectedItem + 1;
                }
                if ( key == ui::kLEFT || key == ui::kUP )
                    moveLeft();
                else if ( key == ui::kRIGHT || key == ui::kDOWN )
                    moveRight();
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
                namespace ui = StarterPack::UserInterface;
                if ( !ui::hasScreen() ) return;
                LCDInterface *lcd = ui::LCD;

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
                namespace ui = StarterPack::UserInterface;
                if ( !ui::hasScreen() ) return;
                LCDInterface *lcd = ui::LCD;

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

    };

    // BASIC
    template<size_t optCount>
    static inline uint8_t choose( uint8_t row, const char* (&options)[optCount],
    bool crossover = false, uint8_t initialSelection = 0 ) {
        chooser c = chooser( row, optCount, options, initialSelection, crossover );
        return c.prompt();
    }

    // BASIC
    template<size_t optCount>
    static inline uint8_t choose( uint8_t row, const char* (&options)[optCount],
    uint8_t initialSelection, bool crossover = false ) {
        chooser c = chooser( row, optCount, options, initialSelection, crossover );
        return c.prompt();
    }

    // WITH BACKGROUND PROCESS
    template<size_t optCount>
    static inline uint8_t choose( uint8_t row, const char* (&options)[optCount],
    bool (*backgroundProcess)(void), bool crossover = false ) {
        chooser c = chooser( row, options, backgroundProcess, crossover );
        return c.prompt();
    }

    // WITH THROTTLER
    template<size_t optCount>
    static inline uint8_t choose( uint8_t row, const char* (&options)[optCount],
    Throttle &throttler, bool crossover = false ) {
        chooser c = chooser( row, options, throttler, crossover );
        return c.prompt();
    }

    // WITH MESSAGE
    template<size_t optCount>
    static inline uint8_t choose( uint8_t captionRow, const char *caption, uint8_t optionsRow, const char* (&options)[optCount],
    bool crossover = false, uint8_t initialSelection = 0 ) {
        namespace ui = StarterPack::UserInterface;
        if ( !ui::hasScreen() ) return 0;
        ui::LCD->printStrAtRow( captionRow, caption );
        chooser c = chooser( optionsRow, optCount, options, initialSelection, crossover );
        return c.prompt();
    }

    // WITH MESSAGE
    template<size_t optCount>
    static inline uint8_t choose( uint8_t captionRow, const char *caption, uint8_t optionsRow, const char* (&options)[optCount],
    uint8_t initialSelection, bool crossover = false ) {
        namespace ui = StarterPack::UserInterface;
        if ( !ui::hasScreen() ) return 0;
        ui::LCD->printStrAtRow( captionRow, caption );
        chooser c = chooser( optionsRow, optCount, options, initialSelection, crossover );
        return c.prompt();
    }

    //
    // PREDEFINED
    //

    // YES/NO - CORE
    static uint8_t chooseYesNoCore( uint8_t captionRow, const char *caption,
    uint8_t optionsRow,
    uint8_t initialSelection, bool crossover,
    bool (*backgroundProcess)(void) ) {
        namespace ui = StarterPack::UserInterface;
        if ( ui::LCD == nullptr ) return 0;
        static const char *options[] = { "Yes", "No" };
        if ( caption != nullptr )
            ui::LCD->printStrAtRow( captionRow, caption );
        chooser c( optionsRow, options, backgroundProcess, initialSelection, crossover );
        return c.prompt();
    }

    // YES/NO/CANCEL - CORE
    static uint8_t chooseYesNoCancelCore( uint8_t captionRow, const char *caption,
    uint8_t optionsRow,
    uint8_t initialSelection, bool crossover,
    bool (*backgroundProcess)(void) ) {
        namespace ui = StarterPack::UserInterface;
        if ( ui::LCD == nullptr ) return 0;
        static const char *options[] = { "Yes", "No", "Cancel" };
        if ( caption != nullptr )
            ui::LCD->printStrAtRow( captionRow, caption );
        chooser c( optionsRow, options, backgroundProcess, initialSelection, crossover );
        return c.prompt();
    }

     // NO/YES - CORE
    static uint8_t chooseNoYesCore( uint8_t captionRow, const char *caption,
    uint8_t optionsRow,
    uint8_t initialSelection, bool crossover,
    bool (*backgroundProcess)(void) ) {
        namespace ui = StarterPack::UserInterface;
        if ( ui::LCD == nullptr ) return 0;
        static const char *options[] = { "No", "Yes" };
        if ( caption != nullptr )
            ui::LCD->printStrAtRow( captionRow, caption );
        chooser c( optionsRow, options, backgroundProcess, initialSelection, crossover );
        return c.prompt();
    }

    // NO/YES/CANCEL - CORE
    static uint8_t chooseNoYesCancelCore( uint8_t captionRow, const char *caption,
    uint8_t optionsRow,
    uint8_t initialSelection, bool crossover,
    bool (*backgroundProcess)(void) ) {
        namespace ui = StarterPack::UserInterface;
        if ( ui::LCD == nullptr ) return 0;
        static const char *options[] = { "No", "Yes", "Cancel" };
        if ( caption != nullptr )
            ui::LCD->printStrAtRow( captionRow, caption );
        chooser c( optionsRow, options, backgroundProcess, initialSelection, crossover );
        return c.prompt();
    }

    // plain
    // with caption
    // with background process
    // with caption + background process
    #define CHOOSER_VARIATIONS(fName,fNameCore) \
        inline static uint8_t fName( uint8_t row, \
        bool crossover=false, uint8_t initialSelection=0 ) { \
            return fNameCore( -1, nullptr, row, initialSelection, crossover, nullptr ); \
        } \
        inline static uint8_t fName( uint8_t captionRow, const char *caption, uint8_t optionsRow, \
        bool crossover=false, uint8_t initialSelection=0 ) { \
            return fNameCore( captionRow, caption, optionsRow, initialSelection, crossover, nullptr ); \
        } \
        static inline uint8_t fName( uint8_t row, \
        bool (*backgroundProcess)(void), bool crossover=false ) { \
            return fNameCore( -1, nullptr, row, 0, crossover, backgroundProcess ); \
        } \
        static inline uint8_t fName( uint8_t captionRow, const char *caption, uint8_t optionsRow, \
        bool (*backgroundProcess)(void), bool crossover=false ) { \
            return fNameCore( captionRow, caption, optionsRow, 0, crossover, backgroundProcess ); \
        }

    CHOOSER_VARIATIONS( chooseYesNo,       chooseYesNoCore       )
    CHOOSER_VARIATIONS( chooseYesNoCancel, chooseYesNoCancelCore )
    CHOOSER_VARIATIONS( chooseNoYes,       chooseNoYesCore       )
    CHOOSER_VARIATIONS( chooseNoYesCancel, chooseNoYesCancelCore )
    #undef CHOOSER_VARIATIONS 

    // // YES/NO - PLAIN
    // static inline uint8_t chooseYesNo( uint8_t row,
    // bool crossover = false, uint8_t initialSelection = 0 ) {
    //     return chooseYesNoCore( -1, nullptr, row, initialSelection, crossover, nullptr );
    //     // static const char *options[] = { "Yes", "No" };
    //     // chooser c( row, options, initialSelection, crossover );
    //     // return c.prompt();
    // }

    // // YES/NO - WITH MESSAGE
    // static inline uint8_t chooseYesNo( uint8_t captionRow, const char *caption, uint8_t optionsRow,
    // bool crossover = false, uint8_t initialSelection = 0 ) {
    //     return chooseYesNoCore( captionRow, caption, optionsRow, initialSelection, crossover, nullptr );
    //     // namespace ui = StarterPack::UserInterface;
    //     // if ( ui::LCD == nullptr ) return 0;
    //     // ui::LCD->printStrAtRow( captionRow, caption );
    //     // return chooseYesNo( optionsRow, crossover, initialSelection );
    // }

    // YES/NO - WITH BACKGROUND PROCESS
    // static inline uint8_t chooseYesNo( uint8_t row,
    // bool (*backgroundProcess)(void), bool crossover = false ) {
    //     return chooseYesNoCore( -1, nullptr, row, 0, crossover, backgroundProcess );
    //     // static const char *options[] = { "Yes", "No" };
    //     // chooser c( row, options, backgroundProcess, 0, crossover );
    //     // return c.prompt();
    // }

    // YES/NO - WITH MESSAGE / BACKGROUND PROCESS
    // static inline uint8_t chooseYesNo( uint8_t captionRow, const char *caption, uint8_t optionsRow,
    // bool (*backgroundProcess)(void), bool crossover = false ) {
    //     return chooseYesNoCore( captionRow, caption, optionsRow, 0, crossover, backgroundProcess );
    //     // namespace ui = StarterPack::UserInterface;
    //     // if ( ui::LCD == nullptr ) return 0;
    //     // ui::LCD->printStrAtRow( captionRow, caption );
    //     // return chooseYesNo( optionsRow, backgroundProcess, crossover );
    // }

/*
    // YES/NO/CANCEL
    static inline uint8_t chooseYesNoCancel( uint8_t row,
    bool crossover = false, uint8_t initialSelection = 0 ) {
        static const char *options[] = { "Yes", "No", "Cancel" };
        chooser c( row, options, initialSelection, crossover );
        return c.prompt();
    }

    // YES/NO/CANCEL - WITH MESSAGE
    static inline uint8_t chooseYesNoCancel( uint8_t captionRow, const char *caption, uint8_t optionsRow,
    bool crossover = false, uint8_t initialSelection = 0 ) {
        namespace ui = StarterPack::UserInterface;
        if ( ui::LCD == nullptr ) return 0;
        ui::LCD->printStrAtRow( captionRow, caption );
        return chooseYesNoCancel( optionsRow, crossover, initialSelection );
    }

    // YES/NO/CANCEL - WITH BACKGROUND PROCESS
    static inline uint8_t chooseYesNoCancel( uint8_t row,
    bool (*backgroundProcess)(void), bool crossover = false ) {
        static const char *options[] = { "Yes", "No", "Cancel" };
        chooser c( row, options, backgroundProcess, 0, crossover );
        return c.prompt();
    }

    // YES/NO/CANCEL - WITH MESSAGE / BACKGROUND PROCESS
    static inline uint8_t chooseYesNoCancel( uint8_t captionRow, const char *caption, uint8_t optionsRow,
    bool (*backgroundProcess)(void), bool crossover = false ) {
        namespace ui = StarterPack::UserInterface;
        if ( ui::LCD == nullptr ) return 0;
        ui::LCD->printStrAtRow( captionRow, caption );
        return chooseYesNoCancel( optionsRow, backgroundProcess, crossover );
    }

    // NO/YES
    static inline uint8_t chooseNoYes( uint8_t lineNo,
    bool crossover = false, uint8_t initialSelection = 0 ) {
        static const char *options[] = { "No", "Yes" };
        chooser c( lineNo, options, initialSelection, crossover );
        return c.prompt();
    }

    // NO/YES - WITH MESSAGE
    static inline uint8_t chooseNoYes( uint8_t captionRow, const char *caption, uint8_t optionsRow,
    bool crossover = false, uint8_t initialSelection = 0 ) {
        namespace ui = StarterPack::UserInterface;
        if ( ui::LCD == nullptr ) return 0;
        ui::LCD->printStrAtRow( captionRow, caption );
        return chooseNoYes( optionsRow, crossover, initialSelection );
    }

    // NO/YES - WITH BACKGROUND PROCESS
    static inline uint8_t chooseNoYes( uint8_t row,
    bool (*backgroundProcess)(void), bool crossover = false ) {
        static const char *options[] = { "No", "Yes" };
        chooser c( row, options, backgroundProcess, 0, crossover );
        return c.prompt();
    }

    // NO/YES - WITH MESSAGE / BACKGROUND PROCESS
    static inline uint8_t chooseNoYes( uint8_t captionRow, const char *caption, uint8_t optionsRow,
    bool (*backgroundProcess)(void), bool crossover = false ) {
        namespace ui = StarterPack::UserInterface;
        if ( ui::LCD == nullptr ) return 0;
        ui::LCD->printStrAtRow( captionRow, caption );
        return chooseNoYes( optionsRow, backgroundProcess, crossover );
    }

    // NO/YES/CANCEL
    static inline uint8_t chooseNoYesCancel( uint8_t lineNo,
    bool crossover = false, uint8_t initialSelection = 0 ) {
        namespace ui = StarterPack::UserInterface;
        static const char *options[] = { "No", "Yes", "Cancel" };
        chooser c( lineNo, options, initialSelection, crossover );
        return c.prompt();
    }

    // NO/YES/CANCEL - WITH MESSAGE
    static inline uint8_t chooseNoYesCancel( uint8_t captionRow, const char *caption, uint8_t optionsRow,
    bool crossover = false, uint8_t initialSelection = 0 ) {
        namespace ui = StarterPack::UserInterface;
        if ( ui::LCD == nullptr ) return 0;
        ui::LCD->printStrAtRow( captionRow, caption );
        return chooseNoYesCancel( optionsRow, crossover, initialSelection );
    }
*/

//
// LONG MESSAGE
//

    // https://solarianprogrammer.com/2016/11/28/cpp-passing-c-style-array-with-size-information-to-function/
    template<size_t msgCount>
    static uint8_t showMultiLineMsg( const char* (&msg)[msgCount], bool (*backgroundProcess)(void) = nullptr ) {
        namespace ui = StarterPack::UserInterface;

        if ( !ui::hasScreen() ) return 0;

        ui::LCD->clear();
        int lineNo = 0;

        ui::waitUntilNothingIsPressed();

        while ( true ) {

            // display text
            int tmp = lineNo;
            for( int i = 0 ; i < ui::LCD->maxRows ; i++ ) {
                if ( tmp < msgCount )
                    ui::LCD->printStrAtRow( i, msg[tmp] );
                else
                    ui::LCD->clearRow( i );
                tmp++;
            }
            if ( lineNo < msgCount - ui::LCD->maxRows ) {
                // not last line, show marker
                ui::LCD->printAt( 18, ui::LCD->maxRows-1, " \x7E" );
            }
            ui::LCD->displayAll();

            if ( ui::KeyHandler != nullptr ) {
                uint8_t key = ui::getRepeatingKey();
                if ( key == ui::kENTER || key == ui::kESCAPE ) {
                    ui::waitUntilNothingIsPressed();
                    return key;
                }
                else if ( key == ui::kUP || key == ui::kLEFT ) {
                    if ( lineNo > 0 ) lineNo--;
                } else if ( key == ui::kDOWN || key == ui::kRIGHT ) {
                    if ( lineNo < msgCount - ui::LCD->maxRows ) lineNo++;
                }
            }

            if ( backgroundProcess != nullptr ) {
                if ( !backgroundProcess() )
                    return 0;
            }
        }
    }

//
// BLINK
//

    class blink {

            uint32_t blinkTime = millis();
            bool blinkMode = false;

            // LCDInterface *lcd;
            uint8_t col = 0;
            uint8_t row = 0;

        public:

            blink() {}

            blink( uint8_t col, uint8_t row ) {
                this->col = col;
                this->row = row;
            }

            void update() {
                namespace ui = StarterPack::UserInterface;
                if ( !ui::hasScreen() ) return;
                uint32_t now = millis();
                if ( now - blinkTime >= 1000 ) {
                    ui::LCD->writeAt( col, row, blinkMode ? 0xA5 : ' ' );
                    blinkMode = !blinkMode;
                    blinkTime = now;
                    // ui::LCD->displayAll();
                }
            }

            void setPosition( uint8_t col, uint8_t row ) {
                namespace ui = StarterPack::UserInterface;
                if ( !ui::hasScreen() ) return;
                if ( !blinkMode ) {
                    // false means dot is currently displayed
                    ui::LCD->writeAt( col, row, blinkMode ? 0xA5 : ' ' );
                    blinkMode = !blinkMode;
                    // ui::LCD->displayAll();
                }
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

}

}
