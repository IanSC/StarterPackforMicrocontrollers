//  2022/01 - isc
//
//  LCD Buffered
//  ------------
//  - keep user updates in memory
//  - update only changes to LCD
//  - can use both hardwired or i2c LCDs
//
//  For Hardwired
//
//      const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
//      lcd = LCDBuffered_wired( rs, en, d4, d5, d6, d7 );
//
//  For I2C
//
//      lcd = LCDBuffered_i2c( 0x27 );
//
//  Functions
//
//      void begin( maxColumns, maxLines )
//      void reset()                         reset device to initial state
//
//  I2C Specific (ignored if wired LCD)
//    
//      void setTimeoutInMs( timeout )       set i2c bus timeout, ignored for wired lcd
//      void setFrequency( frequency )       set i2c bus frequency, ignored for wire lcd
//
//      bool verify()                        verify i2c status
//      ERROR_NO verifyWithError()           verify and get i2c error
//      bool recoverIfHasError()             recover from i2c errors
//      void setRecoveryThrottleInMs()       throttle between i2c error recoveries
//
//   Cursor Control (Printing Position)
//
//      bool cursorAutoCarriageReturn        move to next line after end of current line, default false
//      bool cursorAutoJumpToStart           move to (0,0) after end of screen, default false
//
//   Virtual Cursor
//
//      Since updates are buffered, displaying of the actual screen's cursor is complicated.
//      To overcome this, virtual cursor are used. Virtual cursor and printing position
//      are independent.
//
//      setVirtualCursor( col, row )          set virtual cursor position
//      char virtualCursorChar                lcd character for cursor, default 0xFF
//      cursorOn()         cursor()           show/hide cursor
//      cursorOff()        noCursor()
//      cursorBlinkOn()    blink()            blink cursor if shown
//      cursorBlinkOff()   noBlink()
//      uint16_t cursorBlinkOnDurationInMs    blink on duration, default 600
//      uint16_t cursorBlinkOffDurationInMs   blink off duration, default 400
//
//   Update
//
//      update()                              update the LCD, spend only [updateDurationInMs] time for each call
//                                            if not completely updated, continue from last call
//                                            if throttled, run only every few millisec even if continuously called
//      updateAllNow()                        update the LCD completely, no duration or throttling
//
//      uint16_t updateDurationInMs           time to spend in update(), default 10
//      setUpdateThrottleInMs( timeInMs )     time between actual updates
//      uint16_t getUpdateThrottleInMs        get current throttle
//
//      ex. lcd.setUpdateThrottleInMs(100);
//          lcd.updateDurationInMs = 50;
//          void loop() {
//              int val = analogRead( A0 );
//              lcd.setCursor(3,0);
//              lcd.print( val );
//
//              // will spend 50ms on each call
//              // after the whole screen is updated
//              //    will pause for 100ms before updating again
//              lcd.update();
//          }
//      
//   Multicore Support (ESP32)
//        
//      bool pauseUpdate()                      pause updates until resumed
//      void resumeUpdate()                     updates can continue
//      void setSemaphoreLockTimeoutInMs( t )   timeout for locks
//      useSingleBuffer()                       increased performance but no multicore support
//      useDoubleBuffer()                       required for multicore support, default
//
//      ESP32         single buffer is faster
//      Arduino Uno   same speed
//
//      ex. void core0() {
//              lcd.update();
//              delay(1000);
//          }
//          void core1() {
//              lcd.pauseUpdate();
//              lcd.setCursor(0,0);
//              lcd.print( "the quick brown fox" );
//              lcd.setCursor(0,1);
//              lcd.print( "jumped over the dog" );
//              lcd.resumeUpdate();
//              delay(500);
//              lcd.pauseUpdate();
//              lcd.setCursor(0,0);
//              lcd.print( "jumped over the dog" );
//              lcd.setCursor(0,1);
//              lcd.print( "the quick brown fox" );
//              lcd.resumeUpdate();
//          }

#pragma once

#if defined(ESP32)
    #include <Arduino.h>
#endif
#include <inttypes.h>

#include <LCD/LCDInterface.h>
#include <Utility/spSemaphore.h>

namespace StarterPack {

typedef uint8_t ERROR_NO;

class LCDBuffered : public LCDInterface {

    // for testing:
    friend char* LCDBuffered_internalAccess( LCDBuffered *lcd, uint8_t dataToGet );

    protected:

        // approach (1)
        // - slower write(), keep track of changes
        // - faster updates, already know if no updates
        //
        // approach (2)
        // - fast write()
        // - slower updates, check if there are no changes
        //
        // use approach (2)
        // - assume many write()
        // - calling update is on available processor time

        LCDInterface *lcd;
        bool mustDeletedLCD = true;

    public:

        ~LCDBuffered() {
            if ( userBuffer != nullptr ) delete userBuffer;
            if ( screenData != nullptr ) delete screenData;
            if ( btsBufferCore != nullptr ) delete btsBufferCore;
            if ( mustDeletedLCD ) delete lcd;
        }

        inline bool isBuffered() override { return true; }
        inline void displayAll() override { updateAllNow(); }

        LCDBuffered() {}

