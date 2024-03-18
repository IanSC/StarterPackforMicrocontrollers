//  2022/01 - isc
//
//  I2C Helper
//  ----------
//  - checks all possible error conditions for i2c communication
//  - able to recover from errors
//    eg. disconnect any/all VCC, GND, SDA, SCL
//        after reconnecting, communication will continue without rebooting
//  - uses <Wire.h> of Arduino or ESP32
//    note: Arduino's Wire.h now includes timeout
//          but some error conditions are possibly not detected
//  - use wrapper style to avoid rewriting/updating core wire library
//
//  Creation
//
//      1. Default Global Wire
//      
//         Wire.begin();
//         TwoWireHelper i2cHelper = TwoWireHelper( i2cAddress );
//         i2cHelper.setTimeoutInMs( 100 ); // default is 50 ms
//         i2cHelper.setFrequency( 100000 );
//
//      2. Specify Different SDA/SCL Pins
//
//         Wire1.begin( SDApin, SCLpin );
//         TwoWireHelper i2cHelper = TwoWireHelper( Wire1, i2cAddress );
//         i2cHelper.setTimeoutInMs( 100 );
//         i2cHelper.setFrequency( 100000 );
//
//  Functions
//
//      void     setTimeoutInMs( value )    sets i2c timeout, default is 50 ms
//      void     setFrequency( value )      sets i2c speed, default is based on Wire.h
//      bool     verify()                   verify i2c connection, returns true/false
//      ERROR_NO verifyWithError()          verify i2c connection, returns error number
//      char *   errorMessage( ERROR_NO )   return error message string
//      bool     recoverIfHasError()        resets i2c if error has been found, return true if recovery attempt was made
//      uint16_t recoveryThrottleInMs       time between recovery attempts, default 2000 ms
//      ERROR_NO lastError                  last error detected, will not reset even if next calls succeeds
//                                          resets only upon calling recoverIfHasError(), clearLastError() or directly cleared
//      void     clearLastError()           clear lastError
//
//  Read 1 Byte
//
//      ERROR_NO readOneByte( dataAddr, & result )
//      ex. byte result;
//          ERROR_NO err = i2cHelper.readOneByte( dataAddress, result );
//          if ( err != ERR_I2C_OK ) ... error found
//
//      uint8_t readOneByte( dataAddr )
//      ex. byte result = i2cHelper.readOneByte( dataAddress );
//          if ( i2cHelper.lastError != ERR_I2C_OK ) ... error found
//
//  Read 2 Bytes from Different Address
//
//      ERROR_NO readTwoBytes_DiffAddr( lowDataAddr, highDataAddr, & result )
//      ex. uint16_t result;
//          ERROR_NO err = i2cHelper.readTwoBytes_DiffAddr( addr1, addr2, result );
//          if ( err != ERR_I2C_OK ) ... error found
//
//      uint16_t readTwoBytes_DiffAddr( lowDataAddr, highDataAddr )
//      ex. uint16_t result = i2cHelper.readTwoBytes_DiffAddr( addr1, addr2 );
//          if ( i2cHelper.lastError != ERR_I2C_OK ) ... error found
//
//  Read 2 Bytes from Same Address (ex. AS5600 magnetic encoder)
//
//      ERROR_NO readTwoBytes_SameAddr_HiLo( dataAddr, & result )
//      ERROR_NO readTwoBytes_SameAddr_LoHi( dataAddr, & result )
//      ex. uint16_t result;
//          ERROR_NO err = i2cHelper.readTwoBytes_SameAddr_HiLo( addr, result );
//          if ( err != ERR_I2C_OK ) ... error found
//
//      uint16_t readTwoBytes_SameAddr_HiLo( dataAddr )        
//      uint16_t readTwoBytes_SameAddr_LoHi( dataAddr )
//      ex. uint16_t result = i2cHelper.readTwoBytes_SameAddr_LoHi( addr );
//          if ( i2cHelper.lastError != ERR_I2C_OK ) ... error found
//
//  Ex:
//      TwoWireHelper i2cHelper = TwoWireHelper( 0x36 )
//      i2cHelper.recoveryThrottleInMs = 3000; // recover after every 3 seconds only
//  
//      while( true ) {
//          byte result;
//          ERROR_NO err = i2cHelper.readOneByte( 0x0B, result );
//          if ( err != ERR_I2C_OK )
//              Serial.println( i2cHelper.errorMessage( err ) );
//          else
//              Serial.println( result );
//          if ( i2cHelper.recoverIfHasError() ) {
//              // ... reset device if necessary
//          }
//      }
//
//  Ex:
//      TwoWireHelper i2cHelper = TwoWireHelper( 0x36 )
//      byte result1 = i2cHelper.readOneByte( 0x0B );
//      // ... do something assume no errors
//      int result2 = i2cHelper.readTwoBytes_SameAddr_HiLo( 0x0C );
//      // ... do something assume no errors
//      // check in case of failure once in a while
//      if ( i2cHelper.lastError != ERR_I2C_OK ) {
//          Serial.println( i2cHelper.errorMessage( i2cHelper._lastError ) );
//          // ... reset device if necessary
//      }
//
//  To Explore:
//      https://github.com/stickbreaker/arduino-esp32/tree/master/libraries/Wire
//      https://github.com/esp8266/Arduino/issues/1025
//      http://www.forward.com.au/pfod/ArduinoProgramming/I2C_ClearBus/index.html

