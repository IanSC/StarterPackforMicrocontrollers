#pragma once
#include <DigitalIO.h>
#include <AnalogIO.h>

namespace StarterPack {

namespace UserInput {

    enum Keys {
        None          = 0b00000000,
        Okay          = 0b00000001,
        Left          = 0b00000010,
        Right         = 0b00000100,
        Up            = 0b00001000,
        Down          = 0b00010000,
        Cancel        = 0b00100000, // user defined
        UpLeft        = Up | Left,
        UpRight       = Up | Right,
        DownLeft      = Down | Left,
        DownRight     = Down | Right,
        LeftRight     = Left | Right,
        UpDown        = Up | Down,
        LeftRightOkay = Left | Right | Okay,
        UpDownOkay    = Up | Down | Okay,
        All           = Left | Right | Up | Down | Okay
    };

    // https://stackoverflow.com/a/15889501
    inline Keys operator | ( Keys a, Keys b ) {
        return static_cast<Keys>( static_cast<int>(a) | static_cast<int>(b) );
    }
    inline Keys operator & ( Keys a, Keys b ) {
        return static_cast<Keys>( static_cast<int>(a) & static_cast<int>(b) );
    }
    inline Keys& operator |= ( Keys& a, Keys b ) {
        a = a | b; return a;
    }
    inline Keys& operator &= ( Keys& a, Keys b ) {
        a = a & b; return a;
    }

    static void displayKey( Keys key ) {
        if ( key == Keys::None )
            Serial.print( "None " );
        else {
            if ( (key & Keys::Okay  ) != 0 ) Serial.print( "Okay" );
            if ( (key & Keys::Up    ) != 0 ) Serial.print( "Up" );
            if ( (key & Keys::Down  ) != 0 ) Serial.print( "Down" );
            if ( (key & Keys::Left  ) != 0 ) Serial.print( "Left" );
            if ( (key & Keys::Right ) != 0 ) Serial.print( "Right" );
            if ( (key & Keys::Cancel) != 0 ) Serial.print( "Cancel" );
        }
        Serial.println();
    }

    class Core {

        private:

            bool usingDigitalKeys = true;

            // DIGITAL KEYS
            //                          okay     left     right    up       left
            DigitalIO *ioDigital[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };

            // ANALOG RESISTOR NETWORK KEYS
            AnalogIO *ioAnalog = nullptr;
            int ioAnalogButtonNo[5];

        //
        // INIT
        //
        public:

            void assignKeys( DigitalIO *okayKey, DigitalIO *leftAndUpKey, DigitalIO *rightAndDownKey ) {
                usingDigitalKeys = true;
                ioDigital[0] = okayKey;
                ioDigital[1] = leftAndUpKey;
                ioDigital[2] = rightAndDownKey;
                ioDigital[3] = leftAndUpKey;
                ioDigital[4] = rightAndDownKey;
                checkCommonKeys();
            }

            void assignKeys( DigitalIO *okayKey,
            DigitalIO *leftKey, DigitalIO *rightKey, DigitalIO *upKey, DigitalIO *downKey ) {
                usingDigitalKeys = true;
                ioDigital[0] = okayKey;
                ioDigital[1] = leftKey;
                ioDigital[2] = rightKey;
                ioDigital[3] = upKey;
                ioDigital[4] = downKey;
                checkCommonKeys();
            }

            void assignKeys( AnalogIO &analogKey,
            int okayKeyValue,
            int leftKeyValue, int rightKeyValue, int upKeyValue, int downKeyValue ) {
                usingDigitalKeys = false;
                this->ioAnalog = &analogKey;
                ioAnalogButtonNo[0] = okayKeyValue;
                ioAnalogButtonNo[1] = leftKeyValue;
                ioAnalogButtonNo[2] = rightKeyValue;
                ioAnalogButtonNo[3] = upKeyValue;
                ioAnalogButtonNo[4] = downKeyValue;
                checkCommonKeys();
            }

            // allow UpLeft, UpRight, DownLeft, DownRight
            bool allowDiagonals = false;
        
        private:

            // special handling for common keys for left/up and right/down:
            // for common keys:
            // - use same 2 keys for LCDUtility::choose, LCDUtility::Story
            // for separate keys:
            // - use left/right for LCDUtility::choose
            // - use up/down    for LCDUtility::Story
            bool commonUpLeft = false;
            bool commonDownRight = false;

            void checkCommonKeys() {
                if ( usingDigitalKeys ) {
                    commonUpLeft    = ( ioDigital[1] == ioDigital[3] );
                    commonDownRight = ( ioDigital[2] == ioDigital[4] );
                } else {
                    commonUpLeft    = ( ioAnalog->getButtonValue(ioAnalogButtonNo[1]) == ioAnalog->getButtonValue(ioAnalogButtonNo[3]) );
                    commonDownRight = ( ioAnalog->getButtonValue(ioAnalogButtonNo[2]) == ioAnalog->getButtonValue(ioAnalogButtonNo[4]) );
                }
            }

        //
        //
        //
        public:

            // inline Keys getContinuousInput( Keys allowedKeys = Keys::All ) {
            //     return getInput( allowedKeys, false );
            // }

            Keys getContinuousKey( Keys allowedKeys = Keys::All ) {
                Keys input = Keys::None;
                if ( usingDigitalKeys ) {
                    for( int i=0 ; i<5 ; i++ ) {
                        if ( ioDigital[i] != nullptr && ioDigital[i]->getContinuousKey() )
                            input = static_cast<Keys>( input | (1<<i) );
                    }
                } else {                    
                    int8_t buttonNo = ioAnalog->getContinuousKey();
                    if ( buttonNo != 0 ) {
                        for( int i = 0 ; i < 5 ; i++ ) {
                            if ( ioAnalogButtonNo[i] == buttonNo ) {
                                input = static_cast<Keys>( input | (1<<i) );
                                break;
                            }
                        }
                    }
                }
                return processKeysCore( allowedKeys, input, false );
            }

            Keys getKeyDown( Keys allowedKeys = Keys::All ) {
                Keys input = Keys::None;
                if ( usingDigitalKeys ) {
                    for( int i=0 ; i<5 ; i++ ) {
                        if ( ioDigital[i] != nullptr && ioDigital[i]->getKeyDown() )
                            input = static_cast<Keys>( input | (1<<i) );
                    }
                } else {                    
                    int8_t buttonNo = ioAnalog->getKeyDown();
                    if ( buttonNo != 0 ) {
                        for( int i = 0 ; i < 5 ; i++ ) {
                            if ( ioAnalogButtonNo[i] == buttonNo ) {
                                input = static_cast<Keys>( input | (1<<i) );
                                break;
                            }
                        }
                    }
                }
                return processKeysCore( allowedKeys, input, false );
            }

            // Keys getInput( Keys allowedKeys = Keys::All, bool issueWaitForKeyUp = true ) {
            //     Keys input = Keys::None;
            //     if ( usingDigitalKeys ) {
            //         for( int i=0 ; i<5 ; i++ ) {
            //             if ( ioDigital[i] != nullptr && ioDigital[i]->getContinuousKey() )
            //                 input = static_cast<Keys>( input | (1<<i) );
            //         }
            //     } else {                    
            //         int8_t buttonNo = ioAnalog->getContinuousKey();
            //         if ( buttonNo != 0 ) {
            //             for( int i = 0 ; i < 5 ; i++ ) {
            //                 if ( ioAnalogButtonNo[i] == buttonNo ) {
            //                     input = static_cast<Keys>( input | (1<<i) );
            //                     break;
            //                 }
            //             }
            //         }
            //     }
            //     return processKeysCore( allowedKeys, input, issueWaitForKeyUp );
            // }

            Keys getRepeatingKey( bool okayKeyAlsoRepeating, Keys allowedKeys = Keys::All ) {
                return getRepeatingKey( allowedKeys, okayKeyAlsoRepeating );
            }