        inline LCDBuffered( LCDInterface &lcd, uint16_t throttleTimeInMs = 50, uint16_t updateDurationInMs = 10 ) {
            this->lcd = &lcd;
            mustDeletedLCD = false;
            // assume user already called begin() of lcd
            initBuffers();
            // ... otherwise must call our begin, not lcd's
            // ex 1. lcd = LCD_i2c();
            //       lcd.begin( 16, 2 );
            //       lcdBuff = LCDBuffered( lcd );
            // ex 2. lcd = LCD_i2c();
            //       lcdBuff = LCDBuffered( lcd );
            //       lcdBuff.begin( 16, 2 );
            setUpdateThrottleInMs( throttleTimeInMs );
            this->updateDurationInMs = updateDurationInMs;
        }

        inline void setTimeoutInMs( uint16_t timeOut ) override {
            lcd->setTimeoutInMs( timeOut );
        }

        inline void setFrequency( uint32_t frequency ) override {
            lcd->setFrequency( frequency );
        }

    //
    // BEGIN
    //
    public:

        inline void begin( uint8_t maxColumns, uint8_t maxRows, LCDInterface::charDotSize dotSize = LCDInterface::charDotSize::size5x8 ) override {
            lcd->begin( maxColumns, maxRows, dotSize );
            initBuffers();
        }

    //
    // BUFFERS
    //
    protected:
    
        uint16_t bufferSize = 0;

        // actual storage
        char *userBuffer    = nullptr; // user updates
        char *btsBufferCore = nullptr; // double buffer area
        char *screenData    = nullptr; // text already sent to LCD

        // (buffer to send) points to:
        // - userBuffer    (single buffer mode)
        // - btsBufferCore (double buffer mode)
        char *btsBuffer = nullptr;        

        enum bufferModes {
            singleBuffer, doubleBuffer
        };
        bufferModes bufferMode = doubleBuffer;
        
        void initBuffers() {

            // not initialized
            if ( lcd == nullptr ) return;
            if ( lcd->maxColumns == 0 || lcd->maxRows == 0 ) return;

            this->maxColumns = lcd->maxColumns;
            this->maxRows = lcd->maxRows;

            bufferSize = maxColumns * maxRows;
            if ( userBuffer    != nullptr ) delete userBuffer;
            if ( btsBufferCore != nullptr ) delete btsBufferCore;
            if ( screenData    != nullptr ) delete screenData;
            userBuffer    = new char[ bufferSize ];
            screenData    = new char[ bufferSize ];
            btsBufferCore = new char[ bufferSize ];

            memset( userBuffer, ' ', bufferSize );
            memset( screenData, ' ', bufferSize );

            btsBuffer = btsBufferCore;
            
            // fake to single, so can switch to double            
            bufferMode = singleBuffer;
            useDoubleBuffer();

            // if not properly initialized, will go offScreen
            // eg. lcd.begin not called, so maxColumns = maxRows = 0
            setCursor( 0, 0 );
        }

    public:

        // userBuffer - contains user updates 
        // screenData - contains data already sent to LCD 
        // btsBuffer  - data to be compared/sent to LCD
        //
        // Single Buffer
        // - userBuffer directly compared to screenData
        // - if using multicore, possible to display partial info
        //   ex. print at x=0, "hello world"
        //       current compare position at x=6
        //       so "world" will get sent first, then possibly trottled
        //
        // Double Buffer
        // - userBuffer copied to btsBuffer
        // - btsBuffer compared to screenData
        // - if using multicore, can use pauseUpdate()/resumeUpdate()
        //   so copying userBuffer to btsBuffer will get paused
        //   on resume will start updating from position 0

        void useSingleBuffer() {
            // userBuffer --> (compared) --> screenData
            if ( bufferMode == singleBuffer ) return;
            // point btsBuffer to userBuffer
            btsBuffer  = userBuffer;
            bufferMode = singleBuffer;
            btsMode = mStart;
            btsLastCompletedUpdate = millis() - btsThrottleInMs;
        }

        void useDoubleBuffer() {
            // userBuffer --> (copied) --> btsBufferCore --> (compared) --> screenData
            if ( bufferMode == doubleBuffer ) return;
            // point btsBuffer to btsBufferCore
            // copy first current userBuffer, best effort to lock
            // xSemaphoreTake( userBufferSem, semLockTimeout );
            userBufferSem.take( SEM_ID_copyUserBuffer );
            memcpy( btsBufferCore, userBuffer, bufferSize );
            userBufferSem.release( SEM_ID_copyUserBuffer );
            // xSemaphoreGive( userBufferSem );
            btsBuffer  = btsBufferCore;
            bufferMode = doubleBuffer;
            btsMode = mStart;
            btsLastCompletedUpdate = millis() - btsThrottleInMs;
        }

    //
    // VERIFY / RECOVERY
    //
    public:

        bool verify() override {
            return lcd->verify();
        }
        ERROR_NO verifyWithError() override {
            return lcd->verifyWithError();
        }
        
        bool recoverIfHasError() override {
            bool r = lcd->recoverIfHasError();
            if ( r ) reset();
            return r;
        }
        inline void setRecoveryThrottleInMs( uint16_t delay ) override {
            lcd->setRecoveryThrottleInMs( delay );
        }
        void reset() override {
            // if LCD is reset, will revert to blanks
            memset( screenData, ' ', bufferSize );
            updateAllNow();
        }

