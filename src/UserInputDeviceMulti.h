#pragma once
#include <UserInputDevice.h>
#include <spUtility.h>

namespace StarterPack {

class UserInputDeviceMulti : public UserInputDevice {

    public:

        ~UserInputDeviceMulti() {
            delete_NextLinkedList( combinedList_2Keys );
            // deleteCombinedKeys();
        }

    //
    // KEYS
    //
    protected:

        uint8_t allenKey( char *allowedKeys = nullptr ) {
            // uint32_t keyBitmap = readBitmapRaw();
            // char *keyList = getKeymapsForMultiKey32( keyBitmap );
            // return processKeysCore( allowedKeys, keyList, false );
            return processKeysCore( allowedKeys, readMappedKeyList(), false );
        }

    public:

// uint8_t prevReadMappedKey = -1;
// void reportReadMappedKey( uint8_t v ) {
//     if ( prevReadMappedKey != v ) {
//         Serial.printf( "dm readMappedKey=%d\n", v );
//         prevReadMappedKey = v;
//     }
// }


        // return mapped key number
        uint8_t readMappedKey() override {
            // get 1st and only key
            char *keys = readMappedKeyList();
            if ( keys == nullptr ) {
// reportReadMappedKey( 255 );
                return 0;
            }
            uint8_t l = strlen( keys );
            if ( l != 1 ) {
// reportReadMappedKey( 0 );
                return 0;
            }
// reportReadMappedKey( keys[0] );
            return keys[0];
        }

        // return string containing all mapped key values
        // virtual char *readMappedKeyList() override = 0;

        char *getContinuousKeys( char *allowedKeys = nullptr ) override {
            char *keyList = readMappedKeyList();
            if ( allowedKeys != nullptr )
                removeDisallowedKeys( allowedKeys, keyList );
            return keyList;
        }

        uint8_t getContinuousKey( char *allowedKeys = nullptr ) override {
            return debouncer.getContinuousKey( allenKey( allowedKeys ) );
        }

        uint8_t getKeyDown( char *allowedKeys = nullptr ) override {
            return debouncer.getKeyDown( allenKey( allowedKeys ) );
        }

        uint8_t getKeyUp( char *allowedKeys = nullptr ) override {
            return debouncer.getKeyUp( allenKey( allowedKeys ) );
        }

        uint8_t getRepeatingKey( char *allowedKeys = nullptr ) override {
            return debouncer.getRepeatingKey( allenKey( allowedKeys ) );
        }

        uint8_t getRepeatingKeyExcept( uint8_t nonRepeatingKey, char *allowedKeys = nullptr ) override {
            uint8_t key = debouncer.getRepeatingKey( allenKey( allowedKeys ) );
            if ( key == nonRepeatingKey )
                flagWaitForKeyupSpecific( key );
            return key;
        }

        uint8_t getRepeatingKeyExcept( char *nonRepeatingKeys, char *allowedKeys = nullptr ) override {
            uint8_t key = debouncer.getRepeatingKey( allenKey( allowedKeys ) );
            if ( nonRepeatingKeys != nullptr && isCharInString( key, nonRepeatingKeys ) )
                flagWaitForKeyupSpecific( key );
            return key;
        }

    //
    // KEYMAP
    //
    /*
    protected:

        uint8_t getKeymapForMultiKey32( uint32_t keyBitmap ) {
            if ( keyBitmap == 0 ) return 0;
            if ( countBits( keyBitmap ) > 1 ) return 0;
            uint8_t bitPosition = first1BitOccurence( keyBitmap );
            if ( keyMap == nullptr )
                return bitPosition;
            if ( strlen( keyMap ) < bitPosition )
                return bitPosition;
            return keyMap[ bitPosition-1 ];
        }

        char *getKeymapsForMultiKey32( uint32_t keyBitmap ) {

            static char buffer[33];

            if ( keyBitmap == 0 ) return nullptr;
            if ( keyMap == nullptr ) {
                buffer[0] = getKeymapForMultiKey32( keyBitmap );
                buffer[1] = 0;
                return buffer;
            }

            uint8_t mapLen = strlen( keyMap );
            char *p = buffer;
            uint8_t mapIndex = 0;

            // get keyMap for each bit pressed
            // combine as string
            while( keyBitmap != 0 ) {
                if ( mapIndex >= mapLen )
                    break;
                if ( (keyBitmap & 1) != 0 ) {
                    // key pressed, add keyMap
                    *p = keyMap[mapIndex];
                    p++;
                }
                mapIndex++;
                keyBitmap >>= 1;
            }
            *p = 0;
            return buffer;
        }*/

