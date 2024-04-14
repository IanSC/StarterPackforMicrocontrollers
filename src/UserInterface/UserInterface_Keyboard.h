#pragma once

#include <UserInterface/UserInterfaceAllKeys.h>
// #include <LCD/LCDInterface.h>
#include <spWDT.h>

namespace StarterPack {
    
namespace UserInterface {

    typedef UserInterfaceAllKeys::KEY KEY;
    static const KEY INACTIVE_KEY = UserInterfaceAllKeys::INACTIVE_KEY;

    //  difference between c[XXX] and k[XXX]
    //  c___ are constants
    //       default value to be used
    //  k___ are variables
    //       user settings to override default

    // try to use same as PCs
    //    https://docs.microsoft.com/en-us/office/vba/language/reference/user-interface-help/keycode-constants
    //    https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
    //    https://css-tricks.com/snippets/javascript/javascript-keycodes/
    //    https://en.wikipedia.org/wiki/Windows-1252
    //    https://keycode.info/
    // ... but keycodes for:
    //    left/up/right/down are: 0x25,0x26,0x27,0x28 / decimal 37,38,39,40
    //    which are ascii: %, &, ', (
    //    so just use anything non-printable ascii, below ( 0x20, 32 )
    //
    // since kXXX occupies space, anything not used in other modules were commented out

//
// CONSTANTS
//

    // static const KEY    cNONE      = UserInputDevice::cNONE; // 0x00;
    static constexpr KEY    cNONE      = 0x00;
    static constexpr KEY    cENTER     = 0x0D; // 13
    static constexpr KEY    cESCAPE    = 0x1B; // 27
    static constexpr KEY    cBACKSPACE = 0x08; //  8 
    // static constexpr KEY cTAB       = 0x09; //  9

    static constexpr KEY    cLEFT      = 0x01;
    static constexpr KEY    cUP        = 0x02;
    static constexpr KEY    cRIGHT     = 0x03;
    static constexpr KEY    cDOWN      = 0x04;
    // static constexpr KEY cPGUP      = ----;
    // static constexpr KEY cPGDOWN    = ----;

    static constexpr KEY    cINSERT    = 0x05;
    static constexpr KEY    cDELETE    = 0x06;

    static constexpr KEY    cNUMBERS   = 0x07; // alphanumeric editor: numbers
    static constexpr KEY    cSYMBOLS   = 0x10; // alphanumeric editor: symbols
    static constexpr KEY    cCHG_CASE  = 0x11; // alphanumeric editor: upper/lower case

    static constexpr KEY    cDECIMAL   = '.';
    static constexpr KEY    cQUESTION  = '?';
    static constexpr KEY    cMINUS     = '-';
    // static constexpr KEY cPLUS      = '+';
    // static constexpr KEY cMUTIPLY   = '*';
    // static constexpr KEY cDIVIDE    = '/';

//
// VARIABLES
//

    // can't change
    static constexpr
    KEY    kNONE      = INACTIVE_KEY;

    KEY    kENTER     = cENTER;
    KEY    kESCAPE    = cESCAPE;
    KEY    kBACKSPACE = cBACKSPACE;
    // KEY kTAB       = cTAB;

    KEY    kLEFT      = cLEFT;
    KEY    kUP        = cUP;
    KEY    kRIGHT     = cRIGHT;
    KEY    kDOWN      = cDOWN;
    // KEY kPGUP      = cPGUP;
    // KEY kPGDOWN    = cPGDOWN;

    KEY    kINSERT    = cINSERT;
    KEY    kDELETE    = cDELETE;

    KEY    kNUMBERS   = cNUMBERS;  // alphanumeric editor: numbers
    KEY    kSYMBOLS   = cSYMBOLS;  // alphanumeric editor: symbols
    KEY    kCHG_CASE  = cCHG_CASE; // alphanumeric editor: upper/lower case

    KEY    kDECIMAL   = cDECIMAL;
    KEY    kQUESTION  = cQUESTION;
    KEY    kMINUS     = cMINUS;
    // KEY kPLUS      = cPLUS;
    // KEY kMUTIPLY   = cMUTIPLY;
    // KEY kDIVIDE    = cDIVIDE;

//
// NUMPAD
//

