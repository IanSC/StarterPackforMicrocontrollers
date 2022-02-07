#pragma once
#include <UserInputCommon.h>
#include <UserInput.h>
#include <AnalogIO.h>
using namespace StarterPack::UserInput;
// namespace ui = StarterPack::UserInput;

namespace StarterPack {

class UserInputAnalog : public UserInputCommon {

    private:

        // ANALOG RESISTOR NETWORK KEYS
        AnalogIO *aIO = nullptr;
        int buttonNo[MAX_KEYS];

    public:

        UserInputAnalog( AnalogIO &analogKey ) {
            this->aIO = &analogKey;
        }

        UserInputAnalog( AnalogIO &analogKey,
        int okayKeyValue,
        int leftKeyValue, int rightKeyValue, int upKeyValue, int downKeyValue ) {
            this->aIO = &analogKey;
            buttonNo[SLOT_OKAY ] = okayKeyValue;
            buttonNo[SLOT_LEFT ] = leftKeyValue;
            buttonNo[SLOT_RIGHT] = rightKeyValue;
            buttonNo[SLOT_UP   ] = upKeyValue;
            buttonNo[SLOT_DOWN ] = downKeyValue;
        }

        UserInputAnalog( AnalogIO &analogKey, uint8_t keyNo, int value ) {
            if ( keyNo > MAX_KEYS ) return;
            this->aIO = &analogKey;
            buttonNo[keyNo] = value;
        }

        void addKey( int analogButtonId, Keys key ) {
            uint8_t numberOfBits = countBits( static_cast<uint16_t>( key ) );
            if ( numberOfBits != 1 ) return;
            // https://stackoverflow.com/a/31393298
            uint8_t bitPosition = __builtin_ctzl( key );
            if ( bitPosition > MAX_KEYS ) return;
            buttonNo[bitPosition] = analogButtonId;
        }

    //
    //
    //
    public:

        Keys getKeyDown( Keys allowedKeys = Keys::All ) {
            //if ( ioAnalog == nullptr ) return Keys::None;
            Keys input = Keys::None;
            int8_t bNo = aIO->getKeyDown();
            if ( bNo != 0 ) {
                for( int i = 0 ; i < MAX_KEYS ; i++ ) {
                    if ( buttonNo[i] == bNo ) {
                        input = static_cast<Keys>( input | (1<<i) );
                        break;
                    }
                }
            }
            return processKeysCore( allowedKeys, input, false );
        }

        Keys getContinuousKey( Keys allowedKeys = Keys::All ) {
            //if ( ioAnalog == nullptr ) return Keys::None;
            Keys input = Keys::None;
            int8_t bNo = aIO->getContinuousKey();
            if ( bNo != 0 ) {
                for( int i = 0 ; i < MAX_KEYS ; i++ ) {
                    if ( buttonNo[i] == bNo ) {
                        input = static_cast<Keys>( input | (1<<i) );
                        break;
                    }
                }
            }
            return processKeysCore( allowedKeys, input, false );
        }

        Keys getRepeatingKey( bool okayKeyAlsoRepeating, Keys allowedKeys = Keys::All ) {
            return getRepeatingKey( allowedKeys, okayKeyAlsoRepeating );
        }

        Keys getRepeatingKey( Keys allowedKeys = Keys::All, bool okayKeyAlsoRepeating = false ) {                
            //if ( ioAnalog == nullptr ) return Keys::None;
            Keys input = Keys::None;
            int8_t bNo = aIO->getRepeatingKey();
            if ( bNo != 0 ) {
                for( int i = 0 ; i < MAX_KEYS ; i++ ) {
                    if ( buttonNo[i] == bNo ) {
                        input = static_cast<Keys>( input | (1<<i) );
                        break;
                    }
                }
            }
            Keys r = processKeysCore( allowedKeys, input, false );
            if ( r == Okay && !okayKeyAlsoRepeating ) {
                aIO->flagWaitForKeyup();
            }
            return r;
        }

    private:

        void flagWaitForKeyup( uint16_t input ) {
            if ( input != Keys::None )
                aIO->flagWaitForKeyup();
        }

};

}
