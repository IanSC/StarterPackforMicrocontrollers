#pragma once

#include <UserInterface/UserInterfaceAllKeys.h>
#include <LCD/LCDInterface.h>

namespace StarterPack {
    
namespace UserInterface {

    typedef UserInterfaceAllKeys::KEY KEY;
    static const KEY INACTIVE_KEY = UserInterfaceAllKeys::INACTIVE_KEY;

    //
    // LCD
    //

        LCDInterface *LCD = nullptr;

        void assignLCD( LCDInterface &lcdToUse ) {
            LCD = &lcdToUse;
        }

        bool hasScreen() {
            return LCD != nullptr;
        }

        void updateBufferedLCD() {
            // draw for buffered screens
            if ( LCD == nullptr ) return;
            LCD->update();
            // if ( !LCD->isBuffered() ) return;
            // ( (LCDBuffered*) LCD )->update();
        }

        void displayAllBufferedLCD() {
            // draw for buffered screens
            if ( LCD == nullptr ) return;
            LCD->displayAll();
            // if ( !LCD->isBuffered() ) return;
            // ( (LCDBuffered*) LCD )->update();
        }

        //
        // FULL SCREEN
        //

        // defined later
        KEY waitForAnyKeyPressed();

        void showLines( const char *row1, const char *row2 = nullptr, const char *row3 = nullptr, const char *row4 = nullptr ) {
            if ( LCD == nullptr ) return;
            LCD->showLines( row1, row2, row3, row4 );
        }
    
        inline void show_12345678901234567890( const char *row1, const char *row2 = nullptr, const char *row3 = nullptr, const char *row4 = nullptr ) {
            // wtf for?
            //
            // StarterPack::UserInterface
            // show_12345678901234567890(
            //     "can this fit in 20 chars?",
            //     "obviously not" );
            showLines( row1, row2, row3, row4 );
        }

        inline void show_01234567890123456789( const char *row1, const char *row2 = nullptr, const char *row3 = nullptr, const char *row4 = nullptr ) {
            // wtf for?
            //
            // StarterPack::UserInterface
            // show_01234567890123456789(
            //     "what col# is X on?",
            //     "it's 13" );
            showLines( row1, row2, row3, row4 );
        }

        KEY showLinesAndWait( const char *row1, const char *row2 = nullptr, const char *row3 = nullptr, const char *row4 = nullptr ) {
            if ( LCD == nullptr ) return 0;
            LCD->showLines( row1, row2, row3, row4 );
            LCD->displayAll();
            return waitForAnyKeyPressed();
        }

        inline KEY wait_01234567890123456789( const char *row1, const char *row2 = nullptr, const char *row3 = nullptr, const char *row4 = nullptr ) {
            return showLinesAndWait( row1, row2, row3, row4 );
        }

        inline KEY wait_12345678901234567890( const char *row1, const char *row2 = nullptr, const char *row3 = nullptr, const char *row4 = nullptr ) {
            return showLinesAndWait( row1, row2, row3, row4 );
        }

        void showHeader( uint8_t row, const char *header, const char *decorator = "((()))" ) {
            if ( LCD == nullptr ) return;
            LCD->setCursor( 0, row );
            int decoLen = strlen( decorator );
            int len = strlen( header ) + decoLen;
            int R = ( LCD->maxColumns - len ) / 2;
            if ( R < 0 ) R = 0;
            int L = LCD->maxColumns - len - R;
            if ( L < 0 ) L = 0;
            LCD->printStrN( decorator, decoLen/2 );
            LCD->printCharsN( ' ', L );
            // LCD->print( std::string( L, ' ' ).c_str() );
            LCD->print( header );
            LCD->printCharsN( ' ', R );
            // LCD->print( std::string( R, ' ' ).c_str() );
            LCD->printStrN( decorator+decoLen/2, decoLen-decoLen/2 );
        }

    //
    // DIALOG BOX
    //

        // void dialogBoxDisplayOnly( const char *header,
        // const char *msg1 = nullptr, const char *msg2 = nullptr, const char *msg3 = nullptr,
        // const char *headerDecorator = "((()))" ) {
        //     if ( LCD == nullptr ) return;
        //     LCD->clear();
        //     showHeader( 0, header, headerDecorator );
        //     if ( msg1 != nullptr ) LCD->printStrAtRow( 1, msg1 );
        //     if ( msg2 != nullptr ) LCD->printStrAtRow( 2, msg2 );
        //     if ( msg3 != nullptr ) LCD->printStrAtRow( 3, msg3 );
        //     LCD->displayAll();
        //     // return waitForAnyKey();
        // }

