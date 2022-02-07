#pragma once

namespace StarterPack { namespace UserInput {

    enum Keys {
        None          = 0b000000000,
        Okay          = 0b000000001,
        Left          = 0b000000010,
        Right         = 0b000000100,
        Up            = 0b000001000,
        Down          = 0b000010000,
        Cancel        = 0b000100000,
        Custom1       = 0b001000000,
        Custom2       = 0b010000000,
        Custom3       = 0b100000000,
        UpLeft        = Up | Left,
        UpRight       = Up | Right,
        DownLeft      = Down | Left,
        DownRight     = Down | Right,
        LeftRight     = Left | Right,
        UpDown        = Up | Down,
        LeftRightOkay = Left | Right | Okay,
        UpDownOkay    = Up | Down | Okay,
        All           = 0xFF // Left | Right | Up | Down | Okay
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

    void displayKey( Keys key ) {
        if ( key == Keys::None )
            Serial.print( "None " );
        else {
            if ( (key & Keys::Okay    ) != 0 ) Serial.print( "Okay" );
            if ( (key & Keys::Up      ) != 0 ) Serial.print( "Up" );
            if ( (key & Keys::Down    ) != 0 ) Serial.print( "Down" );
            if ( (key & Keys::Left    ) != 0 ) Serial.print( "Left" );
            if ( (key & Keys::Right   ) != 0 ) Serial.print( "Right" );
            if ( (key & Keys::Cancel  ) != 0 ) Serial.print( "Cancel" );
            if ( (key & Keys::Custom1 ) != 0 ) Serial.print( "Custom1" );
            if ( (key & Keys::Custom2 ) != 0 ) Serial.print( "Custom2" );
            if ( (key & Keys::Custom3 ) != 0 ) Serial.print( "Custom3" );
        }
        Serial.println();
    }

}}

//
//
//

#include <UserInputCommon.h>

namespace StarterPack { namespace UserInput {

    UserInputCommon *KeyHandler = nullptr;

    void assignHandler( UserInputCommon &handler ) {
        KeyHandler = &handler;
    }

    // static Core core;

    // FOR DEBUGGING ONLY
    // inline static Keys processKeysCore( Keys allowedKeys, Keys input, bool issueWaitForKeyUp ) {
    //     return core.processKeysCore( allowedKeys, input, issueWaitForKeyUp );
    // }

    // inline static void assignKeys( DigitalIO *okayKey, DigitalIO *leftAndUpKey, DigitalIO *rightAndDownKey ) {
    //     core.assignKeys( okayKey, leftAndUpKey, rightAndDownKey );
    // }

    // inline static void assignKeys( DigitalIO *okayKey,
    // DigitalIO *leftKey, DigitalIO *rightKey, DigitalIO *upKey, DigitalIO *downKey ) {
    //     core.assignKeys( okayKey, leftKey, rightKey, upKey, downKey );
    // }

    // inline static void assignKeys( AnalogIO &analogKey,
    // int okayKeyValue,
    // int leftKeyValue, int rightKeyValue, int upKeyValue, int downKeyValue ) {
    //     core.assignKeys( analogKey, okayKeyValue, leftKeyValue, rightKeyValue, upKeyValue, downKeyValue );
    // }

    // inline static void assignKeys( MatrixKeypad &matrixKeypad,
    // int okayKeyId,
    // int leftKeyId, int rightKeyId, int upKeyId, int downKeyId ) {
    //     core.assignKeys( matrixKeypad, okayKeyId, leftKeyId, rightKeyId, upKeyId, downKeyId );
    // }

    inline static void allowDiagonals() {
        if ( KeyHandler == nullptr ) return;
        KeyHandler->allowDiagonals = true;
    }

    inline static void disallowDiagonals() {
        if ( KeyHandler == nullptr ) return;
        KeyHandler->allowDiagonals = false;
    }

    inline static bool isOkayPressed( bool issueWaitForKeyUp = true ) {
        if ( KeyHandler == nullptr ) return false;
        return KeyHandler->isOkayPressed( issueWaitForKeyUp );
    }

    // inline static Keys getInput( Keys allowedKeys = Keys::All ) {
    //     return core.getInput( allowedKeys, true );
    // }

    inline static Keys getKeyDown( Keys allowedKeys = Keys::All ) {
        if ( KeyHandler == nullptr ) return Keys::None;
        return KeyHandler->getKeyDown( allowedKeys );
    }

    inline static Keys getContinuousKey( Keys allowedKeys = Keys::All ) {
        if ( KeyHandler == nullptr ) return Keys::None;
        return KeyHandler->getContinuousKey( allowedKeys );
    }

    // inline static Keys getContinuousInput( Keys allowedKeys = Keys::All ) {
    //     return core.getContinuousInput( allowedKeys );
    // }

    inline static Keys getRepeatingKey( Keys allowedKeys = Keys::All, bool okayKeyAlsoRepeating = false ) {
        if ( KeyHandler == nullptr ) return Keys::None;
        return KeyHandler->getRepeatingKey( allowedKeys, okayKeyAlsoRepeating );
    }

    inline static Keys getRepeatingKey( bool okayKeyAlsoRepeating, Keys allowedKeys = Keys::All ) {
        if ( KeyHandler == nullptr ) return Keys::None;
        return KeyHandler->getRepeatingKey( allowedKeys, okayKeyAlsoRepeating );
    }

    // inline static Keys getRepeatingInput( Keys allowedKeys = Keys::All, bool issueWaitForKeyUpToOkayKey = true ) {
    //     return core.getRepeatingInput( allowedKeys, issueWaitForKeyUpToOkayKey );
    // }

    // inline static Keys getRepeatingInput( bool issueWaitForKeyUpToOkayKey, Keys allowedKeys = Keys::All ) {
    //     return core.getRepeatingInput( allowedKeys, issueWaitForKeyUpToOkayKey );
    // }

    inline static void waitUntilNothingIsPressed() {
        if ( KeyHandler == nullptr ) return;
        KeyHandler->waitUntilNothingIsPressed();
    }

    inline static void waitForAnyKey() {
        // hang it ???
        if ( KeyHandler == nullptr ) return;
        KeyHandler->waitForAnyKey();
    }

}}