    // keys 0..9 are expected to produce KEY '0' to '9'
    // these are hard coded inside routines, such as LCDEditorNumeric.h

    bool hasNumericKeypad = false;

//
// ASSIGN KEY HANDLER DEVICE
//

    UserInterfaceAllKeys * KeyHandler = nullptr;

    void assignKeyHandler( UserInterfaceAllKeys &handler ) {
        KeyHandler = &handler;
    }

    void assignKeyHandler( UserInterfaceAllKeys *handler ) {
        KeyHandler = handler;
    }

    void setDebounceStabilizeTimeInMs( uint16_t activeStateStabilizeTime = 50, uint16_t inactiveStateStabilizeTime = 10
    // , uint16_t minimum = 50 
    ) { 
        if ( KeyHandler == nullptr ) return;
        KeyHandler->setStabilizationTimeInMs(activeStateStabilizeTime, inactiveStateStabilizeTime);
        // KeyHandler->debouncerSettings->stabilizedTimePressedInMs = activeStateStabilizeTime;
        // KeyHandler->debouncerSettings->stabilizedTimeReleasedInMs = inactiveStateStabilizeTime;
    }

    void setDebounceDelayTimeInMs( uint16_t activeStateDelayTime = 0, uint16_t inactiveStateDelayTime = 0 ) {
        if ( KeyHandler == nullptr ) return;
        KeyHandler->setDebouncerDelayInMs(activeStateDelayTime, inactiveStateDelayTime);
        // KeyHandler->debouncerSettings->debounceDelayPressedInMs = activeStateDelayTime;
        // KeyHandler->debouncerSettings->debounceDelayReleasedInMs = inactiveStateDelayTime;
    }

    void setRepeatDelayAndRateInMs( uint16_t repeatDelay = 400, uint16_t repeatRate = 250 ) {
        if ( KeyHandler == nullptr ) return;
        KeyHandler->setRepeatSettingsInMs(repeatDelay, repeatRate);
        // KeyHandler->repeaterSettings->repeatDelayInMs = repeatDelay;
        // KeyHandler->repeaterSettings->repeatRateInMs = repeatRate;
    }

    void setMultiClickSettingsInMs( uint16_t maxInterval, uint16_t repeatRate = 0 ) {
        if ( KeyHandler == nullptr ) return;
        KeyHandler->setMultiClickSettingsInMs(maxInterval, repeatRate);
    }

//
// MISC - OLD ???
//

    // char *getContinuousKeys( char *allowedKeys = nullptr ) {
    //     if ( KeyHandler == nullptr ) return nullptr;
    //     return KeyHandler->getContinuousKeys( allowedKeys );
    // }
    KEY getContinuousKey( char *allowedKeys = nullptr ) {
        if ( KeyHandler == nullptr ) return 0;
        return KeyHandler->getNonDebouncedKey();
    }
    // KEY getContinuousKey( char *allowedKeys = nullptr ) {
    //     if ( KeyHandler == nullptr ) return 0;
    //     return KeyHandler->getContinuousKey( allowedKeys );
    // }

    void flagWaitForKeyup() {}
    // void flagWaitForKeyup() {
    //     if ( KeyHandler == nullptr ) return;
    //     return KeyHandler->flagWaitForKeyup();
    // }

    // void flagWaitForKeyup( uint8_t key ) {
    //     if ( KeyHandler == nullptr ) return;
    //     return KeyHandler->flagWaitForKeyupSpecific( key );
    // }

//
// FROM DEBOUNCER: UserInterfaceDebouncer
//

    KEY getNonDebouncedKey() {
        if ( KeyHandler == nullptr ) return INACTIVE_KEY;
        return KeyHandler->getNonDebouncedKey();
    }

    KEY getDebouncedKey() {
        if ( KeyHandler == nullptr ) return INACTIVE_KEY;
        return KeyHandler->getDebouncedKey();
    }

    KEY getKeyDown() {
        if ( KeyHandler == nullptr ) return INACTIVE_KEY;
        return KeyHandler->getKeyDown();
    }

