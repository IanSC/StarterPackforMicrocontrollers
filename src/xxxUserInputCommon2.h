// #pragma once
// #include <stdint.h>
// #include <Debouncer.h>
// // #include <UserInput2.h>
// // using namespace StarterPack::UserInput2;

// namespace StarterPack {

// class UserInputCommon2 {

//     protected:

//         // static const uint8_t MAX_KEYS = 9;

//         // static const uint8_t SLOT_OKAY     = 0;
//         // static const uint8_t SLOT_LEFT     = 1;
//         // static const uint8_t SLOT_RIGHT    = 2;
//         // static const uint8_t SLOT_UP       = 3;
//         // static const uint8_t SLOT_DOWN     = 4;
//         // static const uint8_t SLOT_CANCEL   = 5;
//         // static const uint8_t SLOT_CUSTOM_1 = 6;
//         // static const uint8_t SLOT_CUSTOM_2 = 7;
//         // static const uint8_t SLOT_CUSTOM_3 = 8;

//     public:

//         ~UserInputCommon2() {}

//         // enum Keys {
//         //     None          = 0b000000000,
//         //     Okay          = 0b000000001,
//         //     Left          = 0b000000010,
//         //     Right         = 0b000000100,
//         //     Up            = 0b000001000,
//         //     Down          = 0b000010000,
//         //     Cancel        = 0b000100000,
//         //     Custom1       = 0b001000000,
//         //     Custom2       = 0b010000000,
//         //     Custom3       = 0b100000000,
//         //     UpLeft        = Up | Left,
//         //     UpRight       = Up | Right,
//         //     DownLeft      = Down | Left,
//         //     DownRight     = Down | Right,
//         //     LeftRight     = Left | Right,
//         //     UpDown        = Up | Down,
//         //     LeftRightOkay = Left | Right | Okay,
//         //     UpDownOkay    = Up | Down | Okay,
//         //     All           = 0xFF // Left | Right | Up | Down | Okay
//         // };

//         // // https://stackoverflow.com/a/15889501
//         // inline friend Keys operator | ( Keys a, Keys b ) {
//         //     return static_cast<Keys>( static_cast<int>(a) | static_cast<int>(b) );
//         // }
//         // inline friend Keys operator & ( Keys a, Keys b ) {
//         //     return static_cast<Keys>( static_cast<int>(a) & static_cast<int>(b) );
//         // }
//         // inline friend Keys& operator |= ( Keys& a, Keys b ) {
//         //     a = a | b; return a;
//         // }
//         // inline friend Keys& operator &= ( Keys& a, Keys b ) {
//         //     a = a & b; return a;
//         // }

//         // static void displayKey( Keys key ) {
//         //     if ( key == Keys::None )
//         //         Serial.print( "None " );
//         //     else {
//         //         if ( (key & Keys::Okay   ) != 0 ) Serial.print( "Okay" );
//         //         if ( (key & Keys::Up     ) != 0 ) Serial.print( "Up" );
//         //         if ( (key & Keys::Down   ) != 0 ) Serial.print( "Down" );
//         //         if ( (key & Keys::Left   ) != 0 ) Serial.print( "Left" );
//         //         if ( (key & Keys::Right  ) != 0 ) Serial.print( "Right" );
//         //         if ( (key & Keys::Cancel ) != 0 ) Serial.print( "Cancel" );
//         //     }
//         //     Serial.println();
//         // }

//         // allow UpLeft, UpRight, DownLeft, DownRight
//         // bool allowDiagonals = false;

//         // void addDevice( DigitalIO key, uint16_t keyId ) {}
//         // void allowSimulaneous( uint16_t keyId1, uint16_t keyId2 ) {}
//         // void addDevice( AnalogIO key ) {}
//         // void analogMapping( uint8_t analogButtonNo, uint16_t keyId ) {}

//     public:

//         // special handling for common keys for left/up and right/down:
//         // for common keys:
//         // - use same 2 keys for LCDUtility::choose, LCDUtility::Story
//         // for separate keys:
//         // - use left/right for LCDUtility::choose
//         // - use up/down    for LCDUtility::Story
//         // bool commonUpLeft = false;
//         // bool commonDownRight = false;

//     //
//     // KEYS
//     //
//     public:

//         virtual char *getContinuousKeys( char *allowedKeys = nullptr ) = 0;
//         virtual uint8_t getContinuousKey( char *allowedKeys = nullptr )  = 0;
//         virtual uint8_t getKeyDown( char *allowedKeys = nullptr ) = 0;
//         virtual uint8_t getRepeatingKey( char *allowedKeys = nullptr ) = 0;
//         virtual uint8_t getRepeatingKeyExcept( char *nonRepeatingKeys, char *allowedKeys = nullptr ) = 0;
//         // virtual uint8_t getRepeatingKey( bool okayKeyAlsoRepeating, char *allowedKeys = nullptr ) = 0;

//         // no key filtering, no combining of keys
//         virtual char *getContinuousKeysFast() = 0;
//         virtual uint8_t getContinuousKeyFast() = 0;
//         virtual uint8_t getKeyDownFast() = 0;
//         virtual uint8_t getRepeatingKeyFast() = 0;
//         virtual uint8_t getRepeatingKeyExceptFast( uint8_t exception ) = 0;
//     public:
//         virtual uint8_t getRepeatingKeyExceptFast( char *nonRepeatingKeys ) = 0;

//     //
//     // KEYMAP
//     //
//     public:

//         inline void assignKeymap( const char *keyMap ) {
//             this->keyMap = keyMap;
//         }

//     protected:

//         const char *keyMap = nullptr;

//         uint8_t getKeymap( const uint32_t keyBitmap ) {
//             if ( keyBitmap == 0 ) return 0;
//             if ( countBits( keyBitmap ) > 1 ) return 0;
//             // https://stackoverflow.com/a/31393298
//             uint8_t bitPosition = __builtin_ctzl( keyBitmap );
//             if ( keyMap == nullptr )
//                 return bitPosition + 1;
//             if ( strlen( keyMap ) < bitPosition )
//                 return bitPosition + 1;
//             // Serial.print( "bitPosition=" );
//             // Serial.println( bitPosition );
//             return keyMap[ bitPosition ];
//         }

//         char *getKeymaps( uint32_t keyBitmap ) {

//             char buffer[33];

//             if ( keyBitmap == 0 ) return nullptr;
//             if ( keyMap == nullptr ) {
//                 buffer[0] = getKeymap( keyBitmap );
//                 buffer[1] = 0;
//                 return buffer;
//             }

//             uint8_t mapLen = strlen( keyMap );
//             char *p = buffer;
//             uint8_t mapIndex = 0;

//             // get keyMap for each bit pressed
//             // combine as string
//             while( keyBitmap != 0 ) {
//                 if ( mapIndex >= mapLen )
//                     break;
//                 if ( (keyBitmap & 1) != 0 ) {
//                     // key pressed, add keyMap
//                     *p = keyMap[mapIndex];
//                     p++;
//                 }
//                 mapIndex++;
//                 keyBitmap >>= 1;
//             }
//             *p = 0;
//             return buffer;
//         }

//     //
//     // DEBOUNCER
//     //
//     public:

//         Debouncer debouncer;

//         void setDebounceTimeInMs( uint16_t activeState = 50, uint16_t inactiveState = 50, uint16_t minimum = 50 ) {
//             Debouncer::Settings *s = debouncer.getSettings();
//             s->activeStatesDebounceInMs = activeState;
//             s->inactiveStateDebounceInMs = inactiveState;
//             s->minimumDebounceTimeInMs = minimum;
//         }

//         inline void flagWaitForKeyup() { debouncer.flagWaitForKeyup(); }
//         inline void cancelDebouncing() { debouncer.cancelDebouncing(); }

//     protected:

//         virtual void flagWaitForKeyup( char *keysPressed ) = 0;
//         virtual void flagWaitForKeyup( char key ) = 0;

//         //     // void setRepeatDelayInMs( uint16_t repeatDelay ) {
//         //     //     if ( usingDigitalKeys ) {
//         //     //         for( int i=0 ; i<5 ; i++ ) {
//         //     //             if ( ioDigital[i] != nullptr )
//         //     //                 ioDigital[i]->setRepeatDelayInMs( repeatDelay );
//         //     //         }
//         //     //         debouncer.repeatDelayInMs = repeatDelay;
//         //     //     } else {
//         //     //         ioAnalog->setRepeatDelayInMs( repeatDelay );
//         //     //     }
//         //     // }