            Keys getRepeatingKey( Keys allowedKeys = Keys::All, bool okayKeyAlsoRepeating = false ) {
                // if repeating from DigitalIO is used, cannot do diagonal repeating
                Keys input = Keys::None;
                if ( usingDigitalKeys ) {
                    // if ( ioDigital[0] != nullptr && ioDigital[0]->getStatus() )
                    //     input = Okay;
                    // for( int i=1 ; i<5 ; i++ ) {
                    //     if ( ioDigital[i] != nullptr && ioDigital[i]->getRepeating() )
                    //         input = static_cast<Keys>( input | (1<<i) );
                    // }
                    for( int i=0 ; i<5 ; i++ ) {
                        if ( ioDigital[i] != nullptr && ioDigital[i]->getContinuousKey() )
                            input = static_cast<Keys>( input | (1<<i) );
                    }
                } else {
                    int8_t buttonNo = ioAnalog->getRepeatingKey();
                    if ( buttonNo != 0 ) {
                        for( int i = 0 ; i < 5 ; i++ ) {
                            if ( ioAnalogButtonNo[i] == buttonNo ) {
                                input = static_cast<Keys>( input | (1<<i) );
                                break;
                            }
                        }
                    }
                }

                Keys r = processKeysCore( allowedKeys, input, false );
                if ( usingDigitalKeys ) {
                    r = static_cast<Keys>( debouncer.getRepeatingKey( r ) );
                }
                if ( r == Okay && !okayKeyAlsoRepeating ) {
                    if ( usingDigitalKeys )
                        ioDigital[0]->flagWaitForKeyup();
                    else
                        ioAnalog->flagWaitForKeyup();
                }
                return r;
            }

            // Keys getRepeatingKey( Keys allowedKeys = Keys::All, bool issueWaitForKeyUpToOkayKey = true ) {
            //     // if repeating from DigitalIO is used, cannot do diagonal repeating
            //     Keys input = Keys::None;
            //     if ( usingDigitalKeys ) {
            //         // if ( ioDigital[0] != nullptr && ioDigital[0]->getStatus() )
            //         //     input = Okay;
            //         // for( int i=1 ; i<5 ; i++ ) {
            //         //     if ( ioDigital[i] != nullptr && ioDigital[i]->getRepeating() )
            //         //         input = static_cast<Keys>( input | (1<<i) );
            //         // }
            //         for( int i=0 ; i<5 ; i++ ) {
            //             if ( ioDigital[i] != nullptr && ioDigital[i]->getContinuousKey() )
            //                 input = static_cast<Keys>( input | (1<<i) );
            //         }
            //     } else {
            //         int8_t buttonNo = ioAnalog->getRepeatingKey();
            //         if ( buttonNo != 0 ) {
            //             for( int i = 0 ; i < 5 ; i++ ) {
            //                 if ( ioAnalogButtonNo[i] == buttonNo ) {
            //                     input = static_cast<Keys>( input | (1<<i) );
            //                     break;
            //                 }
            //             }
            //         }
            //     }

            //     Keys r = processKeysCore( allowedKeys, input, false );
            //     if ( usingDigitalKeys ) {
            //         r = static_cast<Keys>( debouncer.getRepeating( r ) );
            //     }
            //     if ( r == Okay && issueWaitForKeyUpToOkayKey ) {
            //         if ( usingDigitalKeys )
            //             ioDigital[0]->flagWaitForKeyup();
            //         else
            //             ioAnalog->flagWaitForKeyup();
            //     }
            //     return r;
            // }

        private:

            Debouncer debouncer;

        public:

            // void setRepeatDelayInMs( uint16_t repeatDelay ) {
            //     if ( usingDigitalKeys ) {
            //         for( int i=0 ; i<5 ; i++ ) {
            //             if ( ioDigital[i] != nullptr )
            //                 ioDigital[i]->setRepeatDelayInMs( repeatDelay );
            //         }
            //         debouncer.repeatDelayInMs = repeatDelay;
            //     } else {
            //         ioAnalog->setRepeatDelayInMs( repeatDelay );
            //     }
            // }

