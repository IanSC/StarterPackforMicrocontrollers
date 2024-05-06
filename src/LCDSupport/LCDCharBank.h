#pragma once
#include <inttypes.h>

#include <LCD/LCDInterface.h>
// #include <spUtility.h>
#include <Utility/spVector.h>

// #define DEBUG_DIAGNOSE
// #define DBG_SPC "<charbank> "
#include <dbgDefines.h>

namespace StarterPack::spLCD {

class LCDCharBank {

    // dbID     - index where bitmaps are saved in the database
    // charSlot - char number when loaded into LCD's custom character hardware
    //
    // ex. save X into db, dbID=10
    //     load X from db and put into charSlot=3 of LCD
    //        db[10] ---> LCD.bitmap[3]
    //     to use: LCD.write(3)
    //
    // Style 1: automatically find slot used
    //     slower since need to lookup,
    //        but no need to track which character slot
    //     cBank.addtoDb( 10, bitmap );
    //     cBank.loadCharsToSlot0( 10 ); // bitmap loaded starting at position 0
    //                                   // use cBank.loadFromDb( 10, 12, 20, ... ) to load multiple bitmaps
    //     cBank.write( 10 );            // will find actual character to write to LCD screen
    //                                   // in this case 0
    //
    // Style 2: user tracks character offset
    //     cBank.addtoDb( 10, bitmap );
    //     cBank.loadChars( 5, 10 ); // bitmap loaded to position 5
    //     LCD.write( 5 );           // must write slot used to LCD screen

    protected:

        struct data {
            uint8_t dbID;
            const uint8_t * charmap;
        };
        spVector<data> head;

        StarterPack::LCDInterface * lcd = nullptr;

        // dbID of characters loaded into LCD hardware
        // max 8 slot in LCD buffer
        static constexpr uint8_t LCD_BITMAP_BUFFER_COUNT = 8;
        int8_t charMap[LCD_BITMAP_BUFFER_COUNT] = { -1, -1, -1, -1, -1, -1, -1, -1 };

    public:

        LCDCharBank( StarterPack::LCDInterface & lcd ) { this->lcd = &lcd; }

        LCDCharBank( StarterPack::LCDInterface * lcd ) { this->lcd = lcd; }

        void addToDb( uint8_t dbID, const uint8_t charmap[] ) {
            // add bitmap to databank
            auto d = new data();
            d->dbID = dbID;
            d->charmap = &(charmap[0]);
            head.insert( d );
        }

        void addToDb( uint8_t dbID, const char * charmap ) {
            // add bitmap to databank
            auto d = new data();
            d->dbID = dbID;
            d->charmap = (const uint8_t *) charmap;
            head.insert( d );
        }

    public:

        // load characters to LCD screen
        inline bool loadChars( int charSlotStart, int dbID1 ) { return loadN( charSlotStart, 1, dbID1 ); }
        inline bool loadChars( int charSlotStart, int dbID1, int dbID2 ) { return loadN( charSlotStart, 2, dbID1, dbID2 ); }
        inline bool loadChars( int charSlotStart, int dbID1, int dbID2, int dbID3 ) { return loadN( charSlotStart, 3, dbID1, dbID2, dbID3 ); }
        inline bool loadChars( int charSlotStart, int dbID1, int dbID2, int dbID3, int dbID4 ) { return loadN( charSlotStart, 4, dbID1, dbID2, dbID3, dbID4 ); }
        inline bool loadChars( int charSlotStart, int dbID1, int dbID2, int dbID3, int dbID4, int dbID5 ) { return loadN( charSlotStart, 5, dbID1, dbID2, dbID3, dbID4, dbID5 ); }
        inline bool loadChars( int charSlotStart, int dbID1, int dbID2, int dbID3, int dbID4, int dbID5, int dbID6 ) { return loadN( charSlotStart, 6, dbID1, dbID2, dbID3, dbID4, dbID5, dbID6 ); }
        inline bool loadChars( int charSlotStart, int dbID1, int dbID2, int dbID3, int dbID4, int dbID5, int dbID6, int dbID7 ) { return loadN( charSlotStart, 7, dbID1, dbID2, dbID3, dbID4, dbID5, dbID6, dbID7 ); }
        inline bool loadChars( int charSlotStart, int dbID1, int dbID2, int dbID3, int dbID4, int dbID5, int dbID6, int dbID7, int dbID8 ) { return loadN( charSlotStart, 8, dbID1, dbID2, dbID3, dbID4, dbID5, dbID6, dbID7, dbID8 ); }

