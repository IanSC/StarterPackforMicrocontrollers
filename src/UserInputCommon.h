#pragma once
#include <stdint.h>
#include <Debouncer.h>
#include <UserInput.h>
using namespace StarterPack::UserInput;

namespace StarterPack {

class UserInputCommon {

    protected:

        static const uint8_t MAX_KEYS = 9;

        static const uint8_t SLOT_OKAY     = 0;
        static const uint8_t SLOT_LEFT     = 1;
        static const uint8_t SLOT_RIGHT    = 2;
        static const uint8_t SLOT_UP       = 3;
        static const uint8_t SLOT_DOWN     = 4;
        static const uint8_t SLOT_CANCEL   = 5;
        static const uint8_t SLOT_CUSTOM_1 = 6;
        static const uint8_t SLOT_CUSTOM_2 = 7;
        static const uint8_t SLOT_CUSTOM_3 = 8;

    public:

        ~UserInputCommon() {}

        // enum Keys {
        //     None          = 0b000000000,
        //     Okay          = 0b000000001,
        //     Left          = 0b000000010,
        //     Right         = 0b000000100,
        //     Up            = 0b000001000,
        //     Down          = 0b000010000,
        //     Cancel        = 0b000100000,
        //     Custom1       = 0b001000000,
        //     Custom2       = 0b010000000,
        //     Custom3       = 0b100000000,
        //     UpLeft        = Up | Left,
        //     UpRight       = Up | Right,
        //     DownLeft      = Down | Left,
        //     DownRight     = Down | Right,
        //     LeftRight     = Left | Right,
        //     UpDown        = Up | Down,
        //     LeftRightOkay = Left | Right | Okay,
        //     UpDownOkay    = Up | Down | Okay,
        //     All           = 0xFF // Left | Right | Up | Down | Okay
        // };

        // // https://stackoverflow.com/a/15889501
        // inline friend Keys operator | ( Keys a, Keys b ) {
        //     return static_cast<Keys>( static_cast<int>(a) | static_cast<int>(b) );
        // }
        // inline friend Keys operator & ( Keys a, Keys b ) {
        //     return static_cast<Keys>( static_cast<int>(a) & static_cast<int>(b) );
        // }
        // inline friend Keys& operator |= ( Keys& a, Keys b ) {
        //     a = a | b; return a;
        // }
        // inline friend Keys& operator &= ( Keys& a, Keys b ) {
        //     a = a & b; return a;
        // }

        // static void displayKey( Keys key ) {
        //     if ( key == Keys::None )
        //         Serial.print( "None " );
        //     else {
        //         if ( (key & Keys::Okay   ) != 0 ) Serial.print( "Okay" );
        //         if ( (key & Keys::Up     ) != 0 ) Serial.print( "Up" );
        //         if ( (key & Keys::Down   ) != 0 ) Serial.print( "Down" );
        //         if ( (key & Keys::Left   ) != 0 ) Serial.print( "Left" );
        //         if ( (key & Keys::Right  ) != 0 ) Serial.print( "Right" );
        //         if ( (key & Keys::Cancel ) != 0 ) Serial.print( "Cancel" );
        //     }
        //     Serial.println();
        // }

        // allow UpLeft, UpRight, DownLeft, DownRight
        bool allowDiagonals = false;

        // void addDevice( DigitalIO key, uint16_t keyId ) {}
        // void allowSimulaneous( uint16_t keyId1, uint16_t keyId2 ) {}
        // void addDevice( AnalogIO key ) {}
        // void analogMapping( uint8_t analogButtonNo, uint16_t keyId ) {}

    public:

        // special handling for common keys for left/up and right/down:
        // for common keys:
        // - use same 2 keys for LCDUtility::choose, LCDUtility::Story
        // for separate keys:
        // - use left/right for LCDUtility::choose
        // - use up/down    for LCDUtility::Story
        bool commonUpLeft = false;
        bool commonDownRight = false;

        // virtual void checkCommonKeys() = 0;


    //
    //
    //
    public:

        virtual Keys getKeyDown( Keys allowedKeys = Keys::All ) = 0;

        virtual Keys getContinuousKey( Keys allowedKeys = Keys::All )  = 0;

        virtual Keys getRepeatingKey( bool okayKeyAlsoRepeating, Keys allowedKeys = Keys::All ) = 0;

        virtual Keys getRepeatingKey( Keys allowedKeys = Keys::All, bool okayKeyAlsoRepeating = false ) = 0;        

    protected:

        Debouncer debouncer;

        //     // void setRepeatDelayInMs( uint16_t repeatDelay ) {
        //     //     if ( usingDigitalKeys ) {
        //     //         for( int i=0 ; i<5 ; i++ ) {
        //     //             if ( ioDigital[i] != nullptr )
        //     //                 ioDigital[i]->setRepeatDelayInMs( repeatDelay );
        //     //         }
        //     //         debouncer.repeatDelayInMs = repeatDelay;
        //     //     } else {
        //     //         ioAnalog->setRepeatDelayInMs( repeatDelay );
        //     //     }
        //     // }

        //     // void setRepeatRateInMs( uint16_t repeatRate ) {
        //     //     if ( usingDigitalKeys ) {
        //     //         for( int i=0 ; i<5 ; i++ ) {
        //     //             if ( ioDigital[i] != nullptr )
        //     //                 ioDigital[i]->setRepeatRateInMs( repeatRate );
        //     //         }
        //     //         debouncer.repeatRateInMs = repeatRate;
        //     //     } else {
        //     //         ioAnalog->setRepeatRateInMs( repeatRate );
        //     //     }
        //     // }

    public:

        bool isOkayPressed( bool issueWaitForKeyUp = true ) {
            if ( getContinuousKey() == Keys::Okay ) {
                if ( issueWaitForKeyUp )
                    flagWaitForKeyup( 0b0001 ); // leftmost bit for okay key
                return true;
            }
            return false;
        }

        void waitUntilNothingIsPressed() {
            while( true ) {
                if ( getContinuousKey() == Keys::None ) return;
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

    protected:

        virtual void flagWaitForKeyup( uint16_t input ) = 0;

        uint16_t removeCommonKeys( uint16_t input ) {
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

            uint8_t numberOfButtonsPressed = countBits( static_cast<uint16_t>( input ) );
            //Serial.printf( "bits on: %d\n", numberOfBitsOn );

            switch( numberOfButtonsPressed ) {
            case 0: return Keys::None;
            case 1:
                input = input & allowedKeys;
                if ( issueWaitForKeyUp )
                    flagWaitForKeyup( input );
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
                            flagWaitForKeyup( removeCommonKeys(input) );
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
                                flagWaitForKeyup( removeCommonKeys(input) );
                            return input;
                        }
                        break;
                    case Keys::DownRight:
                        if ( commonDownRight ) {
                            input = input & allowedKeys;
                            if ( issueWaitForKeyUp )
                                flagWaitForKeyup( removeCommonKeys(input) );
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

}