#pragma once
#include <Arduino.h>
#include <Wire.h>

extern volatile uint32_t twi_timeout_us; // in Wire.h/TWI.h

namespace StarterPack {

typedef uint8_t ERROR_NO;

class i2cHelper {

    public:

        //
        // ERROR MESSAGES
        //
    
        // https://github.com/arduino/ArduinoCore-avr/blob/master/libraries/Wire/src/utility/twi.c
        // https://github.com/arduino/ArduinoCore-avr/blob/master/libraries/Wire/src/utility/twi.h
        // https://github.com/arduino/ArduinoCore-avr/blob/master/libraries/Wire/src/Wire.cpp
        // https://github.com/arduino/ArduinoCore-avr/blob/master/libraries/Wire/src/Wire.h    
    
        // Wire                  Calls TWI Functions
        // -------------------   -------------------
        // beginTransmission()
        // endTransmission()     twi_writeTo()
        //                       Output   0 .. success
        //                                1 .. length too long for buffer
        //                                2 .. address send, NACK received
        //                                3 .. data send, NACK received
        //                                4 .. other twi error (lost bus arbitration, bus error, ..)
        //                                5 .. timeout
        //                       if ESP32, espressif framework:
        //                          case ESP_OK:          return 0;
        //                          case ESP_FAIL:        return 2;
        //                          case ESP_ERR_TIMEOUT: return 5;
        //                          default:              return 4;
        // write()               twi_transmit(),                       *** ERRORS WERE IGNORED *** dumbf...
        //                       Output   1 length too long for buffer *** NOT HANDLED
        //                                2 not slave transmitter      *** NOT HANDLED
        //                                0 ok
        // requestFrom()         beginTransmission()
        //                       write()
        //                       endTransmission()
        //                       twi_readFrom() - timeout

        // void dummy() {
        //     _wire->beginTransmission( 0 );
        //     _wire->endTransmission();
        //     _wire->write( 0 );
        //     _wire->getWriteError();
        //     _wire->clearWriteError();
        //     _wire->requestFrom(0,1);
        // }

        static const ERROR_NO ERR_I2C_OK        = 0;
        static const ERROR_NO ERR_I2C_BUFFER    = 1;  // 1 .. length too long for buffer
        static const ERROR_NO ERR_I2C_ADDR_NACK = 2;  // 2 .. address send, NACK received
        static const ERROR_NO ERR_I2C_DATA_NACK = 3;  // 3 .. data send, NACK received
        static const ERROR_NO ERR_I2C_TWI       = 4;  // 4 .. other twi error (lost bus arbitration, bus error, ..)
        static const ERROR_NO ERR_I2C_TIMEOUT   = 5;  // 5 .. timeout

