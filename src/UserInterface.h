#pragma once
#include <UserInputDevice.h>
#include <LCDInterface.h>
// #include <LCDEditorAlpha.h>
// #include <LCDEditorNumeric.h>

namespace StarterPack {
    
namespace UserInterface {

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

        //
        // FULL SCREEN
        //

        // defined later
        uint8_t waitForAnyKey();

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

        uint8_t showLinesAndWait( const char *row1, const char *row2 = nullptr, const char *row3 = nullptr, const char *row4 = nullptr ) {
            if ( LCD == nullptr ) return 0;
            LCD->showLines( row1, row2, row3, row4 );
            LCD->displayAll();
            return waitForAnyKey();
        }

        inline uint8_t wait_01234567890123456789( const char *row1, const char *row2 = nullptr, const char *row3 = nullptr, const char *row4 = nullptr ) {
            return showLinesAndWait( row1, row2, row3, row4 );
        }

        inline uint8_t wait_12345678901234567890( const char *row1, const char *row2 = nullptr, const char *row3 = nullptr, const char *row4 = nullptr ) {
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
            LCD->print( std::string( L, ' ' ).c_str() );
            LCD->print( header );
            LCD->print( std::string( R, ' ' ).c_str() );
            LCD->printStrN( decorator+decoLen/2, decoLen-decoLen/2 );
        }

        uint8_t dialogBox( const char *header,
        const char *msg1 = nullptr, const char *msg2 = nullptr, const char *msg3 = nullptr,
        const char *headerDecorator = "((()))" ) {
            if ( LCD == nullptr ) return 0;
            LCD->clear();
            showHeader( 0, header, headerDecorator );
            if ( msg1 != nullptr ) LCD->printStrAtRow( 1, msg1 );
            if ( msg2 != nullptr ) LCD->printStrAtRow( 2, msg2 );
            if ( msg3 != nullptr ) LCD->printStrAtRow( 3, msg3 );
            LCD->displayAll();
            return waitForAnyKey();
        }

        inline uint8_t dbox_12345678901234567890( const char *header,
        const char *row2 = nullptr, const char *row3 = nullptr, const char *row4 = nullptr, 
        const char *headerDecorator = "((()))" ) {
            return dialogBox( header, row2, row3, row4, headerDecorator );
        }

        uint8_t showError( uint8_t errSource, uint8_t errCode,
        const char *errMsg,
        const char *headerDecorator = "((()))" ) {
            if ( LCD == nullptr ) return 0;
            LCD->clear();
            char header[11]; // "ERROR xxyy"
            snprintf( header, 11, "ERROR %02x%02x", errSource, errCode );
            return dialogBox( header, nullptr, errMsg, nullptr );
        }

        uint8_t showError( uint8_t errSource, uint8_t errCode,
        const char *errMsg1, const char *errMsg2, const char *errMsg3 = nullptr,
        const char *headerDecorator = "((()))" ) {
            if ( LCD == nullptr ) return 0;
            LCD->clear();
            char header[11];
            snprintf( header, 11, "ERROR %02x%02x", errSource, errCode );
            return dialogBox( header, errMsg1, errMsg2, errMsg3 );
        }

        inline uint8_t eror_12345678901234567890( uint8_t errSource, uint8_t errCode,
        const char *errMsg1 = nullptr, const char *errMsg2 = nullptr, const char *errMsg3 = nullptr, 
        const char *headerDecorator = "((()))" ) {
            return showError( errSource, errCode, errMsg1, errMsg2, errMsg3, headerDecorator );
        }

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

        static const uint8_t    cNONE      = 0x00;
        static const uint8_t    cENTER     = 0x0D; // 13
        static const uint8_t    cESCAPE    = 0x1B; // 27
        static const uint8_t    cBACKSPACE = 0x08; //  8 
        // static const uint8_t cTAB       = 0x09; //  9

        static const uint8_t    cLEFT      = 0x01;
        static const uint8_t    cUP        = 0x02;
        static const uint8_t    cRIGHT     = 0x03;
        static const uint8_t    cDOWN      = 0x04;
        // static const uint8_t cPGUP      = ----;
        // static const uint8_t cPGDOWN    = ----;

        static const uint8_t    cINSERT    = 0x05;
        static const uint8_t    cDELETE    = 0x06;

        static const uint8_t    cNUMBERS   = 0x07; // alphanumeric editor: numbers
        static const uint8_t    cSYMBOLS   = 0x10; // alphanumeric editor: symbols
        static const uint8_t    cCHG_CASE  = 0x11; // alphanumeric editor: upper/lower case

        static const uint8_t    cDECIMAL   = '.';
        static const uint8_t    cQUESTION  = '?';
        static const uint8_t    cMINUS     = '-';
        // static const uint8_t cPLUS      = '+';
        // static const uint8_t cMUTIPLY   = '*';
        // static const uint8_t cDIVIDE    = '/';