//         //     // void setRepeatRateInMs( uint16_t repeatRate ) {
//         //     //     if ( usingDigitalKeys ) {
//         //     //         for( int i=0 ; i<5 ; i++ ) {
//         //     //             if ( ioDigital[i] != nullptr )
//         //     //                 ioDigital[i]->setRepeatRateInMs( repeatRate );
//         //     //         }
//         //     //         debouncer.repeatRateInMs = repeatRate;
//         //     //     } else {
//         //     //         ioAnalog->setRepeatRateInMs( repeatRate );
//         //     //     }
//         //     // }

//     //
//     // COMMON
//     //
//     public:

//         bool isKeyPressed( char key, bool issueWaitForKeyUp = true ) {
//             if ( getContinuousKey() == key ) {
//                 if ( issueWaitForKeyUp )
//                     flagWaitForKeyup( key ); // 0b0001 ); // leftmost bit for okay key
//                 return true;
//             }
//             return false;
//         }

//         // bool isOkayPressed( bool issueWaitForKeyUp = true ) {
//         //     if ( getContinuousKey() == Keys::Okay ) {
//         //         if ( issueWaitForKeyUp )
//         //             flagWaitForKeyup( nullptr ); // 0b0001 ); // leftmost bit for okay key
//         //         return true;
//         //     }
//         //     return false;
//         // }

//         void waitUntilNothingIsPressed() {
//             while( true ) {
//                 if ( getContinuousKey() == 0 ) return;
//             }
//         }

//         void waitForAnyKey() {
//             waitUntilNothingIsPressed();
//             while ( true ) {
//                 if ( getKeyDown() != 0 ) return;
//             }
//         }

//     //
//     // COMBINED KEYS
//     //
//     private:

//         static const uint8_t MAX_COMBINED = 5;
//         char combined[MAX_COMBINED][3];
//         uint8_t combinedIndex = 0;

//     public:

//         void addCombinedKey( uint8_t key1, uint8_t key2, uint8_t newKey ) {
//             if ( combinedIndex >= MAX_COMBINED ) return;
//             combined[combinedIndex][0] = key1;
//             combined[combinedIndex][1] = key2;
//             combined[combinedIndex][2] = newKey;
//             combinedIndex++;
//         }

//     // FOR DEBUGGING ONLY
//     // public:
//     protected:

//         uint8_t processKeysCore( char *allowedKeys, char *keysPressed, bool issueWaitForKeyUp ) {

//             if ( keysPressed == nullptr ) return 0;
//             if ( allowedKeys != nullptr )
//                 removeDisallowedKeys( allowedKeys, keysPressed );

//             uint8_t pressedCount = strlen( keysPressed );
//             switch( pressedCount ) {
//             case 1:
//                 if ( issueWaitForKeyUp )
//                     flagWaitForKeyup( keysPressed[0] );
//                 return keysPressed[0];
//             case 2:
//                 for( int i = 0 ; i < combinedIndex ; i++ ) {
//                     // check if both keys are pressed
//                     if( ( combined[i][0]==keysPressed[0] && combined[i][1]==keysPressed[1] )
//                     || ( combined[i][0]==keysPressed[1] && combined[i][1]==keysPressed[0] ) ) {
//                         if ( issueWaitForKeyUp )
//                             flagWaitForKeyup( keysPressed );
//                         return combined[i][2];
//                     }
//                 }
//                 return 0;
//             default:
//                 return 0;
//             }
//         }

//         bool isIncludedInList( uint8_t key, char *list ) {
//             if ( list == nullptr ) return false;
//             return ( strchr( list, key ) != nullptr );
//         }

//         void removeDisallowedKeys( char *allowedKeys, char *keysPressed ) {
//             // if ( allowedKeys == nullptr ) return;
//             char *checking = keysPressed;
//             char *accepted = keysPressed;
//             while ( *checking != 0 ) {
//                 if ( strchr( allowedKeys, *checking ) != nullptr ) {
//                     *accepted = *checking;
//                     accepted++;
//                 }
//                 checking++;
//             }
//             *accepted = 0;
//         }

// };

// }