        // additional errors possibly not handled in Wire.h
        static const ERROR_NO ERR_I2C_WRITE    = 101;
        static const ERROR_NO ERR_I2C_ENDTRANS = 102;
        static const ERROR_NO ERR_I2C_REQUEST  = 103;
        static const ERROR_NO ERR_I2C_TIMEOUT2 = 104;

        static const char * errorMessage( ERROR_NO errorNo ) {
            switch ( errorNo ) {
            case ERR_I2C_OK:        return "no error";                          // errors from Arduino Wire.h
            case ERR_I2C_BUFFER:    return "length too long for buffer";
            case ERR_I2C_ADDR_NACK: return "address send, NACK received";
            case ERR_I2C_DATA_NACK: return "data send, NACK received";
            case ERR_I2C_TWI:       return "other twi error";
            case ERR_I2C_TIMEOUT:   return "timeout";
            case ERR_I2C_WRITE:     return "invalid return value from write()"; // additional errors
            case ERR_I2C_ENDTRANS:  return "invalid return value from endTransmission()";
            case ERR_I2C_REQUEST:   return "invalid requestFrom() length";
            case ERR_I2C_TIMEOUT2:  return "uncaught timeout";
            default:                return "unknown error";
            }
        }

    private:

        TwoWire * _wire;
        uint8_t   _defaultI2cAddress;

    public:

        #if defined(ARDUINO_ARCH_ESP32)
            // https://github.com/espressif/arduino-esp32/issues/6616
            // Confusing overload of Wire::begin
            // master/slave mode depends on variable type
            inline static bool beginMaster( TwoWire & wire, uint8_t sda, uint8_t scl ) {
                return wire.begin( (int) sda, (int) scl );
            }
            inline static bool beginMaster( TwoWire & wire, uint8_t sda, uint8_t scl, uint32_t freq=100000, uint32_t timeout=50 ) {
                wire.setTimeOut( timeout );
                return wire.begin( (int) sda, (int) scl, (uint32_t) freq );
            }
            inline static bool beginSlave( TwoWire & wire, uint8_t sda, uint8_t scl ) {
                // slave has no clock
                return wire.begin( sda, scl );
            }
        #elif defined(ARDUINO_ARCH_SAMD)
            // SEEDUINO XIAO SAMD21
            // https://community.platformio.org/t/using-preprocessor-directives-defined-in-platformio-ini/24169/2
            inline static bool beginMaster( TwoWire & wire ) {
                wire.begin();
                return true;
            }
            inline static bool beginSlave( TwoWire & wire, uint8_t __defaultI2cAddress ) {
                wire.begin(__defaultI2cAddress);
                return true;
            }
        // #elif defined(ARDUINO_ARCH_AVR)
        #else

        #endif

        // // https://github.com/espressif/arduino-esp32/issues/6616
        // // Confusing overload of Wire::begin
        // // master/slave mode depends on variable type
        // inline static bool beginMaster( TwoWire & wire, uint8_t sda, uint8_t scl ) {
        //     #if defined(ARDUINO_ARCH_SAMD)
        //         #error UNSUPPORTED: i2cHelper.h
        //         // wire.begin();
        //         // return true;
        //     #else
        //         return wire.begin( (int) sda, (int) scl );
        //     #endif
        // }
        // inline static bool beginMaster( TwoWire & wire, uint8_t sda, uint8_t scl, uint32_t freq ) {
        //     #if defined(ARDUINO_ARCH_SAMD)
        //         #error UNSUPPORTED: i2cHelper.h
        //         // wire.begin();
        //         // return true;
        //     #else
        //         return wire.begin( (int) sda, (int) scl, (uint32_t) freq );
        //     #endif
        // }
        // inline static bool beginSlave( TwoWire & wire, uint8_t sda, uint8_t scl ) {
        //     #if defined(ARDUINO_ARCH_SAMD)
        //         #error UNSUPPORTED: i2cHelper.h
        //     #else
        //         // slave has no clock
        //         return wire.begin( sda, scl );
        //     #endif
        // }
        // inline static bool beginSlave( TwoWire & wire, uint8_t __defaultI2cAddress ) {
        //     #if defined(ARDUINO_ARCH_SAMD)
        //         wire.begin(__defaultI2cAddress);
        //         return true;
        //     #else
        //         #error UNSUPPORTED: i2cHelper.h
        //     #endif
        // }