        inline bool loadCharsToSlot0( int dbID1 ) { return loadN( 0, 1, dbID1 ); }
        inline bool loadCharsToSlot0( int dbID1, int dbID2 ) { return loadN( 0, 2, dbID1, dbID2 ); }
        inline bool loadCharsToSlot0( int dbID1, int dbID2, int dbID3 ) { return loadN( 0, 3, dbID1, dbID2, dbID3 ); }
        inline bool loadCharsToSlot0( int dbID1, int dbID2, int dbID3, int dbID4 ) { return loadN( 0, 4, dbID1, dbID2, dbID3, dbID4 ); }
        inline bool loadCharsToSlot0( int dbID1, int dbID2, int dbID3, int dbID4, int dbID5 ) { return loadN( 0, 5, dbID1, dbID2, dbID3, dbID4, dbID5 ); }
        inline bool loadCharsToSlot0( int dbID1, int dbID2, int dbID3, int dbID4, int dbID5, int dbID6 ) { return loadN( 0, 6, dbID1, dbID2, dbID3, dbID4, dbID5, dbID6 ); }
        inline bool loadCharsToSlot0( int dbID1, int dbID2, int dbID3, int dbID4, int dbID5, int dbID6, int dbID7 ) { return loadN( 0, 7, dbID1, dbID2, dbID3, dbID4, dbID5, dbID6, dbID7 ); }
        inline bool loadCharsToSlot0( int dbID1, int dbID2, int dbID3, int dbID4, int dbID5, int dbID6, int dbID7, int dbID8 ) { return loadN( 0, 8, dbID1, dbID2, dbID3, dbID4, dbID5, dbID6, dbID7, dbID8 ); }

    protected:

        inline bool loadCharCore( int dbID, int & charSlot ) {
            // if ( lcd == nullptr ) return false;
            data * p = find( dbID );
            if ( p != nullptr ) {
                if ( charSlot >= LCD_BITMAP_BUFFER_COUNT ) charSlot = 0; // 0 to 7 only
                charMap[charSlot] = dbID;
                lcd->createChar( charSlot, p->charmap );
                _DBG( "slot " ); DBG( charSlot ); DBG( " = " ); DBG_( dbID );
                charSlot++;
                return true;
            }
            return false;
        }

        bool loadN( int charSlot, int argCount, ... ) {
            // load N chars into LCD, with starting index
            // ex. loadN( 6, 2, A, B )
            //     load 2 chars
            //     load A into LCD.bitmap[6]
            //     load B into LCD.bitmap[6+1]

            if ( lcd == nullptr ) return false;

            va_list valist;
            va_start( valist, argCount );

            for ( int i = 0 ; i < argCount; i++ ) {
                auto dbID = va_arg( valist, int );
                loadCharCore( dbID, charSlot );
            }
            va_end( valist );
            return true;
        }

    protected:

        data * find( uint8_t dbID ) {
            // return bitmap of character
            data * p = head.getFirst();
            while( p != nullptr ) {
                if ( p->dbID == dbID )
                    return p;
                p = head.getNext();
            }
            return nullptr;
        }

        int8_t findLoadedSlot( uint8_t dbID ) {
            // return true if char is loaded
            for( int i=0 ; i<LCD_BITMAP_BUFFER_COUNT ; i++ ) {
                if ( charMap[i] == dbID )
                    return i;
            }
            return -1;
        }
        
    public:

        bool exists( uint8_t dbID ) {
            // return true if char is in database
            return ( find( dbID ) != nullptr );
        }

        bool isLoaded( uint8_t dbID ) {
            // return true if char is loaded into LCD screen
            return ( findLoadedSlot( dbID ) != -1 );
        }

        void write( uint8_t dbID ) {
            // write character to LCD at current cursor position, if loaded
            if ( lcd == nullptr ) return;
            auto p = findLoadedSlot( dbID );
            if ( p == -1 )
                lcd->write( ' ' );
            else
                lcd->write( p );
        }

        void writeAt( uint8_t col, uint8_t row, uint8_t dbID ) {
            // write character to LCD at given position
            lcd->setCursor( col, row );
            write( dbID );
        }

};

}

#include <dbgDefinesOff.h>
