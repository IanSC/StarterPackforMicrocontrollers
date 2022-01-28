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
//      lcd = zz_LCDBuffered( rs, en, d4, d5, d6, d7 );
//
//  For I2C
//
//      lcd = zz_LCDBuffered( 0x27 );
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
//      setUpdateThrottleInMs( delayInMs )    time between actual updates
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
#include <LCDInterface.h>
#include <pmSemaphore.h>

namespace StarterPack {

typedef uint8_t ERROR_NO;

class LCDBuffered : public LCDInterface {

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

    public:

        bool isBuffered() { return true; }
        void displayAll() { updateAllNow(); }

        LCDBuffered() {}

        LCDBuffered( LCDInterface &lcd ) {
            this->lcd = &lcd;
            // assume user already called begin() of lcd
            initBuffers();
        }

        inline void setTimeoutInMs( uint16_t timeOut ) {
            lcd->setTimeoutInMs( timeOut );
        }

        inline void setFrequency( uint32_t frequency ) {
            lcd->setFrequency( frequency );
        }

    //
    // BEGIN
    //
    public:

        inline void begin( uint8_t maxColumns, uint8_t maxRows, LCDInterface::charDotSize dotSize = LCDInterface::charDotSize::size5x8 ) {
            lcd->begin( maxColumns, maxRows, dotSize );
            initBuffers();
        }

    //
    // BUFFERS
    //
    private:
    
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

            //Serial.println( lcd->maxColumns );
            //Serial.println( lcd->maxRows );

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

        bool verify() {
            return lcd->verify();
        }
        ERROR_NO verifyWithError() {
            return lcd->verifyWithError();
        }
        
        bool recoverIfHasError() {
            bool r = lcd->recoverIfHasError();
            if ( r ) reset();
            return r;
        }
        inline void setRecoveryThrottleInMs( uint16_t delay ) {
            lcd->setRecoveryThrottleInMs( delay );
        }
        void reset() {
            // if LCD is reset, will revert to blanks
            memset( screenData, ' ', bufferSize );
            updateAllNow();
        }

    //
    // TESTS
    //
    public:

        void runSpeedTest() {

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
            memset( userBuffer, 'A', bufferSize );
            memset( screenData, ' ', bufferSize );
            updateDurationInMs = 5000;
            uint32_t start = millis();
            updateAllNow();
//            Serial.printf( "Full Screen = %lu\n", millis() - start ); // 22ms
            Serial.print( "Full Screen = " );
            Serial.println( millis() - start ); // 22ms
            Serial.println();
            updateDurationInMs = 10;

            cursorAutoCarriageReturn = true;
            cursorAutoJumpToStart = true;

            runSpeedTestSolo( doubleB,   0,   1000, false ); //  2,128
            runSpeedTestSolo( singleB,   0,   1000, false ); //  2,118
            runSpeedTestSolo( doubleB,   0,   2000, false ); //  6,184
            runSpeedTestSolo( singleB,   0,   2000, false ); //  6,163
            runSpeedTestSolo( doubleB,   0,   5000, false ); // 18,341
            runSpeedTestSolo( singleB,   0,   5000, false ); // 18,288
            Serial.println();

            runSpeedTestSolo( doubleB,  10,   1000, false ); //     31
            runSpeedTestSolo( singleB,  10,   1000, false ); //     25
            runSpeedTestSolo( doubleB,  10,  10000, false ); //    320
            runSpeedTestSolo( singleB,  10,  10000, false ); //    149
            runSpeedTestSolo( doubleB,  50,  10000, false ); //    
            runSpeedTestSolo( singleB,  50,  10000, false ); //    
            runSpeedTestSolo( doubleB, 100,  10000, false ); //    
            runSpeedTestSolo( singleB, 100,  10000, false ); //    
            runSpeedTestSolo( doubleB, 200,  10000, false ); //    
            runSpeedTestSolo( singleB, 200,  10000, false ); //    
            //runSpeedTestSolo( doubleB,  10, 100000, false ); //  3,160
            //runSpeedTestSolo( singleB,  10, 100000, false ); //  1,422
            Serial.println();
            /*
            runSpeedTestSolo( doubleB,  10, 500000, false ); // 16,894
            runSpeedTestSolo( singleB,  10, 500000, false ); //  7,777
            runSpeedTestSolo( doubleB,  50, 500000, false ); //  7,114
            runSpeedTestSolo( singleB,  50, 500000, false ); //  3,356
            runSpeedTestSolo( doubleB, 100, 500000, false ); //  6,053
            runSpeedTestSolo( singleB, 100, 500000, false ); //  2,814
            runSpeedTestSolo( doubleB, 200, 500000, false ); //  5,491
            runSpeedTestSolo( singleB, 200, 500000, false ); //  2,564
            Serial.println();
            */
            Serial.println( "TEST END" );
        }