    public:

        i2cHelper( uint8_t defaultI2cAddress ) {
            // default to global "Wire"
            _wire = &Wire;
            _defaultI2cAddress = defaultI2cAddress;
            // up to user to set timeout on wire or through setTimeoutInMs() ???            
            setTimeoutInMs( 50 ); // let's just set it
        }

        i2cHelper( TwoWire & wire, uint8_t defaultI2cAddress ) {
            _wire = &wire;
            _defaultI2cAddress = defaultI2cAddress;
            // up to user to set timeout on wire or through setTimeoutInMs() ???
            setTimeoutInMs( 50 ); // let's just set it
        }

        inline void setTimeoutInMs( uint16_t timeOut ) {
            #if defined(ESP32)
                _wire->setTimeOut( timeOut );
            #elif defined(ARDUINO_ARCH_AVR)
                _wire->setWireTimeout( timeOut * 1000 ); // Arduino in microsecs
            #endif
            // Stream.h in microsecs
            _wire->setTimeout( timeOut );
        }

        inline uint16_t getTimeoutInMs() {
            #if defined(ESP32)
                return _wire->getTimeOut();
            #elif defined(ARDUINO_ARCH_AVR)
                return twi_timeout_us / 1000;
            #else
                // get from Stream.h ???
                return _wire->getTimeout(); // assume milliseconds
            #endif
        }

        inline void setFrequency( uint32_t frequency ) {
            _wire->setClock( frequency );
        }
        
    //
    // LAST ERROR
    //
    public:

        ERROR_NO lastError = ERR_I2C_OK;

        inline void clearLastError() { lastError = ERR_I2C_OK; }
        
    private:
    
        inline void RecordError( ERROR_NO newError ) {
            lastError = newError;
        }

        #if defined(ARDUINO_ARCH_AVR)
        
            inline bool CheckAndRecordError() {
                if ( _wire->getWireTimeoutFlag() ) {
                    lastError = ERR_I2C_TIMEOUT;
                    return false;
                } else
                    return true;
            }

        #else

            inline bool CheckAndRecordError() {
                return true;
            }
            
        #endif

    //
    // VERIFY
    //
    public:

        bool verify() {
            return ( verifyWithError( _defaultI2cAddress ) == ERR_I2C_OK );
        }
        
        bool verify( uint8_t _i2cAddress ) {
            return ( verifyWithError( _i2cAddress ) == ERR_I2C_OK );
        }

        inline ERROR_NO verifyWithError() {
            return verifyWithError( _defaultI2cAddress );
        }

        ERROR_NO verifyWithError( uint8_t _i2cAddress ) {
            _wire->beginTransmission( _i2cAddress );
            if ( !endTransmission() ) return lastError;
            return ERR_I2C_OK;
        }

    //
    // RECOVERY
    //
    private:

        uint32_t lastRecovery = millis();

    public:

        uint16_t recoveryThrottleInMs = 2000;

        #if defined(ARDUINO_ARCH_AVR)

            bool recoverIfHasError( uint8_t _i2cAddress ) {
                if ( lastError == ERR_I2C_OK )
                    return false;

                // try to recover every recoveryThrottleInMs only
                uint32_t now = millis();
                if ( now - lastRecovery > recoveryThrottleInMs ) {
                    lastRecovery = now;
                    
                    // Serial.print( "RECO: " );
                    // Serial.println( errorMessage( lastError ) );

                    // compute existing frequency since it was not stored
                    // https://github.com/arduino/ArduinoCore-avr/blob/master/libraries/Wire/src/utility/twi.c
                    // twi_setFrequency()
                    // Frequency = CPU Clock Frequency / (16 + (2 * TWBR))
                    // TWBR = ((F_CPU / frequency) - 16) / 2;
                    uint32_t freq = F_CPU / (16 + (2 * TWBR));

                    //Serial.print( "freq = " );
                    //Serial.println( freq );
                    
                    // need _wire->end() to recover
                    //    ERROR: 2 NACK    = no
                    //    ERROR: 4 TIMEOUT = yes
                    _wire->end();
                    _wire->begin( _i2cAddress );
                    _wire->setClock( freq );
                    
                    lastError = ERR_I2C_OK;
                    _wire->clearWireTimeoutFlag();
                    _wire->clearWriteError();
                    return true;
                }
                return false;
            }