        // 3-liner text dialog, display only
        void showDialog3( const char *header,
        const char *msg1 = nullptr, const char *msg2 = nullptr, const char *msg3 = nullptr,
        const char *headerDecorator = "((()))" ) {
            if ( LCD == nullptr ) return;
            LCD->clear();
            showHeader( 0, header, headerDecorator );
            if ( msg1 != nullptr ) LCD->printStrAtRow( 1, msg1 );
            if ( msg2 != nullptr ) LCD->printStrAtRow( 2, msg2 );
            if ( msg3 != nullptr ) LCD->printStrAtRow( 3, msg3 );
            LCD->displayAll();
            // return waitForAnyKey();
        }

        // 3-liner text dialog, wait for any key
        inline KEY promptDialog3( const char *header,
        const char *msg1 = nullptr, const char *msg2 = nullptr, const char *msg3 = nullptr,
        const char *headerDecorator = "((()))" ) {
            if ( LCD == nullptr ) return INACTIVE_KEY;
            showDialog3( header, msg1, msg2, msg3, headerDecorator );
            // if ( LCD == nullptr ) return 0;
            // LCD->clear();
            // showHeader( 0, header, headerDecorator );
            // if ( msg1 != nullptr ) LCD->printStrAtRow( 1, msg1 );
            // if ( msg2 != nullptr ) LCD->printStrAtRow( 2, msg2 );
            // if ( msg3 != nullptr ) LCD->printStrAtRow( 3, msg3 );
            // LCD->displayAll();
            return waitForAnyKeyPressed();
        }

        // 1-liner text dialog, display only
        inline void showDialog1( const char *header,
        const char *msg = nullptr,
        const char *headerDecorator = "((()))" ) {
            showDialog3( header, nullptr, msg, nullptr );
        }

        // 1-liner text dialog, wait for any key
        inline KEY promptDialog1( const char *header,
        const char *msg = nullptr,
        const char *headerDecorator = "((()))" ) {
            if ( LCD == nullptr ) return INACTIVE_KEY;
            showDialog3( header, nullptr, msg, nullptr );
            return waitForAnyKeyPressed();
        }

        inline KEY dbox_12345678901234567890( const char *header,
        const char *row2 = nullptr, const char *row3 = nullptr, const char *row4 = nullptr, 
        const char *headerDecorator = "((()))" ) {
            return promptDialog3( header, row2, row3, row4, headerDecorator );
        }

    //
    // ERROR CODES
    //

        // 3-liner error code dialog, display only
        void showErrorDialog3( uint8_t errSource, uint8_t errCode,
        const char *errMsg1 = nullptr, const char *errMsg2 = nullptr, const char *errMsg3 = nullptr,
        const char *headerDecorator = "((()))" ) {
            if ( LCD == nullptr ) return;
            LCD->clear();
            char header[11];
            snprintf( header, 11, "ERROR %02x%02x", errSource, errCode );
            showDialog3( header, errMsg1, errMsg2, errMsg3 );
        }

        // 3-liner error code dialog, wait for any key
        inline KEY promptErrorDialog3( uint8_t errSource, uint8_t errCode,
        const char *errMsg1 = nullptr, const char *errMsg2 = nullptr, const char *errMsg3 = nullptr,
        const char *headerDecorator = "((()))" ) {
            if ( LCD == nullptr ) return INACTIVE_KEY;
            showErrorDialog3( errSource, errCode, errMsg1, errMsg2, errMsg3, headerDecorator );
            return waitForAnyKeyPressed();
        }

        // 1-liner error code dialog, display only
        inline void showErrorDialog1( uint8_t errSource, uint8_t errCode,
        const char *errMsg = nullptr,
        const char *headerDecorator = "((()))" ) {
            showErrorDialog3( errSource, errCode, nullptr, errMsg, nullptr );
            // if ( LCD == nullptr ) return;
            // LCD->clear();
            // char header[11]; // "ERROR xxyy"
            // snprintf( header, 11, "ERROR %02x%02x", errSource, errCode );
            // displayOnlyDialog1( header, errMsg );
        }

        // 1-liner error code dialog, wait for any key
        KEY promptErrorDialog1( uint8_t errSource, uint8_t errCode,
        const char *errMsg = nullptr,
        const char *headerDecorator = "((()))" ) {
            if ( LCD == nullptr ) return INACTIVE_KEY;
            showErrorDialog3( errSource, errCode, nullptr, errMsg, nullptr );
            return waitForAnyKeyPressed();
            // if ( LCD == nullptr ) return 0;
            // displayOnlyErrorDialog1( errSource, errCode, errMsg, headerDecorator );
            // return waitForAnyKey();
        }

