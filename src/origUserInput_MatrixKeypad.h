// #pragma once
// #include <UserInputCommon.h>
// #include <UserInput.h>
// #include <MatrixKeypad.h>
// using namespace StarterPack::UserInput;
// // namespace ui = StarterPack::UserInput;

// namespace StarterPack {

// class origUserInputMatrixKeypad : public UserInputCommon {

//     private:

//         // MATRIX KEYPAD
//         MatrixKeypad *keypad = nullptr;
//         int buttonId[MAX_KEYS];

//     public:

//         origUserInputMatrixKeypad( MatrixKeypad &matrixKeypad ) {
//             this->keypad = &matrixKeypad;
//         }

//         origUserInputMatrixKeypad( MatrixKeypad &matrixKeypad,
//         int okayKeyId,
//         int leftKeyId, int rightKeyId, int upKeyId, int downKeyId ) {
//             this->keypad = &matrixKeypad;
//             buttonId[0] = okayKeyId;
//             buttonId[1] = leftKeyId;
//             buttonId[2] = rightKeyId;
//             buttonId[3] = upKeyId;
//             buttonId[4] = downKeyId;
//             // checkCommonKeys();
//         }

//         origUserInputMatrixKeypad( MatrixKeypad &matrixKeypad, int keypadId, uint8_t slot ) {
//             this->keypad = &matrixKeypad;
//             if ( slot < MAX_KEYS )
//                 buttonId[slot] = keypadId;
//         }

//         void addKey( int keypadValue, Keys key ) {
//             uint8_t numberOfBits = countBits( static_cast<uint16_t>( key ) );
//             if ( numberOfBits != 1 ) return;
//             // https://stackoverflow.com/a/31393298
//             uint8_t bitPosition = __builtin_ctzl( key );
//             buttonId[bitPosition] = keypadValue;
//         }
        
//         void addKey( int keypadValue, uint8_t slot ) {
//             if ( slot < MAX_KEYS )
//                 buttonId[slot] = keypadValue;
//         }
        
//     private:

//         void checkCommonKeys() {
//             commonUpLeft    = ( buttonId[SLOT_LEFT]  == buttonId[SLOT_UP]   );
//             commonDownRight = ( buttonId[SLOT_RIGHT] == buttonId[SLOT_DOWN] );
//         }

//     //
//     //
//     //
//     public:

//         Keys getKeyDown( Keys allowedKeys = Keys::All ) {
//             //if ( ioMatrixKeypad == nullptr ) return Keys::None;
//             Keys input = Keys::None;
//             int8_t bNo = keypad->getKeyDown();
//             if ( bNo != 0 ) {
//                 for( int i = 0 ; i < MAX_KEYS ; i++ ) {
//                     if ( buttonId[i] == bNo ) {
//                         input = static_cast<Keys>( input | (1<<i) );
//                         break;
//                     }
//                 }
//             }
//             return processKeysCore( allowedKeys, input, false );
//         }

//         Keys getContinuousKey( Keys allowedKeys = Keys::All ) {
//             //if ( ioMatrixKeypad == nullptr ) return Keys::None;
//             Keys input = Keys::None;
//             int8_t bNo = keypad->getContinuousKey();
//             if ( bNo != 0 ) {
//                 for( int i = 0 ; i < MAX_KEYS ; i++ ) {
//                     if ( buttonId[i] == bNo ) {
//                         input = static_cast<Keys>( input | (1<<i) );
//                         break;
//                     }
//                 }
//             }
//             return processKeysCore( allowedKeys, input, false );
//         }

//         Keys getRepeatingKey( bool okayKeyAlsoRepeating, Keys allowedKeys = Keys::All ) {
//             return getRepeatingKey( allowedKeys, okayKeyAlsoRepeating );
//         }

//         Keys getRepeatingKey( Keys allowedKeys = Keys::All, bool okayKeyAlsoRepeating = false ) {                
//             //if ( ioMatrixKeypad == nullptr ) return Keys::None;
//             Keys input = Keys::None;
//             int8_t bNo = keypad->getRepeatingKey();
//             if ( bNo != 0 ) {
//                 for( int i = 0 ; i < MAX_KEYS ; i++ ) {
//                     if ( buttonId[i] == bNo ) {
//                         input = static_cast<Keys>( input | (1<<i) );
//                         break;
//                     }
//                 }
//             }

//             Keys r = processKeysCore( allowedKeys, input, false );
//             if ( r == Okay && !okayKeyAlsoRepeating ) {
//                 keypad->flagWaitForKeyup();
//             }
//             return r;
//         }

//     private:

//         void flagWaitForKeyup( uint16_t input ) {
//             if ( input != Keys::None )
//                 keypad->flagWaitForKeyup();
//         }

// };

// }
