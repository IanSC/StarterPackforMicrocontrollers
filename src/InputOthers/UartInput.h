#pragma once

#include <Stream.h>

#include <InputHelper/InputKeyMapper.h>

#include <UserInterface/UserInterfaceBasic.h>
#include <UserInterface/UserInterfaceDebounced.h>
#include <UserInterface/UserInterfaceRepeated.h>
#include <UserInterface/UserInterfaceMultiClick.h>
#include <UserInterface/UserInterfaceAllKeys.h>

// #define DEBUG_DIAGNOSE
#include <dbgDefines.h>

namespace StarterPack {

class UartInput : public InputKeyMapper<uint8_t,uint8_t> { // }: public UserInterfaceAllKeys {

    // public:

    //     typedef UserInterface::KEY KEY;
    //     static const KEY INACTIVE_KEY = UserInterface::INACTIVE_KEY;

    public:

        UartInput() { }
        void init( Stream * uart ) { this->uart = uart; }
        void init( Stream & uart ) { this->uart = &uart; }

        UartInput( Stream * uart ) { this->uart = uart; }
        UartInput( Stream & uart ) { this->uart = &uart; }

    protected:

        Stream * uart;
        InputKeySource::KEY lastRead = InputKeySource::INACTIVE_KEY;

    //
    // FILTERS
    //
    public:

        InputKeySource::KEY readKeyCore( bool readLastOnly ) {
            // 9600 bps
            // each key is 3 bytes, 8N1 ~ 30 bits
            // 9600 / 30 = 320 keys/sec
            // 1000 ms / 320 keys = 3.125 msec per key 
            auto available = uart->available();
            if ( available < 3 ) {
                // still incompleted
                DBG_IF( available!=0, "available only = " ); DBG_IF_( available!=0, available );
                return lastRead;
            }
            if ( readLastOnly ) {
                // discard all except last 5
                // 1 2 3 4 5
                // M - - M -   incomplete at end
                // - M - - M   ...
                // - - M - -   incomplete at start
                auto discard = uart->available() - 5;
                DBG_IF( discard>0, "discard = " ); DBG_IF_( discard>0, discard );
                for( int i=0 ; i<discard ; i++ )
                    uart->read();
            }
            // look for marker
            while( uart->available() ) {
                auto marker = uart->read();
                if ( marker == B10101010 ) {
                    DBG_( "marker found" );
                    break;
                }
            }
            lastRead = 0;
            InputKeySource::KEY candidate;
            if( uart->available() ) {
                candidate = uart->read();
                DBG( "candidate = " ); DBG_( (uint8_t) candidate );
            }
            if ( uart->available() ) {
                auto check = uart->read();
                if ( candidate != ~check ) {
                    lastRead = candidate;
                    DBG( "check = " ); DBG_( (uint8_t) check );
                }
            }
            return lastRead;
        }

        InputKeySource::KEY read() {
            auto key = readKeyCore( false );
            return InputKeyMapper::actionMapKey(key);
        }

        InputKeySource::KEY readLast() {
            auto key = readKeyCore( true );
            return InputKeyMapper::actionMapKey(key);
        }

};

//
// DEBOUNCED - NOT ACTUALLY DEBOUNCED
//
class UartInputDB : public UartInput,
                    public UserInterfaceBasic { public:

    UartInputDB() { }
    UartInputDB( Stream * uart ) : UartInput( uart ) { }
    UartInputDB( Stream & uart ) : UartInput( uart ) { }

    inline InputKeySource::KEY getNonDebouncedKey() override {
        return UartInput::read();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UartInput::read();
    }
    inline void clearBuffers() override { UartInput::readLast(); }
    inline void clearDebouncedState() override { }
};

//
// DEBOUNCED REPEATED
//
class UartInputRP : public UartInput,
                    public UserInterfaceBasic,
                    public UserInterfaceRepeated { public:

    UartInputRP() { }
    UartInputRP( Stream * uart ) : UartInput( uart ) { }
    UartInputRP( Stream & uart ) : UartInput( uart ) { }

    inline InputKeySource::KEY getNonDebouncedKey() override {
        return UartInput::read();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UartInput::read();
    }
    inline void clearBuffers() override { UartInput::readLast(); }
    inline void clearDebouncedState() override { }
};

//
// DEBOUNCED MULTI-CLICK
//
class UartInputMC : public UartInput,
                    public UserInterfaceBasic,
                    public UserInterfaceMultiClick { public:

    UartInputMC() { }
    UartInputMC( Stream * uart ) : UartInput( uart ) { }
    UartInputMC( Stream & uart ) : UartInput( uart ) { }

    inline InputKeySource::KEY getNonDebouncedKey() override {
        return UartInput::read();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UartInput::read();
    }
    inline void clearBuffers() override { UartInput::readLast(); }
    inline void clearDebouncedState() override { }
};

//
// DEBOUNCED MULTI-CLICK WITH REAL REPEATED LOGIC
//
class UartButton : public UartInput,
                   public UserInterfaceAllKeys { public:

    UartButton() : UartInput() { }
    UartButton( Stream * uart ) : UartInput( uart ) { }
    UartButton( Stream & uart ) : UartInput( uart ) { }

    inline InputKeySource::KEY getNonDebouncedKey() override {
        return UartInput::read();
    }
    inline InputKeySource::KEY getStableKey() override {
        return UartInput::read();
    }
    inline void clearBuffers() override { UartInput::readLast(); }
    inline void clearDebouncedState() override { }
};

}

#include <dbgDefinesOff.h>