    //
    // USER'S UPDATES
    //
    private:
        
        uint8_t userCursorX = 0, userCursorY = 0;
        bool offscreen = true;
        // SemaphoreHandle_t userBufferSem = xSemaphoreCreateMutex();
        // TickType_t semLockTimeout = portTICK_PERIOD_MS * 50;
        spSemaphore userBufferSem = spSemaphore( 50 );

        const uint8_t SEM_ID_pauseResume = 10;
        const uint8_t SEM_ID_copyUserBuffer = 20;

    public:

        //  Use pauseUpdate() and resumeUpdate(), if:
        //  - using different core/thread for:
        //      1. user operations, eg. print(), write()
        //      2. screen updates
        //  - screen update throttling has been set
        //
        //  to prevent printing strings halfway
        //  ex. print( "hello" ), might pause in middle of "hel"
        //
        //      scDoubleBuffer db( &lcd, 16, 2 );
        //      db.setUpdateThrottle( 50 );
        //      core1() {
        //          db.pauseUpdate();
        //          db.print( "long string" );
        //          db.resumeUpdate();
        //      } 
        //      core0() {
        //          db.update();
        //      }

        void setSemaphoreLockTimeoutInMs( uint16_t waitingTime ) {
            // semLockTimeout = portTICK_PERIOD_MS * waitingTime;
            userBufferSem.timeoutInMs = waitingTime;
        }

        bool pauseUpdate() {
            // pause succeeding operations (eg. print, write ) from propagating to screen
            // previous pending updates will proceed
            if ( bufferMode == doubleBuffer ) {
                // xSemaphoreTake( userBufferSem, portMAX_DELAY );
                // return ( xSemaphoreTake( userBufferSem, semLockTimeout ) == pdTRUE );
                return userBufferSem.take( SEM_ID_pauseResume );
            }
            return true;
        }

        void resumeUpdate() {
            // resume updating
            if ( bufferMode == doubleBuffer )
                // xSemaphoreGive( userBufferSem );
                userBufferSem.release( SEM_ID_pauseResume );
        }

    //
    // WRITE
    //
    private:

        bool cursorMovementLeftToRight = true;

    public:
    
        size_t write( uint8_t ch ) override {
            if ( offscreen ) return 0;
            uint16_t pos = userCursorY * maxColumns + userCursorX;
            if ( pos > bufferSize ) pos = pos % bufferSize;
            userBuffer[pos] = ch;
            if ( cursorMovementLeftToRight )
                cursorForward();
            else
                cursorBackward();
            return 1;
        }
        
        bool cursorAutoCarriageReturn = false; // move to next line after end of current line
        bool cursorAutoJumpToStart    = false; // move to (0,0) after end of screen

    private:

        void cursorForward() {
            if ( offscreen ) return;
            if ( userCursorX < maxColumns - 1 )
                // enough space
                userCursorX++;
            else if ( cursorAutoCarriageReturn ) {
                // not enough space, but can auto next/prev line
                if ( userCursorY < maxRows - 1 ) {
                    // enough lines
                    userCursorX = 0;
                    userCursorY++;
                } else if ( cursorAutoJumpToStart ) {
                    // not enough lines, but can auto start/end
                    userCursorX = 0;
                    userCursorY = 0;
                } else {
                    offscreen = true;
                }
            } else {
                offscreen = true;
            }
        }

        void cursorBackward() {
            if ( offscreen ) return;
            if ( userCursorX > 0 ) {
                // enough space
                userCursorX--;
            } else if ( cursorAutoCarriageReturn ) {
                // not enough space, but can auto next/prev line
                if ( userCursorY > 0 ) {
                    // enough lines
                    userCursorX = maxColumns - 1;
                    userCursorY--;
                } else if ( cursorAutoJumpToStart ) {
                    // not enough lines, but can auto start/end
                    userCursorX = maxColumns - 1;
                    userCursorY = maxRows - 1;
                } else {
                    offscreen = true;
                }
            } else {
                offscreen = true;
            }
        }

    //
    // USER COMMANDS
    //
    public:

        void clear() override {
            memset( userBuffer, ' ', bufferSize );
            setCursor( 0, 0 );
        }

        inline void home() override {
            // no need to call lcd->home()
            // since LCD origin scrolling not supported
            clear();
        }

        void setCursor( uint8_t col, uint8_t row ) override {
            userCursorX = col; userCursorY = row;
            offscreen = ( userCursorX >= maxColumns || userCursorY >= maxRows );
        }

        inline void backlightOn()  override { lcd->backlightOn();  }
        inline void backlightOff() override { lcd->backlightOff(); }
        inline void displayOn()    override { lcd->displayOn();    }
        inline void displayOff()   override { lcd->displayOff();   }

        inline void moveCursorRight() override { cursorForward(); }
        inline void moveCursorLeft()  override { cursorBackward(); }

        void scrollDisplayLeft() override {
            // ABCDE
            // BCDE_
            char *from = userBuffer+1;
            char *to   = userBuffer;
            for( uint8_t i = 0 ; i < maxRows ; i++ ) {
                memmove( to, from, maxColumns-1 );
                userBuffer[(i+1)*maxColumns-1] = ' ';
                from += maxColumns;
                to += maxColumns;
            }
        }
        void scrollDisplayRight() override {
            // ABCDE
            // _ABCD
            char *from = userBuffer;
            char *to   = userBuffer+1;
            for( uint8_t i = 0 ; i < maxRows ; i++ ) {
                memmove( to, from, maxColumns-1 );
                userBuffer[i*maxColumns] = ' ';
                from += maxColumns;
                to += maxColumns;
            }
        }

