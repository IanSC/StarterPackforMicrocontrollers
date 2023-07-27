#pragma once
#include <inttypes.h>
#include <string.h>

#include <Utility/spMacros.h>
#include <spSerial.h>

// #define DEBUG_TRACE(x)   x;
#define DEBUG_TRACE(x)   ;

namespace StarterPack {

class WindowedText {
        
    private:
    
        // handle scrolling and positioning to display
        // a longer string through a narrower window

        //   01234...
        //   the quick brown fox jumped over the lazy dog
        //          |<--- windowSize --->|
        //          ^
        //          startPos

    //
    // CURSOR POSITION
    //
    public:

        // allowed position of cursor within buffer
        // eg. move anywhere
        //     stay only at the end, and need to backspace

        enum cursorRangeOptions : uint8_t {
            //fromDataStart      = 1, // if no data, no cursor ???

            fromBufferStart    = 2,     // MIN: position 0
            fromDataEnd        = 4,     // MIN: on last character
            fromDataEndPlusOne = 8,     // MIN: one space after last character

            toDataEnd          = 16,    // MAX: on last character
            toDataEndPlusOne   = 32,    // MAX: one space after last character
            toBufferEnd        = 64,    // MAX: last character in buffer
            toBufferEndPlusOne = 128,   // MAX: one space after end of buffer

            WithinBuffer        = fromBufferStart | toBufferEnd,        // can move freely within buffer
            WithinBufferPlusOne = fromBufferStart | toBufferEndPlusOne, // can move freely within buffer + 1            
            EndOfDataPlusOne    = fromDataEndPlusOne | toDataEndPlusOne // restrict to position after data
        };
        CLASS_ENUM_MANIPULATION( cursorRangeOptions )

    // private:

        // allowed cursor position relative to buffer
        cursorRangeOptions cursorRange = WithinBuffer;

    // private:

        char    *buffer;
        uint8_t maxBufferSize;      // maximum buffer size, including NULL terminator
        uint8_t currentLength = 0;  // current length of buffer
        uint8_t cursorPos;          // cursor position in buffer

        uint8_t windowSize;         // size of window to display the buffer
        uint8_t windowIndex;        // offset in buffer to print on start position of window
        
    public:

        WindowedText( char *buffer, uint8_t bufferSize, uint8_t windowSize,
        cursorRangeOptions cursorRange = cursorRangeOptions::WithinBufferPlusOne,
        int8_t cursorPos = -1, int8_t startPos = 0 ) {
            this->buffer = buffer;
            this->maxBufferSize = bufferSize;
            this->windowSize = windowSize;
            this->cursorRange = cursorRange;
            this->cursorPos = cursorPos;
            this->windowIndex = startPos;
            if ( cursorPos == -1 )
                this->cursorPos = strlen( buffer );
            else
                this->cursorPos = cursorPos;
            currentLength = strlen( buffer );
            recompute();
            constrictCursor();
        }

        void reset() {
            cursorPos = strlen( buffer );
            currentLength = strlen( buffer );
            recompute();
            constrictCursor();
        }

        //
        // CHARACTER
        //

        inline char *charPtrAtCursor() {
            return charPtrAt( cursorPos );
        }

        char *charPtrAt( uint8_t position ) {
            if ( position < 0 || position > currentLength )
                return nullptr;
            return buffer+position;
        }

        char charAtCursor() {
            return buffer[cursorPos];
        }

        char charAt( uint8_t position ) {
            return buffer[position];
        }

        bool setCharAtCursor( char ch ) {
            char *ptr = charPtrAtCursor();
            if ( ptr == nullptr ) return false;
            *ptr = ch;
            return true;
        }

        inline uint8_t length() {
            return currentLength;
        }
        
        inline char *string() {
            return buffer;
        }

        //
        // STRING
        //

        bool trimEnd() {
            bool modified = false;
            char *ptr = buffer+currentLength-1;
            while( *ptr == ' ' && ptr >= buffer ) {
                *ptr = 0;
                ptr--;
                modified = true;
            }
            return modified;
        }

    //
    // CURSOR MOVEMENT
    //    
    public:

        bool cursorForward() {
            uint8_t orig = cursorPos;
            cursorPos++;
            constrictCursor();
            if ( cursorPos != orig ) {
                // can move cursor

                if ( cursorPos > currentLength ) {
                    // moved beyond end of data
                    // replace with ' ' mising data
                    buffer[cursorPos] = 0;
                    char *p = buffer+cursorPos-1;
                    while( *p == 0 ) {
                        *p = ' ';
                        p--;
                    }
                    currentLength = strlen( buffer );
                }
                recompute();
                return true;
            }
            return false;
        }

        bool cursorBackward() {
            uint8_t orig = cursorPos;
            if ( cursorPos > 0 ) {
                cursorPos--;
                constrictCursor();
                if ( cursorPos != orig ) {
                    recompute();
                    return true;
                }
            }
            return false;
        }

    //
    // CURSOR LOGIC
    //
    public:

        void constrictCursor() {
            
            // uint8_t bLength = strlen( buffer );

            uint8_t min = 0;
            if ( ( cursorRange & fromBufferStart ) != 0 )
                min = 0;
            else if ( ( cursorRange & fromDataEnd ) != 0 )
                min = currentLength-1;
            else if ( ( cursorRange & fromDataEndPlusOne ) != 0 )
                min = currentLength;

            uint8_t max = 0;
            if ( ( cursorRange & toDataEnd ) != 0 )
                max = currentLength-1;
            else if ( ( cursorRange & toDataEndPlusOne ) != 0 )
                max = currentLength;
            else if ( ( cursorRange & toBufferEnd ) != 0 )
                max = maxBufferSize-1;
            else if ( ( cursorRange & toBufferEndPlusOne ) != 0 )
                max = maxBufferSize;

            if ( cursorPos < min ) {
                cursorPos = min;
                DEBUG_TRACE( SerialPrintfln( "cursor min = %d", min ) )
            }
            if ( cursorPos > max ) {
                cursorPos = max;
                DEBUG_TRACE( SerialPrintfln( "cursor max = %d", max ) )
            }
        }

        inline uint8_t cursorPositionOnWindow() {
            // cursor position relative to window
            //                0123456789
            // data           abcdefghijk
            // startPos  = 3     ^
            // cursorPos = 7       ^
            // window            #####   5-3 = 2
            //                   01234
            return cursorPos - windowIndex;
        }

        //
        // RECOMPUTE
        //

        void recompute() {

            // adjust cursor position validity
            // ideal startPos so cursor is visible on window

            if ( cursorPos < 0 ) {
                DEBUG_TRACE( Serial.println( "   zero cursor" ) )
                cursorPos = 0;
            }

            if ( cursorPos > currentLength ) {
                //                01234567
                // data           abcdef    len=6
                // cursorPos = 7         ^
                // cursorPos = 6        ^    ok
                DEBUG_TRACE( Serial.println( "   cursor to end" ) )
                cursorPos = currentLength;
            }

            // remove blanks beyond cursor position
            // maintain character at cursor
            if ( cursorPos < currentLength-1 ) {
                //         0123456789
                //         abcde_____  len=10
                // cursor         ^
                //         abcde___
                for( int i = currentLength-1 ; i > cursorPos ; i-- ) {
                    if ( *(buffer+i) == ' ' ) {
                        *(buffer+i) = 0;
                        DEBUG_TRACE( SerialPrintfln( "   set to null %d", i ) )
                    } else
                        break;
                }
                // length changed
                currentLength = strlen( buffer );
            }

            // cursor must be visible within window
            uint8_t endVisible = windowIndex + windowSize - 1; // 5+6-1 = 10
            if ( cursorPos < windowIndex ) {
                //                 0123456789012345
                // data            abcdefghijklmnop
                // startPos  = 5        ^
                // window    = 6        ######       size=6
                // cursorPos = 2     ^
                // AFTER:
                // window            ######
                // startPos  = 2     ^
                windowIndex = cursorPos;
                DEBUG_TRACE( Serial.println( "   before start" ) )
            }
            if ( cursorPos > endVisible ) {
                //                 0123456789012345
                // data            abcdefghijklmnop
                // startPos  = 5        ^
                // window    = 6        ######       size=6
                // endVis    = 10            ^
                // cursorPos = 13               ^
                // AFTER:
                // window                  ######    13-6+1=8
                // startPos  = 8           ^
                windowIndex = cursorPos - windowSize + 1;
                DEBUG_TRACE( Serial.println( "   after end" ) )
            }
            if ( windowIndex < 0 ) {
                windowIndex = 0;
                DEBUG_TRACE( Serial.println( "   start is negative" ) )
            }

            // if there are hidden text on the left and
            // cursor is not at the rightmost yet
            // adjust to make visible as much text as possible
            while( windowIndex > 0 && cursorPos-windowIndex < windowSize-1 ) {
                // BEFORE:        0123456789012345
                // data           abcdefg
                // startPos  = 3     ^
                // cursorPos = 6        ^
                // window            ######   size=6

                // AFTER:         0123456789012345
                // data           abcdefg
                // startPos  = 1   ^
                // cursorPos = 6        ^
                // window          ######
                windowIndex--;
                DEBUG_TRACE( Serial.println( "   visible adj" ) )
            }

            // if there are hidden text on the left and
            // cursor is on the leftmost adjust
            // otherwise cursor will overlap with left hidden marker
            if ( windowIndex > 0 && cursorPos==windowIndex ) {
                // left marker
                windowIndex--;
                DEBUG_TRACE( Serial.println( "   left marker adj" ) )
            }

            // if there are hidden text on the right and
            // cursor happens to be on the hidden marker position
            // adjust it
            uint8_t userLength = strlen( buffer+windowIndex );
            if ( userLength >= windowSize ) {
                // BEFORE:        0123456789
                // data           abcdefghijk
                // startPos  = 3     ^
                // cursorPos = 7         ^
                // window            #####   size=5

                // AFTER:         0123456789
                // data           abcdefghijk
                // startPos  = 4      ^
                // cursorPos = 7         ^
                // window             #####  size=5
                if ( cursorPos >= windowIndex+windowSize-1 ) {
                    windowIndex++;
                    DEBUG_TRACE( Serial.println( "   right marker adj" ) )
                }
            }
        }