        void runSpeedTestSolo( bool doubleBuffer, uint16_t throttleInMs, uint32_t count, bool showResults ) {
            if ( doubleBuffer )
                useDoubleBuffer();
            else
                useSingleBuffer();
            setUpdateThrottleInMs( throttleInMs );

            // pauseUpdate();
            clear();
            // resumeUpdate();
            updateAllNow();

            cursorOn();
            cursorBlinkOn();
            setVirtualCursor(7,0);

            uint32_t start = millis();
            uint32_t n = 0;
            while( n <= count ) {
                // pauseUpdate();
                print( n++ );
                print( '.' );
                // resumeUpdate();
                update();
            }
            updateAllNow();
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
//            Serial.printf( "%dms / %d = %d\n", throttleInMs, count, elapsed );
            if ( showResults ) {
                Serial.println( "userBuffer" );
                debug( userBuffer );
                Serial.println( "btsBuffer" );
                debug( btsBuffer );
                Serial.println( "screenData" );
                debug( screenData );
            }
        }

        void debug( char *buffer ) {
            uint8_t ptr = 0;
            for( int i = 0 ; i < maxRows ; i++ ) {
                for( int j = 0 ; j < maxColumns ; j++ )
                    Serial.write( buffer[ ptr++ ] );
                Serial.println();
            }
        }

    //
    // USER'S UPDATES
    //
    private:
        
        uint8_t userCursorX = 0, userCursorY = 0;
        bool offscreen = true;
        // SemaphoreHandle_t userBufferSem = xSemaphoreCreateMutex();
        // TickType_t semLockTimeout = portTICK_PERIOD_MS * 50;
        pmSemaphore userBufferSem = pmSemaphore( 50 );

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