    KEY getKeyUp() {
        if ( KeyHandler == nullptr ) return INACTIVE_KEY;
        return KeyHandler->getKeyUp();
    }

    bool isKeyPressed( KEY key ) {
        if ( KeyHandler == nullptr ) return false;
        return KeyHandler->isKeyPressed( key );
    }

    void waitUntilNothingIsPressed() {
        if ( KeyHandler == nullptr ) return;
        KeyHandler->waitUntilNothingIsPressed();
    }

    KEY waitForAnyKeyPressed() {
        if ( KeyHandler == nullptr ) {
            // hang it ???
            // Serial.println( "waitForAnyKeyPressed(): hang" );
            while( true ) {
                feedTheDog();
                delay( 100 );
            }
        }
        return KeyHandler->waitForAnyKeyPressed();
    }

    KEY waitForAnyKeyReleased() {
        if ( KeyHandler == nullptr ) {
            // hang it ???
            // Serial.println( "waitForAnyKeyReleased(): hang" );
            while( true ) {
                feedTheDog();
                delay( 100 );
            }
        }
        // if ( KeyHandler == nullptr ) while( true ); // hang it ???
        return KeyHandler->waitForAnyKeyReleased();
    }

    void clearDebouncedState() {
        if ( KeyHandler == nullptr ) return;
        KeyHandler->clearDebouncedState();
    }

    void skipDebounceDelay_XXX() {
        if ( KeyHandler == nullptr ) return;
        KeyHandler->skipDebounceDelay_XXX();
    }

    void cancelDebounceDelay_XXX() {
        if ( KeyHandler == nullptr ) return;
        KeyHandler->cancelDebounceDelay_XXX();
    }

//
// FROM MULTI-CLICK: UserInterfaceMultiClick
//

    KEY getMultiClickKey() {
        if ( KeyHandler == nullptr ) return INACTIVE_KEY;
        return KeyHandler->getMultiClick_Key();
    }

    bool getMultiClickInfo(uint8_t &count, bool &isLongPressed, bool &isRepeated) {
        if ( KeyHandler == nullptr ) {
            count         = 0;
            isLongPressed = false;
            isRepeated    = false;
            return false;
        }
        count         = KeyHandler->multiClick_count;
        isLongPressed = KeyHandler->multiClick_isLongPressed;
        isRepeated    = KeyHandler->multiClick_isRepeated;
        return true;
    }

    uint8_t getMultiClickCount() {
        if ( KeyHandler == nullptr ) return 0;
        return KeyHandler->multiClick_count;
    }

    bool getMultiClickIsLongPressed() {
        if ( KeyHandler == nullptr ) return false;
        return KeyHandler->multiClick_isLongPressed;
    }

    bool getMultiClickIsRepeated() {
        if ( KeyHandler == nullptr ) return false;
        return KeyHandler->multiClick_isRepeated;
    }

//
// FROM REPEATED: UserInterfaceRepeated
//

    KEY getRepeatingKey() {
        if ( KeyHandler == nullptr ) return INACTIVE_KEY;
        return KeyHandler->getRepeatingKey();
    }

    void skipRepeatingCurrentKey() {
        if ( KeyHandler == nullptr ) return;
        return KeyHandler->skipRepeatingCurrentKey();
    }

    // KEY getRepeatingKeyExcept( KEY nonRepeatingKey, char *allowedKeys = nullptr ) {
    //     if ( KeyHandler == nullptr ) return 0;
    //     return KeyHandler->getRepeatingKeyExcept( nonRepeatingKey, allowedKeys );
    // }

    // KEY getRepeatingKeyExcept( char *nonRepeatingKeys, char *allowedKeys = nullptr ) {
    //     if ( KeyHandler == nullptr ) return 0;
    //     return KeyHandler->getRepeatingKeyExcept( nonRepeatingKeys, allowedKeys );
    // }

//
// EDITORS
//

    // alphanumericEditor *alphaEditor = nullptr;
    // numericEditor *numberEditor = nullptr;

    // void assignAlphaEditor( alphanumericEditor &alphaEditorToUse ) {
    //     alphaEditor = &alphaEditorToUse;
    // }

}

}