        inline void leftToRight() override { cursorMovementLeftToRight = true;  }
        inline void rightToLeft() override { cursorMovementLeftToRight = false; }
        
        inline void autoscrollOn () override {} // not supported
        inline void autoscrollOff() override {}

        inline void createChar( uint8_t charID, const uint8_t charmap[] ) override { lcd->createChar( charID, charmap ); }
        inline void createChar( uint8_t charID, const char *charmap )     override { lcd->createChar( charID, charmap ); }

        inline void command( uint8_t value ) override { lcd->command( value ); }

    //
    // ADDITIONAL FUNCTIONALITIES
    //
    public:

        void scrollDisplayUp( bool clearLastRow = true ) {
            for( uint8_t i = 0 ; i+1 < maxRows ; i++ )
                memcpy( userBuffer + i*maxColumns, userBuffer + (i+1)*maxColumns, maxColumns );
            if ( clearLastRow )
                memset( userBuffer + (maxRows-1)*maxColumns, ' ', maxColumns );
        }

        void scrollDisplayDown( bool clearTopRow = true ) {
            for( int i = maxRows-1 ; i >= 1 ; i-- )
                memcpy( userBuffer + i*maxColumns, userBuffer + (i-1)*maxColumns, maxColumns );
            if ( clearTopRow )
                memset( userBuffer, ' ', maxColumns );
        }

        inline bool checkToBounds( uint8_t &col1, uint8_t &row1, uint8_t &col2, uint8_t &row2 ) {
            if ( col1 > col2 ) { auto t=col1; col1=col2; col2=t; }
            if ( row1 > col2 ) { auto t=row1; row1=col2; col2=t; }
            // CASE (1) - adjust to bounds
            if ( col1 < 0 ) col1 = 0;
            if ( row1 < 0 ) row1 = 0;
            if ( col2 >= maxColumns ) col2 = maxColumns - 1; // if maxColumns=0 crash
            if ( row2 >= maxRows ) row2 = maxRows - 1;       // if maxRows=0 crash
            // CASE (2) exit if out of bounds
            // if ( col1 < 0 || row1 < 0 ) return false;
            // if ( col2 >= maxColumns || row2 >= maxRows ) return false;
            return true;
        }

        void scrollWindowLeft( uint8_t col1, uint8_t row1, uint8_t col2, uint8_t row2 ) {
            if ( !checkToBounds( col1, row1, col2, row2 ) ) return;

            char *from = userBuffer + row1*maxColumns+col1+1;
            char *to   = userBuffer + row1*maxColumns+col1;
            uint8_t count = col2 - col1;
            char *spaceChar = userBuffer + row1*maxColumns+col2;

            for( uint8_t i = row1 ; i <= row2 ; i++ ) {
                memmove( to, from, count );
                from += maxColumns;
                to += maxColumns;
                *spaceChar = ' ';
                spaceChar += maxColumns;
            }
        }

        void scrollWindowRight( uint8_t col1, uint8_t row1, uint8_t col2, uint8_t row2 ) {
            if ( !checkToBounds( col1, row1, col2, row2 ) ) return;

            char *from = userBuffer + row1*maxColumns+col1;
            char *to   = userBuffer + row1*maxColumns+col1+1;
            uint8_t count = col2 - col1;
            char *spaceChar = userBuffer + row1*maxColumns+col1;
            
            for( uint8_t i = row1 ; i <= row2 ; i++ ) {
                memmove( to, from, count );
                from += maxColumns;
                to += maxColumns;
                *spaceChar = ' ';
                spaceChar += maxColumns;
            }
        }

        void scrollWindowUp( uint8_t col1, uint8_t row1, uint8_t col2, uint8_t row2, bool clearLastRow = true ) {
            if ( !checkToBounds( col1, row1, col2, row2 ) ) return;

            char *from = userBuffer + (row1+1)*maxColumns+col1;
            char *to   = userBuffer + row1*maxColumns+col1;
            uint8_t count = col2 - col1 + 1;

            for( uint8_t i = row1 ; i+1 <= row2 ; i++ ) {
                memcpy( to, from, count );
                from += maxColumns;
                to += maxColumns;
            }
            if ( clearLastRow )
                memset( to, ' ', count );
        }

        void scrollWindowDown( uint8_t col1, uint8_t row1, uint8_t col2, uint8_t row2, bool clearLastRow = true ) {
            if ( !checkToBounds( col1, row1, col2, row2 ) ) return;

            char *from = userBuffer + (row2-1)*maxColumns+col1;
            char *to   = userBuffer + row2*maxColumns+col1;
            uint8_t count = col2 - col1 + 1;

            for( uint8_t i = row2 ; i > row1 ; i-- ) {
                memcpy( to, from, count );
                from -= maxColumns;
                to -= maxColumns;
            }
            if ( clearLastRow )
                memset( to, ' ', count );
        }