            // void setRepeatRateInMs( uint16_t repeatRate ) {
            //     if ( usingDigitalKeys ) {
            //         for( int i=0 ; i<5 ; i++ ) {
            //             if ( ioDigital[i] != nullptr )
            //                 ioDigital[i]->setRepeatRateInMs( repeatRate );
            //         }
            //         debouncer.repeatRateInMs = repeatRate;
            //     } else {
            //         ioAnalog->setRepeatRateInMs( repeatRate );
            //     }
            // }

            bool isOkayPressed( bool issueWaitForKeyUp = true ) {
                if ( usingDigitalKeys ) {
                    bool okay = ( ioDigital[0] != nullptr && ioDigital[0]->getStatus() );
                    if ( okay && issueWaitForKeyUp ) ioDigital[0]->flagWaitForKeyup();
                    return okay;
                } else {
                    bool okay = ( ioAnalog->getContinuousKey() == ioAnalogButtonNo[0] );
                    if ( okay && issueWaitForKeyUp ) ioAnalog->flagWaitForKeyup();
                    return okay;                    
                }
            }

            void waitUntilNothingIsPressed() {
                if ( usingDigitalKeys ) {
                    while ( true ) {
                        Keys input = Keys::None;
                        for( int i=0 ; i<5 ; i++ ) {
                            if ( ioDigital[i] != nullptr && ioDigital[i]->getContinuousKey() )
                                input = static_cast<Keys>( input | (1<<i) );
                        }
                        if ( input == 0 ) break;
                    }
                } else {
                    while( true ) {
                        if ( ioAnalog->getContinuousKey() == ioAnalog->debouncer.inactiveState )
                            break;
                    }
                }
            }

            void waitForAnyKey() {
                waitUntilNothingIsPressed();
                while ( true ) {
                    Keys input = getKeyDown();
                    if ( input != None )
                        return;
                }
            }

        private:

            void sendWaitForKeyUp( uint8_t input ) {
                // input = 0b000DURLO
                //    O - if 1, send flag to okay button
                //    L - if 1, send flag to left button
                //    ...
                if ( usingDigitalKeys ) {
                    for( int i = 0 ; i < 5 ; i++ ) {
                        if ( (input & 1) != 0 )
                            ioDigital[i]->flagWaitForKeyup();
                        input >>= 1;
                    }                                   
                } else if ( input != Keys::None ) {
                    ioAnalog->flagWaitForKeyup();
                }
            }

            uint8_t countBits( uint8_t input ) {
                // count number of "1" bits
                uint8_t bits = 0;
                for( int i = 0 ; i < 5 ; i++ ) {
                    if ( (input & 1) == 1 )
                        bits++;
                    input >>= 1;
                }
                return bits;
            }

            uint8_t removeCommonKeys( uint8_t input ) {
                // input = 0b000DURLO
                // if L and U are both pressed, remove U, ex. -U-L- => ---L-
                if ( commonUpLeft && ( input & UpLeft ) == UpLeft )
                    input = input & ~Up;
                // if R and D are both pressed, remove D, ex. D-R-- => --R--
                if ( commonDownRight && ( input & DownRight ) == DownRight )
                    input = input & ~Down;
                return input;
            }

        // FOR DEBUGGING ONLY
        // public: 

            Keys processKeysCore( Keys allowedKeys, Keys input, bool issueWaitForKeyUp ) {
                //                    54321
                // allowedKeys = 0b000DURLO
                // input       = 0b000DURLO
                //      bit 1 - okay  key status (O)
                //      bit 2 - left  key status (L)
                //      bit 3 - right key status (R)
                //      bit 4 - up    key status (U)
                //      bit 5 - down  key status (D)

                uint8_t numberOfButtonsPressed = countBits( input );
                //Serial.printf( "bits on: %d\n", numberOfBitsOn );

                switch( numberOfButtonsPressed ) {
                case 0: return Keys::None;
                case 1:
                    input = input & allowedKeys;
                    if ( issueWaitForKeyUp )
                        sendWaitForKeyUp( input );
                    return input;
                case 2:
                    if ( allowDiagonals ) {
                        switch( input ) {
                        case Keys::UpLeft:
                        case Keys::UpRight:
                        case Keys::DownLeft:
                        case Keys::DownRight:
                            input = input & allowedKeys;
                            if ( issueWaitForKeyUp )
                                sendWaitForKeyUp( removeCommonKeys(input) );
                            return input;
                        default:
                            return Keys::None;
                        }
                    } else {
                        // diagonals not allowed
                        switch( input ) {
                        case Keys::UpLeft:
                            if ( commonUpLeft ) {
                                input = input & allowedKeys;
                                if ( issueWaitForKeyUp )
                                    sendWaitForKeyUp( removeCommonKeys(input) );
                                return input;
                            }
                            break;
                        case Keys::DownRight:
                            if ( commonDownRight ) {
                                input = input & allowedKeys;
                                if ( issueWaitForKeyUp )
                                    sendWaitForKeyUp( removeCommonKeys(input) );
                                return input;
                            }
                            break;
                        default: break;
                        }
                        return Keys::None;
                    }
                    break;
                default: return Keys::None;
                }
            }

    };