    //
    // INSERT CHARACTER
    //
    public:

        inline bool insertAtCursor( char ch = ' ' ) {
            // insert character on cursor
            return insertAt( ch, cursorPos );
        }

        bool insertAt( const char ch, const uint8_t position ) {
            // insert character on given position
            if ( position < 0 || position > currentLength || !canAddChar() )
                return false;

            //                012345678901
            // data           abcdefg*       len=7
            // position  = 3     ^
            // copy/move          defg*
            // copy from             ^^      copy null terminator *
            // copy to           ^^          copy 7-3+1 = 5 chars

            // bool insertNul = ( position == bLength );

            char *ptr = buffer + currentLength;
            int8_t cnt = currentLength - position + 1;
            while ( cnt > 0 ) {
                *(ptr+1) = *ptr;
                ptr--;
                cnt--;
            }
            buffer[position] = ch;

            currentLength = strlen( buffer );
            // bLength++;
            // if ( strlen(buffer) != bLength ) {
            //     Serial.println( "*****");
            //     Serial.println( strlen(buffer));
            //     Serial.println( bLength );
            // }

            // if ( insertNul )
            //     buffer[bLength] = 0;
            constrictCursor();
            recompute();
            DEBUG_TRACE( Serial.println( "inserted" ) )
            return true;
        }

    //
    // DELETE CHARACTER
    //
    private:

        bool deleteAtCore( const uint8_t position ) {
            if ( position < 0 || position >= currentLength )
                return false;
            char *ptr = buffer + position;
            while ( *ptr != 0 ) {
                *ptr = *(ptr+1);
                ptr++;
            }
            // bLength--;
            currentLength = strlen( buffer );
            return true;
        }

    public:

        bool backspace() {
            // delete character before cursor
            if ( deleteAtCore( cursorPos-1 ) ) {
                cursorPos--;
                constrictCursor();
                recompute();
                return true;
            }
            return false;
        }

        inline bool deleteAtCursor() {
            // delete character on cursor
            return deleteAt( cursorPos );
        }

        bool deleteAt( const uint8_t position ) {
            if ( deleteAtCore( position ) ) {
                constrictCursor();
                recompute();
                return true;
            }
            return false;
        }

    //
    // EVAL
    //
    public:

        enum cPos {
            first,          // first charcter
            last,           // last character
            beforeFirst,    // before 1st character
            rightAfterLast, // right after last charcter (at null terminator)
            wayAfterLast,   // past [rightAfterLast]
            middle          // somewhere between 1st/last character
        };

