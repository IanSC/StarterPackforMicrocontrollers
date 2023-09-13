#pragma once

// #include <UserInterface/UserInterfaceAllKeys.h>
#include <LCD/LCDInterface.h>
#include <UserInterface/UserInterface_Keyboard.h>

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

    // void updateBufferedLCD() {
    //     // draw for buffered screens
    //     if ( LCD == nullptr ) return;
    //     LCD->refreshPartial();
    //     // LCD->update();
    //     // if ( !LCD->isBuffered() ) return;
    //     // ( (LCDBuffered*) LCD )->update();
    // }

    // void displayAllBufferedLCD() {
    //     // draw for buffered screens
    //     if ( LCD == nullptr ) return;
    //     LCD->refresh();
    //     // LCD->displayAll();
    //     // if ( !LCD->isBuffered() ) return;
    //     // ( (LCDBuffered*) LCD )->update();
    // }

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
        LCD->refresh();
        // LCD->displayAll();
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
        LCD->refresh();
        // LCD->displayAll();
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

}

}