        #else

            bool recoverIfHasError( uint8_t _i2cAddress ) {
                if ( lastError == ERR_I2C_OK )
                    return false;
                
                // try to recover every recoveryThrottleInMs only
                uint32_t now = millis();
                if ( now - lastRecovery > recoveryThrottleInMs ) {
                    lastRecovery = now;

                    //Serial.println( "RECO: " );
                    //if ( lastError != ERR_I2C_OK )
                    //   Serial.printf( "   Last Error = %s\n", errorMessage( lastError ) );

                    _wire->begin();
                    _wire->clearWriteError();
                    lastError = ERR_I2C_OK;
                    return true;
                }
                return false;
            }

        #endif

        inline bool recoverIfHasError() {
            return recoverIfHasError( _defaultI2cAddress );
        }

    //
    // WRAPPERS
    //
    
        inline bool endTransmission() {
            auto err = _wire->endTransmission();
            if ( err != ERR_I2C_OK ) { RecordError( err ); return false; }
            if ( !CheckAndRecordError() ) return false;
            return true;
        }

        inline bool write( uint8_t data ) {
            size_t bytesWritten = _wire->write( data );
            auto err = _wire->getWriteError();
            if ( err != 0 ) { RecordError( ERR_I2C_WRITE ); return false; }
            if ( !CheckAndRecordError() ) return false;
            // NEEDED FOR ARDUINO
            if ( bytesWritten != 1 ) { RecordError( ERR_I2C_WRITE ); return false; }
            return true;
        }

        inline bool available( uint8_t length ) {
            if ( _wire->available() < length ) {
                uint32_t start = millis();
                while ( _wire->available() < length ) {
                    if ( millis() - start >= getTimeoutInMs() ) { RecordError( ERR_I2C_TIMEOUT2 ); return false; }
                    if ( !CheckAndRecordError() ) return false;
                }
            }
            return CheckAndRecordError();
        }

        // #if defined(ESP32)

        //     inline bool available( uint8_t length ) {
        //         if ( _wire->available() < length ) {
        //             uint32_t start = millis();
        //             while ( _wire->available() < length ) {
        //                 if ( millis() - start >= _wire->getTimeOut() ) { RecordError( ERR_I2C_TIMEOUT2 ); return false; }
        //                 if ( !CheckAndRecordError() ) return false;
        //             }
        //         }
        //         return CheckAndRecordError();
        //     }

        // #elif defined(ARDUINO_ARCH_AVR)

        //     // Ardu9no Nano
        //     inline bool available( uint8_t length ) {
        //         // TIMEOUT IN MICROS
        //         // extern volatile uint32_t twi_timeout_us; // in Wire.h/TWI.h
        //         if ( _wire->available() < length ) {
        //             uint32_t start = micros();
        //             while ( _wire->available() < length ) {

        //                 _wire->getTimeout()
        //                 if ( micros() - start >= twi_timeout_us ) { RecordError( ERR_I2C_TIMEOUT2 ); return false; }
        //                 if ( !CheckAndRecordError() ) return false;
        //             }
        //         }
        //         return CheckAndRecordError();
        //     }

        // #else
        
        //     // Seeeduino Xiao
        //     inline bool available( uint8_t length ) {
        //         if ( _wire->available() < length ) {
        //             uint32_t start = micros();
        //             while ( _wire->available() < length ) {