        void clearToEOL() {
            // clear whereever the cursor is to end-of-line
            if ( offscreen ) return;
            printCharsN( ' ', maxColumns - userCursorX );
        }
        
    //
    // VIRTUAL CURSOR
    //
    private:

        // enum virtualCursorOption {
        //     cOff      = 0b000,
        //     cCursorOn = 0b001,
        //     cBlinking = 0b010
        // };
        uint8_t vCursorX = 0, vCursorY = 0;
        bool isCursorOn = false;
        bool isCursorBlinking = false;
        // virtualCursorOption virtualCursor = cOff; // user setting

        // if currently displayed for blinking
        bool cursorIsDisplayed = false;
        uint32_t cursorLastBlinkUpdate;

        // inline virtualCursorOption vcTurnOn( virtualCursorOption value, virtualCursorOption bitToChange ) {
        //     return static_cast<virtualCursorOption>( static_cast<int>(value) | static_cast<int>(bitToChange) ); }
        // inline virtualCursorOption vcTurnOff( virtualCursorOption value, virtualCursorOption bitToChange ) {
        //     return static_cast<virtualCursorOption>( static_cast<int>(value) & (~static_cast<int>(bitToChange)) ); }
        // inline bool vcIsItOn( virtualCursorOption bitToCheck ) {
        //     return ( (static_cast<int>(virtualCursor) & static_cast<int>(bitToCheck)) == static_cast<int>(bitToCheck) ); }

    public:

        char virtualCursorChar = 0xFF;             // cursor character
        uint16_t cursorBlinkOnDurationInMs  = 400; // blinking duration
        uint16_t cursorBlinkOffDurationInMs = 600; //    ON better shorter than OFF to not obscure text

        void setVirtualCursor( uint8_t col, uint8_t row ) {
            vCursorX = col; vCursorY = row;
        }

        void cursorOn() override {
            isCursorOn = true;
            // virtualCursor = vcTurnOn(virtualCursor,cCursorOn);
            // if blink is on next update on immediately
            // do not reset: if user toggles it will appear to be always on
            // cursorLastBlinkUpdate = millis() - cursorBlinkOnDurationInMs - cursorBlinkOffDurationInMs;
            // cursorIsDisplayed = false;
        }
        inline void cursorOff() override {
            isCursorOn = false;
            // virtualCursor = vcTurnOff(virtualCursor,cCursorOn);
        }
        void cursorBlinkOn() override {
            isCursorBlinking = true;
            // virtualCursor = vcTurnOn(virtualCursor,cBlinking);
            // if on:  next update off immediately
            // if off: not visible right now anyways
            // do not reset: if user toggles it will appear to be always on
            // cursorLastBlinkUpdate = millis() - cursorBlinkOnDurationInMs - cursorBlinkOffDurationInMs;
            // cursorIsDisplayed = true;
        }
        inline void cursorBlinkOff() override {
            isCursorBlinking = false;
            // virtualCursor = vcTurnOff(virtualCursor,cBlinking);
        }

    private:

        void toggleBlinkingCursor() {
            uint16_t duration = cursorIsDisplayed ? cursorBlinkOnDurationInMs : cursorBlinkOffDurationInMs;
            uint32_t now = millis();
            if ( now - cursorLastBlinkUpdate >= duration ) {
                cursorLastBlinkUpdate = now;
                cursorIsDisplayed = !cursorIsDisplayed;
            }
            // if ( !cursorIsDisplayed ) return ch;
        }

        inline void processVirtualCursor_DoubleBuffer() {
            if ( !isCursorOn ) return;
            // if ( !vcIsItOn( cCursorOn ) ) return;

            // already checked
            // if ( bufferMode != doubleBuffer ) return;
            // if ( bufferSize == 0 ) return; // not yet initialized

            if ( isCursorBlinking ) {
                toggleBlinkingCursor();
                if ( !cursorIsDisplayed ) return;
            }

            // if ( vcIsItOn( cBlinking ) ) {
            //     uint32_t now = millis();
            //     if ( cursorIsDisplayed ) {
            //         if ( now - cursorLastBlinkUpdate >= cursorBlinkOnDurationInMs ) {
            //             cursorLastBlinkUpdate = now;
            //             cursorIsDisplayed = false;
            //         }
            //     } else if ( now - cursorLastBlinkUpdate >= cursorBlinkOffDurationInMs ) {
            //         cursorLastBlinkUpdate = now;
            //         cursorIsDisplayed = true;
            //     }
            //     if ( !cursorIsDisplayed ) return;
            // }
            uint16_t pos = vCursorY * maxColumns + vCursorX;
            if ( pos > bufferSize ) pos = pos % bufferSize;
            btsBuffer[pos] = virtualCursorChar;
        }

        inline char processVirtualCursor_SingleBuffer( uint8_t col, uint8_t row, char ch ) {
            if ( !isCursorOn ) return ch;
            if ( col != vCursorX || row != vCursorY ) return ch;
            if ( isCursorBlinking ) {
                toggleBlinkingCursor();
                // uint16_t duration = cursorIsDisplayed ? cursorBlinkOnDurationInMs : cursorBlinkOffDurationInMs;
                // uint32_t now = millis();
                // if ( now - cursorLastBlinkUpdate >= duration ) {
                //     cursorLastBlinkUpdate = now;
                //     cursorIsDisplayed = !cursorIsDisplayed;
                // }
                if ( !cursorIsDisplayed ) return ch;
            }
            return virtualCursorChar;
        }