        static const
        uint8_t    kNONE      = cNONE;
        uint8_t    kENTER     = cENTER;
        uint8_t    kESCAPE    = cESCAPE;
        uint8_t    kBACKSPACE = cBACKSPACE;
        // uint8_t kTAB       = cTAB;

        uint8_t    kLEFT      = cLEFT;
        uint8_t    kUP        = cUP;
        uint8_t    kRIGHT     = cRIGHT;
        uint8_t    kDOWN      = cDOWN;
        // uint8_t kPGUP      = cPGUP;
        // uint8_t kPGDOWN    = cPGDOWN;

        uint8_t    kINSERT    = cINSERT;
        uint8_t    kDELETE    = cDELETE;

        uint8_t    kNUMBERS   = cNUMBERS;  // alphanumeric editor: numbers
        uint8_t    kSYMBOLS   = cSYMBOLS;  // alphanumeric editor: symbols
        uint8_t    kCHG_CASE  = cCHG_CASE; // alphanumeric editor: upper/lower case

        uint8_t    kDECIMAL   = cDECIMAL;
        uint8_t    kQUESTION  = cQUESTION;
        uint8_t    kMINUS     = cMINUS;
        // uint8_t kPLUS      = cPLUS;
        // uint8_t kMUTIPLY   = cMUTIPLY;
        // uint8_t kDIVIDE    = cDIVIDE;

    //
    // ASSIGN KEY HANDLER DEVICE
    //

        UserInputDevice *KeyHandler = nullptr;

        void assignKeyHandler( UserInputDevice &handler ) {
            KeyHandler = &handler;
        }

        void setRepeatDelayAndRateInMs( uint16_t repeatDelay = 400, uint16_t repeatRate = 250 ) {
            if ( KeyHandler == nullptr ) return;
            KeyHandler->setRepeatDelayAndRateInMs( repeatDelay, repeatRate );
        }

    //
    // KEYS
    //

        char *getContinuousKeys( char *allowedKeys = nullptr ) {
            if ( KeyHandler == nullptr ) return nullptr;
            return KeyHandler->getContinuousKeys( allowedKeys );
        }

        uint8_t getContinuousKey( char *allowedKeys = nullptr ) {
            if ( KeyHandler == nullptr ) return 0;
            return KeyHandler->getContinuousKey( allowedKeys );
        }

        uint8_t getKeyDown( char *allowedKeys = nullptr ) {
            if ( KeyHandler == nullptr ) return 0;
            return KeyHandler->getKeyDown( allowedKeys );
        }

        uint8_t getKeyUp( char *allowedKeys = nullptr ) {
            if ( KeyHandler == nullptr ) return 0;
            return KeyHandler->getKeyUp( allowedKeys );
        }

        uint8_t getRepeatingKey( char *allowedKeys = nullptr ) {
            if ( KeyHandler == nullptr ) return 0;
            return KeyHandler->getRepeatingKey( allowedKeys );
        }

        uint8_t getRepeatingKeyExcept( uint8_t nonRepeatingKey, char *allowedKeys = nullptr ) {
            if ( KeyHandler == nullptr ) return 0;
            return KeyHandler->getRepeatingKeyExcept( nonRepeatingKey, allowedKeys );
        }

        uint8_t getRepeatingKeyExcept( char *nonRepeatingKeys, char *allowedKeys = nullptr ) {
            if ( KeyHandler == nullptr ) return 0;
            return KeyHandler->getRepeatingKeyExcept( nonRepeatingKeys, allowedKeys );
        }

        void flagWaitForKeyup() {
            if ( KeyHandler == nullptr ) return;
            return KeyHandler->flagWaitForKeyup();
        }

        // void flagWaitForKeyup( uint8_t key ) {
        //     if ( KeyHandler == nullptr ) return;
        //     return KeyHandler->flagWaitForKeyupSpecific( key );
        // }

    //
    // BASIC
    //

        bool isKeyPressed( uint8_t key, bool issueWaitForKeyUp = true ) {
            if ( KeyHandler == nullptr ) return false;
            return KeyHandler->isKeyPressed( key, issueWaitForKeyUp );
        }

        void waitUntilNothingIsPressed() {
            if ( KeyHandler == nullptr ) return;
            KeyHandler->waitUntilNothingIsPressed();
        }

        uint8_t waitForAnyKeyOnly() {
            if ( KeyHandler == nullptr ) while( true ); // hang it ???
            // if ( KeyHandler == nullptr ) return;
            return KeyHandler->waitForAnyKeyOnly();
        }

        uint8_t waitForAnyKey() {
            if ( KeyHandler == nullptr ) while( true ); // hang it ???
            return KeyHandler->waitForAnyKey();
        }

    //
    // EDITORS
    //

        // alphanumericEditor *alphaEditor = nullptr;
        // numericEditor *numberEditor = nullptr;

        // void assignAlphaEditor( alphanumericEditor &alphaEditorToUse ) {
        //     alphaEditor = &alphaEditorToUse;
        // }

}}