        inline KEY eror_12345678901234567890( uint8_t errSource, uint8_t errCode,
        const char *errMsg1 = nullptr, const char *errMsg2 = nullptr, const char *errMsg3 = nullptr, 
        const char *headerDecorator = "((()))" ) {
            return promptErrorDialog3( errSource, errCode, errMsg1, errMsg2, errMsg3, headerDecorator );
        }

    //
    // ERRORS NEW
    //

        // 1-liner error
        // uint8_t 
        // void showError1( uint8_t errSource, uint8_t errCode,
        // const char *errMsg,
        // const char *headerDecorator = "((()))" ) {
        //     if ( LCD == nullptr ) return;
        //     LCD->clear();
        //     char header[11]; // "ERROR xxyy"
        //     snprintf( header, 11, "ERROR %02x%02x", errSource, errCode );
        //     dialogBoxDisplayOnly( header, nullptr, errMsg, nullptr );
        //     // return dialogBox( header, nullptr, errMsg, nullptr );
        // }

        // uint8_t showErrorDialog1( uint8_t errSource, uint8_t errCode,
        // const char *errMsg,
        // const char *headerDecorator = "((()))" ) {
        //     showError1( errSource, errCode, errMsg, headerDecorator );
        //     return waitForAnyKey();
        //     // if ( LCD == nullptr ) return 0;
        //     // LCD->clear();
        //     // char header[11]; // "ERROR xxyy"
        //     // snprintf( header, 11, "ERROR %02x%02x", errSource, errCode );
        //     // return dialogBox( header, nullptr, errMsg, nullptr );
        // }

        // 3-liner error
        // uint8_t 
        // void showError3( uint8_t errSource, uint8_t errCode,
        // const char *errMsg1 = nullptr, const char *errMsg2 = nullptr, const char *errMsg3 = nullptr,
        // const char *headerDecorator = "((()))" ) {
        //     if ( LCD == nullptr ) return; // 0;
        //     LCD->clear();
        //     char header[11];
        //     snprintf( header, 11, "ERROR %02x%02x", errSource, errCode );
        //     dialogBoxDisplayOnly( header, errMsg1, errMsg2, errMsg3 );
        //     // return dialogBox( header, errMsg1, errMsg2, errMsg3 );
        // }

        // uint8_t showErrorDialog3( uint8_t errSource, uint8_t errCode,
        // const char *errMsg1, const char *errMsg2, const char *errMsg3 = nullptr,
        // const char *headerDecorator = "((()))" ) {            
        //     if ( LCD == nullptr ) return 0;
        //     showError3( errSource, errCode, errMsg1, errMsg2, errMsg3, headerDecorator );
        //     return waitForAnyKey();
        //     // LCD->clear();
        //     // char header[11];
        //     // snprintf( header, 11, "ERROR %02x%02x", errSource, errCode );
        //     // dialogBoxDisplayOnly( header, errMsg1, errMsg2, errMsg3 );
        //     // return dialogBox( header, errMsg1, errMsg2, errMsg3 );
        // }

        // inline uint8_t eror_12345678901234567890( uint8_t errSource, uint8_t errCode,
        // const char *errMsg1 = nullptr, const char *errMsg2 = nullptr, const char *errMsg3 = nullptr, 
        // const char *headerDecorator = "((()))" ) {
        //     return showError3( errSource, errCode, errMsg1, errMsg2, errMsg3, headerDecorator );
        // }

        // display only, don't wait for any key
        // void displayDialog( const char *header,
        // const char *msg1 = nullptr, const char *msg2 = nullptr, const char *msg3 = nullptr,
        // const char *headerDecorator = "((()))" ) {
        //     if ( LCD == nullptr ) return;
        //     LCD->clear();
        //     showHeader( 0, header, headerDecorator );
        //     if ( msg1 != nullptr ) LCD->printStrAtRow( 1, msg1 );
        //     if ( msg2 != nullptr ) LCD->printStrAtRow( 2, msg2 );
        //     if ( msg3 != nullptr ) LCD->printStrAtRow( 3, msg3 );
        //     LCD->displayAll();
        //     // return waitForAnyKey();
        // }

        // void displayDialogError( uint8_t errSource, uint8_t errCode,
        // const char *errMsg,
        // const char *headerDecorator = "((()))" ) {
        //     if ( LCD == nullptr ) return;
        //     LCD->clear();
        //     char header[11]; // "ERROR xxyy"
        //     snprintf( header, 11, "ERROR %02x%02x", errSource, errCode );
        //     displayDialog( header, nullptr, errMsg, nullptr );
        // }