    //
    // BUFFER TO SCREEN
    //
    private:

        uint8_t btsPtr = 0;
        uint8_t btsCursorX = 0, btsCursorY = 0;

        uint32_t btsLastCompletedUpdate = millis();
        uint16_t btsThrottleInMs = 50;

        enum btsModes {
            mStart, mRunning
        };
        btsModes btsMode = mStart;

    public:

        uint16_t updateDurationInMs = 10;

        void setUpdateThrottleInMs( uint16_t timeInMs ) {
            btsThrottleInMs = timeInMs;
            // set time in the past so next call will update at once
            btsLastCompletedUpdate = millis() - btsThrottleInMs;
        }
        inline uint16_t getUpdateThrottleInMs() {
            return btsThrottleInMs;
        }

        // enum updateResult {
        //     Timeout,        // screen not yet fully updated
        //     Completed,      // screen fully updated
        //     BufferLock,     // user updates lock not released yet
        //     Throttling,     // update not yet performed due to trottling
        //     NotInitialized  // update skipped, begin() not called or invalid, ex. begin(16,0)
        // };

        LCDInterface::updateResult update() override {
            // send updates to LCD
            
            if ( bufferSize == 0 ) return NotInitialized;

            if ( btsMode == mStart ) {
                if ( btsThrottleInMs != 0 ) {
                    if ( millis() - btsLastCompletedUpdate < btsThrottleInMs ) {
                        //Serial.print( "." );
                        return Throttling;
                    }
                }
                if ( bufferMode == doubleBuffer ) {
                    if ( !fetchBtsBuffer() ) {
                        //Serial.print( "L" );
                        return BufferLock;
                    }
                }
                btsMode = mRunning;
                btsPtr = 0; btsCursorX = 0; btsCursorY = 0;
                //Serial.print( "u" );
            
            } // else btsMode == mRunning...

            if ( updateScreenCore( true ) ) {
                // done sending btsBuffer to screen
                btsMode = mStart;
                btsLastCompletedUpdate = millis();
                //Serial.print( "c" );
                return Completed;
            } else {
                // timeout before finishing full screen
                //Serial.print( "t" );
                return Timeout;
            }
        }

        updateResult updateAllNow() {
            // send updates to LCD without timeout
            if ( bufferSize == 0 ) return NotInitialized;
            if ( bufferMode == doubleBuffer ) {
                if ( !fetchBtsBuffer() ) return BufferLock;
            }
            
            // reset pointers
            btsPtr = 0; btsCursorX = 0; btsCursorY = 0;

            // update without timeout
            updateScreenCore( false );

            btsMode = mStart;
            btsLastCompletedUpdate = millis();
            return Completed;
        }

    private:

        bool fetchBtsBuffer() {
            // copy userBuffer to btsBuffer for updating
            // https://www.freertos.org/a00122.html
            // xSemaphoreTake( userBufferSem, portMAX_DELAY );
            if ( userBufferSem.take( SEM_ID_copyUserBuffer ) ) {
            // if ( xSemaphoreTake( userBufferSem, semLockTimeout ) == pdTRUE ) {
                memcpy( btsBuffer, userBuffer, bufferSize );
                userBufferSem.release( SEM_ID_copyUserBuffer );
                // xSemaphoreGive( userBufferSem );
                processVirtualCursor_DoubleBuffer();
                return true;
            } else {
                // semaphore timeout, in case user forgot to call resumeUpdate()
                return false;
            }
        }
        
        bool updateScreenCore( bool checkTimeout ) {

            // return:
            // true  - finished updating
            // false - timeout occured

            uint32_t start;
            if ( checkTimeout ) start = millis();

            // no need to reposition cursor if previous character
            // was updated, LCD screen will move cursor automatically
            // note: setCursor sends additional 1 byte thru i2c
            bool moveLCDCursor = true;
            uint8_t ch;

            while( true ) {
                // keep copy, it changes if singleBuffer/multiCore
                // ch = btsBuffer[btsPtr];
                if ( bufferMode == singleBuffer ) {
                    ch = processVirtualCursor_SingleBuffer( btsCursorX, btsCursorY, btsBuffer[btsPtr] );
                } else {
                    ch = btsBuffer[btsPtr];
                }
                if ( screenData[btsPtr] != ch ) {
                    if ( moveLCDCursor )
                        lcd->setCursor( btsCursorX, btsCursorY );
                    lcd->write( ch );
                    //if ( bufferMode == singleBuffer )
                        screenData[btsPtr] = ch;
                    moveLCDCursor = false;
                } else {
                    moveLCDCursor = true;
                }
                btsPtr++;
                if ( btsPtr >= bufferSize ) {
                    //if ( bufferMode == doubleBuffer ) {
                    //    // update screenData by swapping buffers
                    //    char *tPtr = screenData; screenData = btsBuffer; btsBuffer = tPtr;
                    //}
                    return true;
                } else {
                    btsCursorX++;
                    if ( btsCursorX >= maxColumns ) {
                        btsCursorX = 0;
                        btsCursorY++;
                        // won't reach this point
                        // if ( btsCursorY >= maxRows ) btsCursorY = 0;
                        moveLCDCursor = true;
                    }
                }
                if ( checkTimeout && ( millis() - start >= updateDurationInMs ) ) {
                    return false;
                }
            }
        }

};

//
// TEST
//