        void setSemaphoreLockTimeoutInMs( uint8_t waitingTime ) {
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
    public:
    
        size_t write( uint8_t ch ) {
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

        bool cursorMovementLeftToRight = true;

        void cursorForward() {
            if ( offscreen ) return;
            userCursorX++;
            if ( userCursorX >= maxColumns ) {
                if ( cursorAutoCarriageReturn ) {
                    userCursorX = 0;
                    userCursorY++;
                    if ( userCursorY >= maxRows ) {
                        if ( cursorAutoJumpToStart ) {
                            userCursorY = 0;
                        } else {
                            offscreen = true;
                        }
                    }
                } else {
                    offscreen = true;
                }
            }
        }

        void cursorBackward() {
            if ( offscreen ) return;
            if ( userCursorX > 0 ) {
                userCursorX--;
            } else {
                if ( cursorAutoCarriageReturn ) {
                    userCursorX = maxColumns-1;
                    userCursorY++;
                    if ( userCursorY >= maxRows ) {
                        if ( cursorAutoJumpToStart ) {
                            userCursorY = 0;
                        } else {
                            offscreen = true;
                        }
                    }
                } else {
                    offscreen = true;
                }
            }
        }

    //
    // USER COMMANDS
    //
    public:

        void clear() {
            memset( userBuffer, ' ', bufferSize );
            setCursor( 0, 0 );                
        }

        inline void home() {
            // no need to call lcd->home()
            // since LCD origin scrolling not supported
            clear();
        }

        void setCursor( uint8_t col, uint8_t row ) {
            userCursorX = col; userCursorY = row;
            offscreen = ( userCursorX >= maxColumns || userCursorY >= maxRows );
        }

        inline void backlightOn()  { lcd->backlightOn();  }
        inline void backlightOff() { lcd->backlightOff(); }
        inline void displayOn()    { lcd->displayOn();    }
        inline void displayOff()   { lcd->displayOff();   }

        void scrollDisplayLeft() {
            // ABCDE
            // BCDE_
            char *from = userBuffer+1;
            char *to   = userBuffer;
            for( int i = 0 ; i < maxRows ; i++ ) {
                memmove( to, from, maxColumns-1 );
                userBuffer[(i+1)*maxColumns-1] = ' ';
                from += maxColumns;
                to += maxColumns;
            }
        }
        void scrollDisplayRight() {
            // ABCDE
            // _ABCD
            char *from = userBuffer;
            char *to   = userBuffer+1;                
            for( int i = 0 ; i < maxRows ; i++ ) {
                memmove( to, from, maxColumns-1 );
                userBuffer[i*maxColumns] = ' ';
                from += maxColumns;
                to += maxColumns;                    
            }
        }

        inline void leftToRight() { cursorMovementLeftToRight = true;  }
        inline void rightToLeft() { cursorMovementLeftToRight = false; }
        
        inline void autoscrollOn () {} // not supported
        inline void autoscrollOff() {}

        inline void createChar( uint8_t location, uint8_t     charmap[] ) { lcd->createChar(location,charmap); }
        inline void createChar( uint8_t location, const char *charmap   ) { lcd->createChar(location,charmap); }

        inline void command( uint8_t value ) { lcd->command( value ); }

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
        uint16_t cursorBlinkOnDurationInMs  = 600; // blinking duration
        uint16_t cursorBlinkOffDurationInMs = 400;

        void setVirtualCursor( uint8_t col, uint8_t row ) {
            vCursorX = col; vCursorY = row;
        }

        void cursorOn() {
            isCursorOn = true;
            // virtualCursor = vcTurnOn(virtualCursor,cCursorOn);
            // if blink is on next update on immediately
            cursorLastBlinkUpdate = millis() - cursorBlinkOnDurationInMs - cursorBlinkOffDurationInMs;
            cursorIsDisplayed = false;
        }
        inline void cursorOff() {
            isCursorOn = false;
            // virtualCursor = vcTurnOff(virtualCursor,cCursorOn);
        }
        void cursorBlinkOn()  {
            isCursorBlinking = true;
            // virtualCursor = vcTurnOn(virtualCursor,cBlinking);
            // if on:  next update off immediately
            // if off: not visible right now anyways
            cursorLastBlinkUpdate = millis() - cursorBlinkOnDurationInMs - cursorBlinkOffDurationInMs;
            cursorIsDisplayed = true;
        }
        inline void cursorBlinkOff() {
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
        uint16_t btsThrottleInMs = 10;

        enum btsModes {
            mStart, mRunning
        };
        btsModes btsMode = mStart;

    public:

        uint16_t updateDurationInMs = 10;

        void setUpdateThrottleInMs( uint16_t delayInMs ) {
            btsThrottleInMs = delayInMs;
            // set time in the past so next call will update at once
            btsLastCompletedUpdate = millis() - btsThrottleInMs;
        }

        enum updateResult {
            Timeout,        // screen not yet fully updated
            Completed,      // screen fully updated
            BufferLock,     // user updates lock not released yet
            Throttling,     // update not yet performed due to trottling
            NotInitialized  // update skipped, begin() not called or invalid, ex. begin(16,0)
        };

        updateResult update() {
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
            }
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

}
