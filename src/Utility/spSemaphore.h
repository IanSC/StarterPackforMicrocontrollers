//  Poor Man's Semaphore
//  2022/01 - isc
//
//  simple semaphore implementation
//  write then read if the same
//  needed it for LCDBuffered()
//     but don't want to include FreeRTOS's semaphore
//     overkill for intended use

//  Semaphore sem = Semaphore();
//
//  core0:
//      sem.take( 1 );
//      ... (A)
//      sem.release( 1 );
//
//  core1:
//      sem.take( 2 );
//      ... (B)
//      sem.release( 2 );
//
//  (A) and (B) will never run at the same "time"

#pragma once
#include <Arduino.h>
#include <stdint.h>

namespace StarterPack {

class spSemaphore {

    // 1. write to variable
    // 2. then read
    //    if it's the same
    //        successfully acquired
    //    if not, others have taken it

    volatile uint8_t token = 0;

public:

    spSemaphore() {}
    spSemaphore( uint16_t timeoutInMs ) {
        this->timeoutInMs = timeoutInMs;
    }

    uint16_t timeoutInMs = 100;

    //
    // WITH ID
    //

    bool take( uint8_t id, uint16_t timeoutInMs = 0 ) {
        if ( token == 0 || token == id ) {
            token = id;
            if ( token == id ) {
                // successful
                return true;
            }
        }
        if ( timeoutInMs == 0 )
            timeoutInMs = this->timeoutInMs;
        if ( timeoutInMs == 0 ) {
            while( true ) {
                while ( token != 0 && token != id ) {
                    delay(10);
                    yield();
                }
                token = id;
                if ( token == id )
                    return true;
            }
        }
        
        uint32_t start = millis();
        while( true ) {
            while ( token != 0 && token != id ) {
                if ( millis() - start >= timeoutInMs )
                    return false;
            }
            token = id;
            if ( token == id )
                return true;
            if ( millis() - start >= timeoutInMs )
                return false;
        }
        return false;
    }

    bool release( uint8_t id ) {
        if ( token != id ) {
            // OMG, something is terribly wrong
            return false;
        }
        token = 0;
        return true;
    }

    //
    // SIMPLE LOCK
    //

    bool locked = false;

    bool takeLock( uint16_t timeoutInMs = 0 ) {
        if ( !locked ) {
            locked = true;
            return true;
        }
        if ( timeoutInMs == 0 )
            timeoutInMs = this->timeoutInMs;
        if ( timeoutInMs == 0 ) {
            //while( true ) {
                while( locked ) {
                    delay(10);
                    yield();
                }
                locked = true;
                return true;
            //}
        }
        uint32_t start = millis();
        while( true ) {
            while ( locked ) {
                if ( millis() - start >= timeoutInMs )
                    return false;
            }
            locked = true;
            return true;
        }
        return false;
    }

    bool releaseLock() {
        if ( !locked ) {
            // OMG, something is terribly wrong
            return false;
        }
        locked = true;
        return true;
    }

};

}