    void TEST_scroll( LCDBuffered *lcd ) {
        lcd->printStrAtRow( 0, "0AAA-567890123456789" );
        lcd->printStrAtRow( 1, "0BBB-567890123456789" );
        lcd->printStrAtRow( 2, "0CCC-567890123456789" );
        lcd->printStrAtRow( 3, "0DDD-567890123456789" );
        lcd->displayAll();
        delay(2000);

        lcd->scrollDisplayUp();
        lcd->displayAll();
        delay(2000);

        lcd->scrollDisplayDown();
        lcd->displayAll();
        delay(3000);

        lcd->scrollDisplayLeft();
        lcd->displayAll();
        delay(2000);

        lcd->scrollDisplayRight();
        lcd->displayAll();
        delay(2000);
    }

    void TEST_windowScroll( LCDBuffered *lcd ) {
        //
        // LEFT
        //
        lcd->printStrAtRow( 0, "01234567890123456789" );
        lcd->printStrAtRow( 1, "01234hellohey3456789" );
        lcd->printStrAtRow( 2, "01234hellohey3456789" );
        lcd->printStrAtRow( 3, "01234567890123456789" );
        lcd->displayAll();
        delay(1000);

        for( int i = 0 ; i < 5 ; i++ ) {
            lcd->scrollWindowLeft( 5, 1, 12, 2 );
            // lcd->scrollWindowRight( 2, 0, 20, 3 );

            lcd->displayAll();
            delay(1000);
        }

        //
        // RIGHT
        //
        lcd->printStrAtRow( 0, "01234567890123456789" );
        lcd->printStrAtRow( 1, "01234hellohey3456789" );
        lcd->printStrAtRow( 2, "01234hellohey3456789" );
        lcd->printStrAtRow( 3, "01234567890123456789" );
        lcd->displayAll();
        delay(1000);

        for( int i = 0 ; i < 5 ; i++ ) {
            lcd->scrollWindowRight( 5, 1, 12, 2 );
            // lcd->scrollWindowRight( 2, 0, 20, 3 );
            lcd->displayAll();
            delay(1000);
        }

        //
        // UP
        //
        lcd->printStrAtRow( 0, "01234567890123456789" );
        lcd->printStrAtRow( 1, "01234hellohey3456789" );
        lcd->printStrAtRow( 2, "01234hiheybye3456789" );
        lcd->printStrAtRow( 3, "01234heythere3456789" );
        lcd->displayAll();
        delay(1000);

        for( int i = 0 ; i < 3 ; i++ ) {
            lcd->scrollWindowUp( 5, 1, 12, 3 );
            lcd->displayAll();
            delay(1000);
        }
        lcd->scrollWindowUp( 5, 0, 12, 0 );
        lcd->displayAll();
        delay(1000);

        //
        // DOWN
        //
        lcd->printStrAtRow( 0, "01234567890123456789" );
        lcd->printStrAtRow( 1, "01234hellohey3456789" );
        lcd->printStrAtRow( 2, "01234hiheybye3456789" );
        lcd->printStrAtRow( 3, "01234heythere3456789" );
        lcd->displayAll();
        delay(1000);

        for( int i = 0 ; i < 3 ; i++ ) {
            lcd->scrollWindowDown( 5, 1, 12, 3 );
            lcd->displayAll();
            delay(1000);
        }
        lcd->scrollWindowDown( 5, 0, 12, 0 );
        lcd->displayAll();
        delay(1000);
    }

//
// SPEED TEST
//

    char* LCDBuffered_internalAccess( LCDBuffered *lcd, uint8_t dataToGet ) {
        switch( dataToGet ) {
        case 0: return lcd->userBuffer;
        case 1: return lcd->btsBuffer;
        case 2: return lcd->screenData;
        }
        return nullptr;
    }

    void TEST_bufferedSpeedDisplay( LCDBuffered *lcd, char *buffer ) {
        uint8_t ptr = 0;
        for( int i = 0 ; i < lcd->maxRows ; i++ ) {
            for( int j = 0 ; j < lcd->maxColumns ; j++ )
                Serial.write( buffer[ ptr++ ] );
            Serial.println();
        }
    }