    //
    // DEBOUNCER
    //
    public:

        virtual void flagWaitForKeyupMulti( char *keys ) = 0;

    protected:

    //
    // COMBINED KEYS
    //
    protected:

        // if 2 keys are presssed at the same time
        //    return another key instead
        // ex. [up] and [left] --> [upLeft]
        struct combined_2Keys {
            uint8_t key1;
            uint8_t key2;
            uint8_t result;
            combined_2Keys *next = nullptr;
        };
        combined_2Keys *combinedList_2Keys = nullptr;

        // void deleteCombinedKeys() {
        //     if ( combinedList_2Keys != nullptr ) {
        //         while( combinedList_2Keys->next != nullptr ) {
        //             // slow but avoids recursion
        //             combined_2Keys *p1 = combinedList_2Keys;
        //             combined_2Keys *p2 = p1->next;
        //             while ( p2->next != nullptr ) {
        //                 p1 = p2;
        //                 p2 = p2->next;
        //             }
        //             // Serial.print( "delete: " );
        //             // Serial.println( p2->result );
        //             delete p2;
        //             p1->next = nullptr;
        //         }
        //         delete combinedList_2Keys;
        //     }
        // }

    public:

        void addCombinedKey( uint8_t key1, uint8_t key2, uint8_t combined ) {
            combined_2Keys *ck = new combined_2Keys;
            ck->key1 = key1; ck->key2 = key2; ck->result = combined;
            insertEnd_NextLinkedList( &combinedList_2Keys, ck );
            // if ( combinedList_2Keys == nullptr )
            //     combinedList_2Keys = ck;
            // else {
            //     combined_2Keys *p = combinedList_2Keys;
            //     while ( p->next != nullptr ) p = p->next;
            //     p->next = ck;
            // }
        }

    //
    // PROCESS MULTIPLE KEYS
    //
    protected:

// uint8_t prevPressedCount = -1;
// uint8_t prevToReturn = -1;
// void reportPressedCount( uint8_t pressedCount ) {
//     if ( prevPressedCount != pressedCount ) {
//         Serial.printf( "dm pressed=%d\n", pressedCount );
//         prevPressedCount = pressedCount;
//     }
// }
// void reportToReturn( uint8_t toReturn ) {
//     if ( prevToReturn != toReturn ) {
//         Serial.printf( "dm ret=%d\n", toReturn );
//         prevToReturn = toReturn;
//     }
// }

        uint8_t processKeysCore( char *allowedKeys, char *keysPressed, bool issueWaitForKeyUp ) {

            // filter keysPressed by allowed keys
            //    if an actual key is pressed it will be ignored
            // combine multiple keys into single key

            if ( keysPressed == nullptr ) return 0;
            if ( allowedKeys != nullptr )
                removeDisallowedKeys( allowedKeys, keysPressed );

            uint8_t pressedCount = strlen( keysPressed );

// reportPressedCount( pressedCount );

            switch( pressedCount ) {
            case 1:
                if ( issueWaitForKeyUp )
                    flagWaitForKeyupMulti( keysPressed );

// reportToReturn( keysPressed[0] );

                return keysPressed[0];
            case 2:
                if ( combinedList_2Keys != nullptr ) {
                    combined_2Keys *ck = combinedList_2Keys;
                    while ( ck != nullptr ) {
                        // check if both keys are pressed
                        if( ( ck->key1==keysPressed[0] && ck->key2==keysPressed[1] )
                        || ( ck->key1==keysPressed[1] && ck->key2==keysPressed[0] ) ) {
                            if ( issueWaitForKeyUp )
                                flagWaitForKeyupMulti( keysPressed );

// reportToReturn( ck->result );

                            return ck->result;
                        }
                        ck = ck->next;
                    }
                }

// reportToReturn( 0 );

                return 0;
            default:

// reportToReturn( 0 );

                return 0;
            }
        }

        void removeDisallowedKeys( char *allowedKeys, char *keysPressed ) {

            // ex.     allowedKeys = "12345"
            //         keysPressed = "1829"
            //     ==> keysPressed = "12";

            // if ( allowedKeys == nullptr ) {
            //    keysPressed[0] = 0;
            //    return;
            // }

            char *checking = keysPressed;
            char *accepted = keysPressed;
            while ( *checking != 0 ) {
                if ( strchr( allowedKeys, *checking ) != nullptr ) {
                    *accepted = *checking;
                    accepted++;
                }
                checking++;
            }
            *accepted = 0;
        }

};

}