    static Core core;

    // FOR DEBUGGING ONLY
    // inline static Keys processKeysCore( Keys allowedKeys, Keys input, bool issueWaitForKeyUp ) {
    //     return core.processKeysCore( allowedKeys, input, issueWaitForKeyUp );
    // }

    inline static void assignKeys( DigitalIO *okayKey, DigitalIO *leftAndUpKey, DigitalIO *rightAndDownKey ) {
        core.assignKeys( okayKey, leftAndUpKey, rightAndDownKey );
    }

    inline static void assignKeys( DigitalIO *okayKey,
    DigitalIO *leftKey, DigitalIO *rightKey, DigitalIO *upKey, DigitalIO *downKey ) {
        core.assignKeys( okayKey, leftKey, rightKey, upKey, downKey );
    }

    inline static void assignKeys( AnalogIO &analogKey,
    int okayKeyValue,
    int leftKeyValue, int rightKeyValue, int upKeyValue, int downKeyValue ) {
        core.assignKeys( analogKey, okayKeyValue, leftKeyValue, rightKeyValue, upKeyValue, downKeyValue );
    }

    inline static void allowDiagonals() {
        core.allowDiagonals = true;
    }

    inline static void disallowDiagonals() {
        core.allowDiagonals = false;
    }

    inline static bool isOkayPressed( bool issueWaitForKeyUp = true ) {
        return core.isOkayPressed( issueWaitForKeyUp );
    }

    // inline static Keys getInput( Keys allowedKeys = Keys::All ) {
    //     return core.getInput( allowedKeys, true );
    // }

    inline static Keys getKeyDown( Keys allowedKeys = Keys::All ) {
        return core.getKeyDown( allowedKeys );
    }

    inline static Keys getContinuousKey( Keys allowedKeys = Keys::All ) {
        return core.getContinuousKey( allowedKeys );
    }

    // inline static Keys getContinuousInput( Keys allowedKeys = Keys::All ) {
    //     return core.getContinuousInput( allowedKeys );
    // }

    inline static Keys getRepeatingKey( Keys allowedKeys = Keys::All, bool okayKeyAlsoRepeating = false ) {
        return core.getRepeatingKey( allowedKeys, okayKeyAlsoRepeating );
    }

    inline static Keys getRepeatingKey( bool okayKeyAlsoRepeating, Keys allowedKeys = Keys::All ) {
        return core.getRepeatingKey( allowedKeys, okayKeyAlsoRepeating );
    }

    // inline static Keys getRepeatingInput( Keys allowedKeys = Keys::All, bool issueWaitForKeyUpToOkayKey = true ) {
    //     return core.getRepeatingInput( allowedKeys, issueWaitForKeyUpToOkayKey );
    // }

    // inline static Keys getRepeatingInput( bool issueWaitForKeyUpToOkayKey, Keys allowedKeys = Keys::All ) {
    //     return core.getRepeatingInput( allowedKeys, issueWaitForKeyUpToOkayKey );
    // }

    inline static void waitUntilNothingIsPressed() {
        core.waitUntilNothingIsPressed();
    }

    inline static void waitForAnyKey() {
        core.waitForAnyKey();
    }

};

}
