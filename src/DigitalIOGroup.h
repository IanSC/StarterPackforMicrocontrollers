#pragma once
#include <DigitalIO.h>
#include <UserInputDeviceMulti.h>

namespace StarterPack {

class DigitalIOGroup : public UserInputDeviceMulti {

    public:

        ~DigitalIOGroup() {
            delete_NextLinkedList( dIOEntry_List );
            // deleteIOList();
        }

    private:

        struct dIOEntry {
            DigitalIO *dIO = nullptr;
            uint8_t key;
            dIOEntry *next = nullptr;
        };

        dIOEntry *dIOEntry_List = nullptr;

        // void deleteIOList() {
        //     if ( dIOEntry_List == nullptr ) return;
        //     while( dIOEntry_List->next != nullptr ) {
        //         // slow but avoids recursion
        //         dIOEntry *p1 = dIOEntry_List;
        //         dIOEntry *p2 = p1->next;
        //         while ( p2->next != nullptr ) {
        //             p1 = p2;
        //             p2 = p2->next;
        //         }
        //         delete p2;
        //         p1->next = nullptr;
        //     }
        //     delete dIOEntry_List;
        // }

    public:

        // DigitalIOGroup() {}

        void add( DigitalIO &dIO, uint8_t key ) {
            dIOEntry *e = new dIOEntry();
            e->dIO = &dIO;
            e->key = key;
            if ( dIOEntry_List == nullptr )
                dIOEntry_List = e;
            else {
                dIOEntry *p = dIOEntry_List;
                while ( p->next != nullptr ) p = p->next;
                p->next = e;
            }
        }

    //
    // DEBOUNCER
    //     
    public:

        void flagWaitForKeyupSpecific( uint8_t key ) { debouncer.flagWaitForKeyup(); }
        void flagWaitForKeyupMulti( char *keysPressed ) { debouncer.flagWaitForKeyup(); }

    //
    // READ DEVICE
    //
    protected:

        static const uint8_t MAX_KEYS = 5;
        char keysPressed[MAX_KEYS+1];

    public:

        char *readMappedKeyList() {
            // record each key pressed into buffer
            uint8_t count = 0;
            dIOEntry *p = dIOEntry_List;
            while ( p != nullptr ) {
                if ( p->dIO->getContinuousKey() ) {
                    keysPressed[count] = p->key;
                    count++;
                    if ( count >= MAX_KEYS )
                        break;
                }
                p = p->next;
            }
            keysPressed[count] = 0;
            return keysPressed;
        }
        
    //
    // KEYS
    //

        // UserInputDigital( DigitalIO *okayKey,
        // DigitalIO *leftKey, DigitalIO *rightKey, DigitalIO *upKey, DigitalIO *downKey ) {
        //     ioDigital[SLOT_OKAY ] = okayKey;
        //     ioDigital[SLOT_LEFT ] = leftKey;
        //     ioDigital[SLOT_RIGHT] = rightKey;
        //     ioDigital[SLOT_UP   ] = upKey;
        //     ioDigital[SLOT_DOWN ] = downKey;
        //     // checkCommonKeys();
        // }

        // UserInputDigital( DigitalIO *okayKey, DigitalIO *leftAndUpKey, DigitalIO *rightAndDownKey ) {
        //     ioDigital[SLOT_OKAY ] = okayKey;
        //     ioDigital[SLOT_LEFT ] = leftAndUpKey;
        //     ioDigital[SLOT_RIGHT] = rightAndDownKey;
        //     ioDigital[SLOT_UP   ] = leftAndUpKey;
        //     ioDigital[SLOT_DOWN ] = rightAndDownKey;
        //     commonUpLeft = true;
        //     commonDownRight = true;
        //     // checkCommonKeys();
        // }

        // UserInputDigital( DigitalIO &dIO, Keys key ) {
        //     addKey( dIO, key );
        // }

        // void addKey( DigitalIO &dIO, Keys key ) {
        //     uint8_t numberOfBits = countBits( static_cast<uint16_t>( key ) );
        //     if ( numberOfBits != 1 ) return;
        //     // https://stackoverflow.com/a/31393298
        //     uint8_t bitPosition = __builtin_ctzl( key );
        //     if ( bitPosition > MAX_KEYS ) return;
        //     ioDigital[bitPosition] = &dIO;
        // }

    // private:

        // void checkCommonKeys() {
        //     commonUpLeft    = ( ioDigital[SLOT_LEFT] == ioDigital[SLOT_UP] );
        //     commonDownRight = ( ioDigital[SLOT_RIGHT] == ioDigital[SLOT_DOWN] );
        // }

    //
    //
    //
    /*
    public:

        Keys getKeyDown( Keys allowedKeys = Keys::All ) {
            Keys input = Keys::None;
            for( int i = 0 ; i < MAX_KEYS ; i++ ) {
                if ( ioDigital[i] != nullptr && ioDigital[i]->getKeyDown() )
                    input = static_cast<Keys>( input | (1<<i) );
            }
            return processKeysCore( allowedKeys, input, false );
        }

        Keys getContinuousKey( Keys allowedKeys = Keys::All ) {
            Keys input = Keys::None;
            for( int i = 0 ; i < MAX_KEYS ; i++ ) {
                if ( ioDigital[i] != nullptr && ioDigital[i]->getContinuousKey() )
                    input = static_cast<Keys>( input | (1<<i) );
            }
            return processKeysCore( allowedKeys, input, false );
        }

        Keys getRepeatingKey( bool okayKeyAlsoRepeating, Keys allowedKeys = Keys::All ) {
            return getRepeatingKey( allowedKeys, okayKeyAlsoRepeating );
        }

        Keys getRepeatingKey( Keys allowedKeys = Keys::All, bool okayKeyAlsoRepeating = false ) {                
            // if repeating from DigitalIO is used, cannot do diagonal repeating
            // so get status and combine here
            Keys input = Keys::None;
            for( int i = 0 ; i < MAX_KEYS ; i++ ) {
                if ( ioDigital[i] != nullptr && ioDigital[i]->getContinuousKey() )
                    input = static_cast<Keys>( input | (1<<i) );
            }

            Keys r = processKeysCore( allowedKeys, input, false );
            r = static_cast<Keys>( debouncer.getRepeatingKey( r ) );
            if ( r == Okay && !okayKeyAlsoRepeating )
                ioDigital[SLOT_OKAY]->flagWaitForKeyup();
            return r;
        }
*/
/*
    private:

        void flagWaitForKeyup( uint16_t input ) {
            // input = 0b000DURLO
            //    O - if 1, send flag to okay button
            //    L - if 1, send flag to left button
            //    ...
            for( int i = 0 ; i < MAX_KEYS ; i++ ) {
                if ( (input & 1) != 0 && ioDigital[i] != nullptr )
                    ioDigital[i]->flagWaitForKeyup();
                input >>= 1;
            }
        }
*/

};

}