        cPos evalPosition() {
            //             first .. middle .. last
            //             |                  |
            //             v                  v
            // buffer      abcdefghijklmnopqrst    len=20
            // cursorPos   012345678901234567890
            //            ^                    ^
            //         .. |                    |
            //  beforeFirst       rightAfterLast ... wayAfterLast
            if ( cursorPos < 0 ) return beforeFirst;
            if ( cursorPos == 0 ) return first;
            // auto bLength = strlen( buffer );
            if ( cursorPos == currentLength ) return rightAfterLast;
            if ( cursorPos > currentLength ) return wayAfterLast;
            if ( cursorPos == currentLength-1 ) return last;
            return middle;
        }

        bool canAddChar() {
            // bufLen = 10, less null, 9
            // if less than 9 can add more chars
            // auto bLength = strlen( buffer );
            return ( currentLength < maxBufferSize-1 );
        }

        bool canModifyCharAtCursor() {

            // check if data on cursor can be modified
            // if exactly on null terminator, check if there is
            //    enough buffer space, effectively inserting another
            //    character but at the end

            //                 0123456
            // data            abcd*     len=4
            // cursorPos = 3      ^      okay
            // cursorPos = 5        ^    not okay
            // cursorPos = 4       ^
            // data            abcd**    prepare

            if ( cursorPos > currentLength ) return false;
            if ( cursorPos < currentLength ) return true;

            // exactly at null terminator
            if ( !canAddChar() ) return false; // buffer full

            buffer[cursorPos+1] = 0;
            DEBUG_TRACE( Serial.println( "   modify at cursor, null added" ) )
            return true;
        }

        bool modifyCharAtCursor( char replaceNullWith ) {
            // if data on cursor is null,
            //    replace with ch
            // otherwise maintain current contents
            if ( canModifyCharAtCursor() ) {
                if ( buffer[cursorPos] == 0 ) {
                    buffer[cursorPos] = replaceNullWith;
                    DEBUG_TRACE( SerialPrintfln( "   set to '%c'", replaceNullWith ) )
                }
                currentLength = strlen( buffer );
                recompute();
                return true;
            }
            return false;
        }

    //
    // UI
    //
    public:

        inline char *stringToDisplay() {
            return buffer + windowIndex;
        }

        uint8_t spaceToClear() {
            uint8_t toDisplayLength = strlen( buffer+windowIndex );
            if ( toDisplayLength < windowSize )
                return windowSize - toDisplayLength;
            return 0;
        }

        inline bool isLeftObscured() {
            return windowIndex != 0;
        }

        bool isRightObscured() {
            //            012345678
            // userData = 123456789_  = 9
            // startOff =   ^
            //              #####     = 5
            uint8_t displayedLen = strlen( buffer+windowIndex );
            return ( displayedLen > windowSize );
        }

    };

    //
    // TEST
    //

        void TEST_WindowedText() {
            char buffer[20];
            buffer[0] = 0;

            WindowedText w( buffer, 20, 10 );

            Serial.println();
            SerialPrintfln( "string = %s}}}", w.string() );
            SerialPrintfln( "length = %d", w.length() );
            SerialPrintfln( "cursorPos = %d", w.cursorPos );

            Serial.println();
            SerialPrintfln( "inserted = %d", w.insertAtCursor( 'A' ) );
            SerialPrintfln( "string = %s}}}", w.string() );
            SerialPrintfln( "length = %d", w.length() );
            SerialPrintfln( "cursorPos = %d", w.cursorPos );

            Serial.println();
            SerialPrintfln( "inserted = %d", w.insertAtCursor( 'B' ) );
            SerialPrintfln( "string = %s}}}", w.string() );
            SerialPrintfln( "length = %d", w.length() );
            SerialPrintfln( "cursorPos = %d", w.cursorPos );

            Serial.println();
            SerialPrintfln( "inserted = %d", w.insertAt( 'C', 2 ) );
            SerialPrintfln( "string = %s}}}", w.string() );
            SerialPrintfln( "length = %d", w.length() );
            SerialPrintfln( "cursorPos = %d", w.cursorPos );

            Serial.println();
            SerialPrintfln( "c forward = %d", w.cursorForward() );
            SerialPrintfln( "cursorPos = %d", w.cursorPos );

            Serial.println();
            SerialPrintfln( "c forward = %d", w.cursorForward() );
            SerialPrintfln( "cursorPos = %d", w.cursorPos );

        }

}

#undef DEBUG_TRACE