        // void displayDialogError( uint8_t errSource, uint8_t errCode,
        // const char *errMsg1, const char *errMsg2, const char *errMsg3 = nullptr,
        // const char *headerDecorator = "((()))" ) {
        //     if ( LCD == nullptr ) return;
        //     LCD->clear();
        //     char header[11];
        //     snprintf( header, 11, "ERROR %02x%02x", errSource, errCode );
        //     displayDialog( header, errMsg1, errMsg2, errMsg3 );
        // }

    //
    // KEY ASSIGNMENT
    //

        // difference between c[XXX] and k[XXX]
        // - c are constants
        // - k are variables

        /*        
        namespace ui = StarterPack::UserInterface;
        void setup1() {
            MatrixKeypad kpad;
            kpad.assignRows( 2,3 );
            kpad.assignColumns( 4,5 );
            kpad.begin();

            // assign custom mapping to device and UserInterface
            // kpad will generate 'A', 'B', ... when pressed
            // which corresponds to the value in kENTER, kESCAPE, ...
            kpad.assignKeymap( "ABCD" );

            // note: use k[XXX]
            ui::assignKeyHandler( kpad );
            ui::kENTER  = 'A';
            ui::kESCAPE = 'B';
            ui::kUP     = 'C';
            ui::kDOWN   = 'D';
        }

        namespace ui = StarterPack::UserInterface;
        void setup2() {
            MatrixKeypad kpad;
            kpad.assignRows( 2,3 );
            kpad.assignColumns( 4,5 );
            kpad.begin();

            // assign default keys in UserInterface to device
            // kpad will generate cENTER, cESCAPE, ... when pressed
            // which corresponds to the value in kENTER, kESCAPE, ...

            // don't forget terminating NULL
            // note: use c[XXX]
            constexpr char map[] = { ui::cENTER, ui::cESCAPE, ui::cUP, ui::cDOWN, NULL };
            kpad.assignKeymap( map );

            ui::assignKeyHandler( kpad );
        }
        */

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

        // static const KEY    cNONE      = UserInputDevice::cNONE; // 0x00;
        static const KEY    cNONE      = 0x00;
        static const KEY    cENTER     = 0x0D; // 13
        static const KEY    cESCAPE    = 0x1B; // 27
        static const KEY    cBACKSPACE = 0x08; //  8 
        // static const KEY cTAB       = 0x09; //  9

        static const KEY    cLEFT      = 0x01;
        static const KEY    cUP        = 0x02;
        static const KEY    cRIGHT     = 0x03;
        static const KEY    cDOWN      = 0x04;
        // static const KEY cPGUP      = ----;
        // static const KEY cPGDOWN    = ----;

        static const KEY    cINSERT    = 0x05;
        static const KEY    cDELETE    = 0x06;

        static const KEY    cNUMBERS   = 0x07; // alphanumeric editor: numbers
        static const KEY    cSYMBOLS   = 0x10; // alphanumeric editor: symbols
        static const KEY    cCHG_CASE  = 0x11; // alphanumeric editor: upper/lower case

        static const KEY    cDECIMAL   = '.';
        static const KEY    cQUESTION  = '?';
        static const KEY    cMINUS     = '-';
        // static const KEY cPLUS      = '+';
        // static const KEY cMUTIPLY   = '*';
        // static const KEY cDIVIDE    = '/';

        static const
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
    // ASSIGN KEY HANDLER DEVICE
    //

        UserInterfaceAllKeys *KeyHandler = nullptr;

        void assignKeyHandler( UserInterfaceAllKeys &handler ) {
            KeyHandler  = &handler;
        }

        void setDebounceStabilizeTimeInMs( uint16_t activeStateStabilizeTime = 50, uint16_t inactiveStateStabilizeTime = 10, uint16_t minimum = 50 ) {
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

        void setMultiClickSettingsInMs(uint16_t maxInterval, uint16_t repeatRate = 0) {
            if ( KeyHandler == nullptr ) return;
            KeyHandler->setMultiClickSettingsInMs(maxInterval, repeatRate);
        }

    //
    // MISC
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
    // FROM DEBOUNCER
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
            if ( KeyHandler == nullptr ) while( true ); // hang it ???
            return KeyHandler->waitForAnyKeyPressed();
        }

        KEY waitForAnyKeyReleased() {
            if ( KeyHandler == nullptr ) while( true ); // hang it ???
            return KeyHandler->waitForAnyKeyReleased();
        }

        void skipDebounceDelay() {
            if ( KeyHandler == nullptr ) return;
            KeyHandler->skipDebounceDelay();
        }

        void cancelDebounceDelay() {
            if ( KeyHandler == nullptr ) return;
            KeyHandler->cancelDebounceDelay();
        }

    //
    // FROM MULTI-CLICK
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
    // FROM REPEATED
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