        //                 _wire->getTimeout()
        //                 if ( micros() - start >= twi_timeout_us ) { RecordError( ERR_I2C_TIMEOUT2 ); return false; }
        //                 if ( !CheckAndRecordError() ) return false;
        //             }
        //         }
        //         return CheckAndRecordError();
        //     }

        // #endif

        inline bool requestFrom( uint8_t _i2cAddress, uint8_t length ) {
            size_t bytesArrived = _wire->requestFrom( _i2cAddress, length );
            if ( !CheckAndRecordError() ) return false;
            // NEEDED FOR ARDUINO / ESP32
            if ( bytesArrived != length ) { RecordError( ERR_I2C_REQUEST ); return false; }
            return true;
        }
        
    //
    // READ CORE
    //
    private:

        ERROR_NO readBytesCore( uint8_t _i2cAddress, uint8_t dataAddr, uint8_t length ) {    
            _wire->beginTransmission( _i2cAddress );
            if ( !write( dataAddr ) ) return lastError;
            if ( !endTransmission() ) return lastError;
            if ( !requestFrom( _i2cAddress, length ) ) return lastError;
            if ( !available( length ) ) return lastError;
            return ERR_I2C_OK;
        }

    //
    // READ ONE BYTE
    //
    public:

        ERROR_NO readOneByte_i2c( uint8_t _i2cAddress, uint8_t dataAddr, uint8_t & result ) {
            ERROR_NO r = readBytesCore( _i2cAddress, dataAddr, 1 );
            if ( r != ERR_I2C_OK ) { return r; }
            result = _wire->read();
            return ERR_I2C_OK;
        }

        uint8_t readOneByte_i2c( uint8_t _i2cAddress, uint8_t dataAddr ) {
            uint8_t result;
            ERROR_NO r = readOneByte_i2c( _i2cAddress, dataAddr, result );
            if ( r != ERR_I2C_OK ) { return 0; }
            return result;
        }

        inline ERROR_NO readOneByte( uint8_t dataAddr, uint8_t & result ) {
            return readOneByte_i2c( _defaultI2cAddress, dataAddr, result );
        }

        inline uint8_t readOneByte( uint8_t dataAddr ) {
            return readOneByte_i2c( _defaultI2cAddress, dataAddr );
        }

    //
    // READ 2 BYTES SEPARATELY
    //
    public:
        
        ERROR_NO readTwoBytes_DiffAddr_i2c( uint8_t _i2cAddress, uint8_t lowDataAddr, uint8_t highDataAddr, uint16_t & result ) {
            // read 2 bytes from different addresses, assemble to result
            //    lowDataAddr  is LSB (low byte)
            //    highDataAddr is MSB (high byte)
            uint8_t lowByte, highByte;
            ERROR_NO r = readOneByte_i2c( _i2cAddress, lowDataAddr, lowByte );
            if ( r != ERR_I2C_OK ) { return r; }
            r = readOneByte_i2c( _i2cAddress, highDataAddr, highByte );
            if ( r != ERR_I2C_OK ) { return r; }
            result = ( highByte << 8 ) | lowByte;
            return ERR_I2C_OK;
        }

        uint16_t readTwoBytes_DiffAddr_i2c( uint8_t _i2cAddress, uint8_t lowDataAddr, uint8_t highDataAddr ) {
            uint16_t result;
            ERROR_NO r = readTwoBytes_DiffAddr_i2c( _i2cAddress, lowDataAddr, highDataAddr, result );
            if ( r != ERR_I2C_OK ) { return 0; }
            return ERR_I2C_OK;
        }

        inline ERROR_NO readTwoBytes_DiffAddr( uint8_t lowDataAddr, uint8_t highDataAddr, uint16_t & result ) {
            return readTwoBytes_DiffAddr_i2c( _defaultI2cAddress, lowDataAddr, highDataAddr, result );
        }

        inline uint16_t readTwoBytes_DiffAddr( uint8_t lowDataAddr, uint8_t highDataAddr ) {
            return readTwoBytes_DiffAddr_i2c( _defaultI2cAddress, lowDataAddr, highDataAddr );
        }        

    //
    // READ 2 BYTES SAME ADDRESS
    //
    public:

        ERROR_NO readTwoBytes_SameAddr_HiLo_i2c( uint8_t _i2cAddress, uint8_t dataAddr, uint16_t & result ) {
            // read 2 bytes from same address
            //    1st byte to arrive is MSB (high byte)
            //    2nd byte to arrive is LSB (low byte)
            ERROR_NO r = readBytesCore( _i2cAddress, dataAddr, 2 );
            if ( r != ERR_I2C_OK ) { return r; }
            uint8_t highByte = _wire->read();
            uint8_t lowByte  = _wire->read();
            result = ( highByte << 8 ) | lowByte;
            return ERR_I2C_OK;
        }

        ERROR_NO readTwoBytes_SameAddr_LoHi_i2c( uint8_t _i2cAddress, uint8_t dataAddr, uint16_t &result ) {
            // read 2 bytes from same address
            //    1st byte to arrive is LSB (low byte)
            //    2nd byte to arrive is MSB (high byte)
            ERROR_NO r = readBytesCore( _i2cAddress, dataAddr, 2 );
            if ( r != ERR_I2C_OK ) { return r; }
            uint8_t lowByte  = _wire->read();
            uint8_t highByte = _wire->read();
            result = ( highByte << 8 ) | lowByte;
            return ERR_I2C_OK;
        }

        uint16_t readTwoBytes_SameAddr_HiLo_i2c( uint8_t _i2cAddress, uint8_t dataAddr ) {
            uint16_t result;
            ERROR_NO r = readTwoBytes_SameAddr_HiLo_i2c( _i2cAddress, dataAddr, result );
            if ( r != ERR_I2C_OK ) { return 0; }
            return result;
        }
        
        uint16_t readTwoBytes_SameAddr_LoHi_i2c( uint8_t _i2cAddress, uint8_t dataAddr ) {
            uint16_t result;
            ERROR_NO r = readTwoBytes_SameAddr_LoHi_i2c( _i2cAddress, dataAddr, result );
            if ( r != ERR_I2C_OK ) { return 0; }
            return result;
        }

        inline ERROR_NO readTwoBytes_SameAddr_HiLo( uint8_t dataAddr, uint16_t & result ) {
            return readTwoBytes_SameAddr_HiLo_i2c( _defaultI2cAddress, dataAddr, result );
        }

        inline ERROR_NO readTwoBytes_SameAddr_LoHi( uint8_t dataAddr, uint16_t & result ) {
            return readTwoBytes_SameAddr_LoHi_i2c( _defaultI2cAddress, dataAddr, result );
        }

        inline uint16_t readTwoBytes_SameAddr_HiLo( uint8_t dataAddr ) {
            return readTwoBytes_SameAddr_HiLo_i2c( _defaultI2cAddress, dataAddr );
        }
        
        inline uint16_t readTwoBytes_SameAddr_LoHi( uint8_t dataAddr ) {
            return readTwoBytes_SameAddr_LoHi_i2c( _defaultI2cAddress, dataAddr );
        }

    //
    // WRITE
    //
    public:

        ERROR_NO writeOneByte_i2c( uint8_t _i2cAddress, uint8_t data ) {
            _wire->beginTransmission( _i2cAddress );
            if ( !write( data ) ) return lastError;
            if ( !endTransmission() ) return lastError;
            return ERR_I2C_OK;
        }

        inline ERROR_NO writeOneByte( uint8_t data ) {
            return writeOneByte_i2c( _defaultI2cAddress, data );
        }

        ERROR_NO writeAddrAndData_i2c( uint8_t _i2cAddress, uint8_t dataAddr, uint8_t dataValue ) {
            _wire->beginTransmission( _i2cAddress );
            if ( !write( dataAddr ) ) return lastError;
            if ( !write( dataValue ) ) return lastError;
            if ( !endTransmission() ) return lastError;
            return ERR_I2C_OK;
        }

        inline ERROR_NO writeAddrAndData( uint8_t dataAddr, uint8_t dataValue ) {
            return writeAddrAndData_i2c( _defaultI2cAddress, dataAddr, dataValue );
        }

};

}