    void TEST_bufferedSpeedCore( LCDBuffered *lcd, bool doubleBuffer, uint16_t throttleInMs, uint32_t count, bool showResults ) {
        if ( doubleBuffer )
            lcd->useDoubleBuffer();
        else
            lcd->useSingleBuffer();
        lcd->setUpdateThrottleInMs( throttleInMs );

        // pauseUpdate();
        lcd->clear();
        // resumeUpdate();
        lcd->updateAllNow();

        lcd->cursorOn();
        lcd->cursorBlinkOn();
        lcd->setVirtualCursor(7,0);

        uint32_t start = millis();
        uint32_t n = 0;
        while( n <= count ) {
            // pauseUpdate();
            lcd->print( n++ );
            lcd->print( '.' );
            // resumeUpdate();
            lcd->update();
        }
        lcd->updateAllNow();
        uint32_t elapsed = millis() - start;

        delay( 1000 );
        if ( doubleBuffer )
            Serial.print( "Double / " );
        else
            Serial.print( "Single / " );
        Serial.print( throttleInMs );
        Serial.print( " / " );
        Serial.print( count );
        Serial.print( " = " );
        Serial.println( elapsed );
        // Serial.printf( "%dms / %d = %d\n", throttleInMs, count, elapsed );
        if ( showResults ) {
            Serial.println( "userBuffer" );
            TEST_bufferedSpeedDisplay( lcd, LCDBuffered_internalAccess( lcd, 0 ) );
            Serial.println( "btsBuffer" );
            TEST_bufferedSpeedDisplay( lcd, LCDBuffered_internalAccess( lcd, 1 ) );
            Serial.println( "screenData" );
            TEST_bufferedSpeedDisplay( lcd, LCDBuffered_internalAccess( lcd, 2 ) );
        }
    }

    void TEST_bufferedSpeed( LCDBuffered *lcd ) {

        // do not simply select the fastest time
        // it depends on available processor time and least amount of data sent to the LCD
        // - for fast data change, increase throttle time, so overwritten changes are not sent
        // - for faster reaction time, ex. pots, decrease or remove throttle
        // - for LCDs with fast updates (ex. hardwired) and fast data changes,
        //   increase throtle time to avoid displaying all changes
        // key is for changes to overlap before sending to screen
        //
        // for this test, higher number tests gets slower
        // eg. 0 to  999, is 3 characters max
        //     0 to 9999, is not only 10x more, but also more characters

        const bool doubleB = true;
        const bool singleB = false;
        Serial.println( "TEST START" );
        
        // ESP32 RESULT

        // time to send full screen data
        // make sure it's different
        lcd->clear();
        lcd->displayAll();
        for( int i = 0 ; i < lcd->maxColumns ; i++ ) {
            lcd->setCursor( 0, i );
            lcd->printCharsN( 'A', lcd->maxColumns );
        }
        // memset( userBuffer, 'A', bufferSize );
        // memset( screenData, ' ', bufferSize );
        lcd->updateDurationInMs = 5000;
        uint32_t start = millis();
        lcd->updateAllNow();
        Serial.print( "Full Screen = " );
        Serial.println( millis() - start );
        // SerialPrintfln( "Full Screen = %lu", millis() - start ); // 22ms
        lcd->updateDurationInMs = 10;

        lcd->cursorAutoCarriageReturn = true;
        lcd->cursorAutoJumpToStart = true;

        TEST_bufferedSpeedCore( lcd, doubleB,   0,   1000, false ); //  2,128
        TEST_bufferedSpeedCore( lcd, singleB,   0,   1000, false ); //  2,118
        TEST_bufferedSpeedCore( lcd, doubleB,   0,   2000, false ); //  6,184
        TEST_bufferedSpeedCore( lcd, singleB,   0,   2000, false ); //  6,163
        TEST_bufferedSpeedCore( lcd, doubleB,   0,   5000, false ); // 18,341
        TEST_bufferedSpeedCore( lcd, singleB,   0,   5000, false ); // 18,288
        Serial.println();

        TEST_bufferedSpeedCore( lcd, doubleB,  10,   1000, false ); //     31
        TEST_bufferedSpeedCore( lcd, singleB,  10,   1000, false ); //     25
        TEST_bufferedSpeedCore( lcd, doubleB,  10,  10000, false ); //    320
        TEST_bufferedSpeedCore( lcd, singleB,  10,  10000, false ); //    149
        TEST_bufferedSpeedCore( lcd, doubleB,  50,  10000, false ); //    
        TEST_bufferedSpeedCore( lcd, singleB,  50,  10000, false ); //    
        TEST_bufferedSpeedCore( lcd, doubleB, 100,  10000, false ); //    
        TEST_bufferedSpeedCore( lcd, singleB, 100,  10000, false ); //    
        TEST_bufferedSpeedCore( lcd, doubleB, 200,  10000, false ); //    
        TEST_bufferedSpeedCore( lcd, singleB, 200,  10000, false ); //    
        //TEST_bufferedSpeedCore( lcd, doubleB,  10, 100000, false ); //  3,160
        //TEST_bufferedSpeedCore( lcd, singleB,  10, 100000, false ); //  1,422
        Serial.println();
        /*
        TEST_bufferedSpeedCore( lcd, doubleB,  10, 500000, false ); // 16,894
        TEST_bufferedSpeedCore( lcd, singleB,  10, 500000, false ); //  7,777
        TEST_bufferedSpeedCore( lcd, doubleB,  50, 500000, false ); //  7,114
        TEST_bufferedSpeedCore( lcd, singleB,  50, 500000, false ); //  3,356
        TEST_bufferedSpeedCore( lcd, doubleB, 100, 500000, false ); //  6,053
        TEST_bufferedSpeedCore( lcd, singleB, 100, 500000, false ); //  2,814
        TEST_bufferedSpeedCore( lcd, doubleB, 200, 500000, false ); //  5,491
        TEST_bufferedSpeedCore( lcd, singleB, 200, 500000, false ); //  2,564
        Serial.println();
        */
        Serial.println( "TEST END" );
    }

}
