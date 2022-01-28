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
//      char*    errorMessage( ERROR_NO )   return error message string
//      bool     recoverIfHasError()        resets i2c if error has been found, return true if recovery attempt was made
//      uint16_t recoveryThrottleInMs       time between recovery attempts, default 2000 ms
//      ERROR_NO lastError                  last error detected, will not reset even if next calls succeeds
//                                          resets only upon calling recoverIfHasError(), clearLastError() or directly cleared
//      void     clearLastError()           clear lastError
//
//  Read 1 Byte
//
//      ERROR_NO readOneByte( dataAddr, &result )
//      ex. byte result;
//          ERROR_NO err = i2cHelper.readOneByte( dataAddress, result );
//          if ( err != I2C_ERROR_OK ) ... error found
//
//      uint8_t readOneByte( dataAddr )
//      ex. byte result = i2cHelper.readOneByte( dataAddress );
//          if ( i2cHelper.lastError != I2C_ERROR_OK ) ... error found
//
//  Read 2 Bytes from Different Address
//
//      ERROR_NO readTwoBytes_DiffAddr( lowDataAddr, highDataAddr, &result )
//      ex. uint16_t result;
//          ERROR_NO err = i2cHelper.readTwoBytes_DiffAddr( addr1, addr2, result );
//          if ( err != I2C_ERROR_OK ) ... error found
//
//      uint16_t readTwoBytes_DiffAddr( lowDataAddr, highDataAddr )
//      ex. uint16_t result = i2cHelper.readTwoBytes_DiffAddr( addr1, addr2 );
//          if ( i2cHelper.lastError != I2C_ERROR_OK ) ... error found
//
//  Read 2 Bytes from Same Address (ex. AS5600 magnetic encoder)
//
//      ERROR_NO readTwoBytes_SameAddr_HiLo( dataAddr, &result )
//      ERROR_NO readTwoBytes_SameAddr_LoHi( dataAddr, &result )
//      ex. uint16_t result;
//          ERROR_NO err = i2cHelper.readTwoBytes_SameAddr_HiLo( addr, result );
//          if ( err != I2C_ERROR_OK ) ... error found
//
//      uint16_t readTwoBytes_SameAddr_HiLo( dataAddr )        
//      uint16_t readTwoBytes_SameAddr_LoHi( dataAddr )
//      ex. uint16_t result = i2cHelper.readTwoBytes_SameAddr_LoHi( addr );
//          if ( i2cHelper.lastError != I2C_ERROR_OK ) ... error found
//
//  Ex:
//      TwoWireHelper i2cHelper = TwoWireHelper( 0x36 )
//      i2cHelper.recoveryThrottleInMs = 3000; // recover after every 3 seconds only
//  
//      while( true ) {
//          byte result;
//          ERROR_NO err = i2cHelper.readOneByte( 0x0B, result );
//          if ( err != I2C_ERROR_OK )
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
//      if ( i2cHelper.lastError != I2C_ERROR_OK ) {
//          Serial.println( i2cHelper.errorMessage( i2cHelper._lastError ) );
//          // ... reset device if necessary
//      }
//
//  To Explore:
//      https://github.com/stickbreaker/arduino-esp32/tree/master/libraries/Wire
//      https://github.com/esp8266/Arduino/issues/1025
//      http://www.forward.com.au/pfod/ArduinoProgramming/I2C_ClearBus/index.html

#pragma once
#include <Wire.h>

namespace StarterPack {

//
// ERROR CODES
//

    typedef uint8_t ERROR_NO;
    
    #if defined(ESP32)

        // ALREADY DEFINED: ~/.platformio/packages/framework-arduinoespressif32/cores/esp32/esp32-hal-i2c.h
        // typedef enum {
        //     I2C_ERROR_OK =      0,
        //     I2C_ERROR_DEV,      1
        //     I2C_ERROR_ACK,      2
        //     I2C_ERROR_TIMEOUT,  3
        //     I2C_ERROR_BUS,      4
        //     I2C_ERROR_BUSY,     5
        //     I2C_ERROR_MEMORY,   6
        //     I2C_ERROR_CONTINUE, 7
        //     I2C_ERROR_NO_BEGIN  8
        // } i2c_err_t;
        
    #else
    
        // match errors as close as possible to ESP32
    
        const ERROR_NO I2C_ERROR_OK        = 0;
        const ERROR_NO I2C_ERROR_MEMORY    = 1;  // 1 .. length too long for buffer
        const ERROR_NO I2C_ERROR_ACK       = 2;  // 2 .. address send, NACK received
        const ERROR_NO I2C_ERROR_BUSY      = 3;  // 3 .. data send, NACK received
        const ERROR_NO I2C_ERROR_BUS       = 4;  // 4 .. other twi error (lost bus arbitration, bus error, ..)
        const ERROR_NO I2C_ERROR_TIMEOUT   = 5;  // 5 .. timeout    
        const ERROR_NO I2C_ERROR_DEV       = 51; // unused - placeholders only so same error codes for ESP32/Arduino
        const ERROR_NO I2C_ERROR_CONTINUE  = 52;
        const ERROR_NO I2C_ERROR_NO_BEGIN  = 53;
    
        // twi calls that generates timeout errors:
        // - twi_readFrom()
        // - two_writeTo()
        // - twi_stop()
        // - ISR --> twi_stop()
        //
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
        // write()               twi_transmit(),                       *** ERRORS WERE IGNORED *** dumbf...
        //                       Output   1 length too long for buffer *** NOT HANDLED
        //                                2 not slave transmitter      *** NOT HANDLED
        //                                0 ok
        // requestFrom()         beginTransmission()
        //                       write()
        //                       endTransmission()
        //                       twi_readFrom() - timeout
    
        extern volatile uint32_t twi_timeout_us;
        
    #endif

    // additional errors possibly not handled in Wire.h
    const ERROR_NO I2C_ERROR_WRITE    = 101;
    const ERROR_NO I2C_ERROR_ENDTRANS = 102;
    const ERROR_NO I2C_ERROR_REQUEST  = 103;
    const ERROR_NO I2C_ERROR_TIMEOUT2 = 104;

class TwoWireHelper {

    private:

        TwoWire *_wire;
        uint8_t _defaultI2cAddress;

    public:

        TwoWireHelper( TwoWire &wire, uint8_t defaultI2cAddress ) {
            _wire = &wire;
            _defaultI2cAddress = defaultI2cAddress;
            setTimeoutInMs( 50 );
        }

        inline void setTimeoutInMs( uint16_t timeOut ) {
            #if defined(ESP32)
                _wire->setTimeOut( timeOut );
            #else
                _wire->setTimeout( timeOut * 1000 ); // Arduino in microsecs
            #endif
        }

        inline void setFrequency( uint32_t frequency ) {
            _wire->setClock( frequency );
        }

    //
    // ERROR MESSAGES
    //
    // https://github.com/arduino/ArduinoCore-avr/blob/master/libraries/Wire/src/utility/twi.c
    // https://github.com/arduino/ArduinoCore-avr/blob/master/libraries/Wire/src/utility/twi.h
    // https://github.com/arduino/ArduinoCore-avr/blob/master/libraries/Wire/src/Wire.cpp
    // https://github.com/arduino/ArduinoCore-avr/blob/master/libraries/Wire/src/Wire.h
    
    public:

        #if defined(ESP32)

            static const char* errorMessage( ERROR_NO errorNo ) {
                // ~/.platformio/packages/framework-arduinoespressif32/cores/esp32/esp32-hal-i2c.h
                switch ( errorNo ) {
                case I2C_ERROR_OK:       return "no error";                          // errors from Wire.h
                case I2C_ERROR_DEV:      return "I2C_ERROR_DEV";
                case I2C_ERROR_ACK:      return "NACK received";
                case I2C_ERROR_TIMEOUT:  return "timeout";
                case I2C_ERROR_BUS:      return "bus arbitration";
                case I2C_ERROR_BUSY:     return "bus busy";
                case I2C_ERROR_MEMORY:   return "memory related error";
                case I2C_ERROR_CONTINUE: return "stop not received";
                case I2C_ERROR_NO_BEGIN: return "I2C_ERROR_NO_BEGIN";
                case I2C_ERROR_WRITE:    return "invalid return from write()";       // additional errors
                case I2C_ERROR_ENDTRANS: return "invalid return from endTransmission()";
                case I2C_ERROR_REQUEST:  return "invalid requestFrom() length";
                case I2C_ERROR_TIMEOUT2: return "uncaught timeout";
                default:                 return "unknown error";
                }
            }

        #else
    
            static const char* errorMessage( ERROR_NO errorNo ) {
                switch ( errorNo ) {
                case I2C_ERROR_OK:       return "no error";                          // errors from Wire.h
                case I2C_ERROR_MEMORY:   return "length too long for buffer";
                case I2C_ERROR_ACK:      return "address send, NACK received";
                case I2C_ERROR_BUSY:     return "data send, NACK received";
                case I2C_ERROR_BUS:      return "other twi error";
                case I2C_ERROR_TIMEOUT:  return "timeout";
                case I2C_ERROR_DEV:                                                  // unused
                case I2C_ERROR_CONTINUE:
                case I2C_ERROR_NO_BEGIN: return "( placeholder only )";
                case I2C_ERROR_WRITE:    return "invalid return value from write()"; // additional errors
                case I2C_ERROR_ENDTRANS: return "invalid return value from endTransmission()";
                case I2C_ERROR_REQUEST:  return "invalid requestFrom() length";
                case I2C_ERROR_TIMEOUT2: return "uncaught timeout";
                default:                 return "unknown error";
                }
            }
            
        #endif
        
    //
    // LAST ERROR
    //
    public:

        ERROR_NO lastError = I2C_ERROR_OK;

        inline void clearLastError() { lastError = I2C_ERROR_OK; }
        
    private:
    
        inline void RecordError( ERROR_NO newError ) {
            lastError = newError;
        }

        #if defined(ESP32)
        
            inline bool CheckAndRecordError() {
                if ( _wire->lastError() == I2C_ERROR_OK )
                    return true;
                else {
                    lastError = _wire->lastError();
                    return false;
                }
            }
            
        #else
        
            inline bool CheckAndRecordError() {
                if ( _wire->getWireTimeoutFlag() ) {
                    lastError = I2C_ERROR_TIMEOUT;
                    return false;
                } else
                    return true;
            }
            
        #endif

    //
    // VERIFY
    //
    public:

        bool verify() {
            return ( verifyWithError( _defaultI2cAddress ) == I2C_ERROR_OK );
        }
        
        bool verify( uint8_t _i2cAddress ) {
            return ( verifyWithError( _i2cAddress ) == I2C_ERROR_OK );
        }

        inline ERROR_NO verifyWithError() {
            return verifyWithError( _defaultI2cAddress );
        }

        ERROR_NO verifyWithError( uint8_t _i2cAddress ) {
            _wire->beginTransmission( _i2cAddress );
            if ( !endTransmission() ) return lastError;
            return I2C_ERROR_OK;
        }

    //
    // RECOVERY
    //
    private:

        uint32_t lastRecovery = millis();

    public:

        uint16_t recoveryThrottleInMs = 2000;

        #if defined(ESP32)

            bool recoverIfHasError( uint8_t _i2cAddress ) {
                if ( lastError == I2C_ERROR_OK && _wire->lastError() == I2C_ERROR_OK )
                    return false;
                
                // try to recover every recoveryThrottleInMs only
                uint32_t now = millis();
                if ( now - lastRecovery > recoveryThrottleInMs ) {
                    lastRecovery = now;

                    //Serial.println( "RECO: " );
                    //if ( lastError != I2C_ERROR_OK )
                    //   Serial.printf( "   Last Error = %s\n", errorMessage( lastError ) );
                    //if ( _wire->lastError() != I2C_ERROR_OK )
                    //   Serial.printf( "   Wire Last Error = %s\n", errorMessage( _wire->lastError() ) );

                    _wire->begin();
                    lastError = I2C_ERROR_OK;
                    return true;
                }
                return false;
            }

        #else

            bool recoverIfHasError( uint8_t _i2cAddress ) {
                if ( lastError == I2C_ERROR_OK )
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
                    
                    lastError = I2C_ERROR_OK;
                    _wire->clearWireTimeoutFlag();
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
    
         #if defined(ESP32)
         
            inline bool endTransmission() {
                // int8_t err =
                _wire->endTransmission();
                if ( !CheckAndRecordError() ) return false;
                // NO NEED FOR ESP32 lastError() detected it
                // if ( err != I2C_ERROR_OK ) { RecordError( I2C_ERROR_ENDTRANS ); return false; }
                return true;
            }
            
            inline bool write( uint8_t data ) {
                // size_t bytesWritten =
                _wire->write( data );
                if ( !CheckAndRecordError() ) return false;
                // NO NEED FOR ESP32 lastError() detected it
                //if ( bytesWritten != 1 ) { RecordError( I2C_ERROR_WRITE ); return false; }
                return true;
            }

            inline bool available( uint8_t length ) {
                // TIMEOUT IN MILLIS
                if ( _wire->available() < length ) {
                    uint32_t start = millis();
                    while ( _wire->available() < length ) {
                        if ( millis() - start >= _wire->getTimeOut() ) { RecordError( I2C_ERROR_TIMEOUT2 ); return false; }
                        if ( !CheckAndRecordError() ) return false;
                    }
                }
                return CheckAndRecordError();
            }
            
        #else
        
            inline bool endTransmission() {
                int8_t err = _wire->endTransmission();
                if ( !CheckAndRecordError() ) return false;
                if ( err != 0 ) { RecordError( I2C_ERROR_ENDTRANS ); return false; }
                return true;
            }
            
            inline bool write( uint8_t data ) {
                size_t bytesWritten = _wire->write( data );
                if ( !CheckAndRecordError() ) return false;
                // NEEDED FOR ARDUINO
                if ( bytesWritten != 1 ) { RecordError( I2C_ERROR_WRITE ); return false; }
                return true;
            }

            inline bool available( uint8_t length ) {
                // TIMEOUT IN MICROS
                if ( _wire->available() < length ) {
                    uint32_t start = micros();
                    while ( _wire->available() < length ) {
                        if ( micros() - start >= twi_timeout_us ) { RecordError( I2C_ERROR_TIMEOUT2 ); return false; }
                        if ( !CheckAndRecordError() ) return false;
                    }
                }
                return CheckAndRecordError();
            }
            
        #endif

        inline bool requestFrom( uint8_t _i2cAddress, uint8_t length ) {
            size_t bytesArrived = _wire->requestFrom( _i2cAddress, length );
            if ( !CheckAndRecordError() ) return false;
            // NEEDED FOR ARDUINO / ESP32
            if ( bytesArrived != length ) { RecordError( I2C_ERROR_REQUEST ); return false; }
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
            return I2C_ERROR_OK;
        }

    //
    // READ ONE BYTE
    //
    public:

        ERROR_NO readOneByte_i2c( uint8_t _i2cAddress, uint8_t dataAddr, uint8_t &result ) {
            ERROR_NO r = readBytesCore( _i2cAddress, dataAddr, 1 );
            if ( r != I2C_ERROR_OK ) { return r; }
            result = _wire->read();
            return I2C_ERROR_OK;
        }

        uint8_t readOneByte_i2c( uint8_t _i2cAddress, uint8_t dataAddr ) {
            uint8_t result;
            ERROR_NO r = readOneByte_i2c( _i2cAddress, dataAddr, result );
            if ( r != I2C_ERROR_OK ) { return 0; }
            return result;
        }

        inline ERROR_NO readOneByte( uint8_t dataAddr, uint8_t &result ) {
            return readOneByte_i2c( _defaultI2cAddress, dataAddr, result );
        }

        inline uint8_t readOneByte( uint8_t dataAddr ) {
            return readOneByte_i2c( _defaultI2cAddress, dataAddr );
        }

    //
    // READ 2 BYTES SEPARATELY
    //
    public:
        
        ERROR_NO readTwoBytes_DiffAddr_i2c( uint8_t _i2cAddress, uint8_t lowDataAddr, uint8_t highDataAddr, uint16_t &result ) {
            // read 2 bytes from different addresses, assemble to result
            //    lowDataAddr  is LSB (low byte)
            //    highDataAddr is MSB (high byte)
            uint8_t lowByte, highByte;
            ERROR_NO r = readOneByte_i2c( _i2cAddress, lowDataAddr, lowByte );
            if ( r != I2C_ERROR_OK ) { return r; }
            r = readOneByte_i2c( _i2cAddress, highDataAddr, highByte );
            if ( r != I2C_ERROR_OK ) { return r; }
            result = ( highByte << 8 ) | lowByte;
            return I2C_ERROR_OK;
        }

        uint16_t readTwoBytes_DiffAddr_i2c( uint8_t _i2cAddress, uint8_t lowDataAddr, uint8_t highDataAddr ) {
            uint16_t result;
            ERROR_NO r = readTwoBytes_DiffAddr_i2c( _i2cAddress, lowDataAddr, highDataAddr, result );
            if ( r != I2C_ERROR_OK ) { return 0; }
            return I2C_ERROR_OK;
        }

        inline ERROR_NO readTwoBytes_DiffAddr( uint8_t lowDataAddr, uint8_t highDataAddr, uint16_t &result ) {
            return readTwoBytes_DiffAddr_i2c( _defaultI2cAddress, lowDataAddr, highDataAddr, result );
        }

        inline uint16_t readTwoBytes_DiffAddr( uint8_t lowDataAddr, uint8_t highDataAddr ) {
            return readTwoBytes_DiffAddr_i2c( _defaultI2cAddress, lowDataAddr, highDataAddr );
        }        

    //
    // READ 2 BYTES SAME ADDRESS
    //
    public:

        ERROR_NO readTwoBytes_SameAddr_HiLo_i2c( uint8_t _i2cAddress, uint8_t dataAddr, uint16_t &result ) {
            // read 2 bytes from same address
            //    1st byte to arrive is MSB (high byte)
            //    2nd byte to arrive is LSB (low byte)
            ERROR_NO r = readBytesCore( _i2cAddress, dataAddr, 2 );
            if ( r != I2C_ERROR_OK ) { return r; }
            uint8_t highByte = _wire->read();
            uint8_t lowByte  = _wire->read();
            result = ( highByte << 8 ) | lowByte;
            return I2C_ERROR_OK;
        }

        ERROR_NO readTwoBytes_SameAddr_LoHi_i2c( uint8_t _i2cAddress, uint8_t dataAddr, uint16_t &result ) {
            // read 2 bytes from same address
            //    1st byte to arrive is LSB (low byte)
            //    2nd byte to arrive is MSB (high byte)
            ERROR_NO r = readBytesCore( _i2cAddress, dataAddr, 2 );
            if ( r != I2C_ERROR_OK ) { return r; }
            uint8_t lowByte  = _wire->read();
            uint8_t highByte = _wire->read();
            result = ( highByte << 8 ) | lowByte;
            return I2C_ERROR_OK;
        }

        uint16_t readTwoBytes_SameAddr_HiLo_i2c( uint8_t _i2cAddress, uint8_t dataAddr ) {
            uint16_t result;
            ERROR_NO r = readTwoBytes_SameAddr_HiLo_i2c( _i2cAddress, dataAddr, result );
            if ( r != I2C_ERROR_OK ) { return 0; }
            return result;
        }
        
        uint16_t readTwoBytes_SameAddr_LoHi_i2c( uint8_t _i2cAddress, uint8_t dataAddr ) {
            uint16_t result;
            ERROR_NO r = readTwoBytes_SameAddr_LoHi_i2c( _i2cAddress, dataAddr, result );
            if ( r != I2C_ERROR_OK ) { return 0; }
            return result;
        }

        inline ERROR_NO readTwoBytes_SameAddr_HiLo( uint8_t dataAddr, uint16_t &result ) {
            return readTwoBytes_SameAddr_HiLo_i2c( _defaultI2cAddress, dataAddr, result );
        }

        inline ERROR_NO readTwoBytes_SameAddr_LoHi( uint8_t dataAddr, uint16_t &result ) {
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
            return I2C_ERROR_OK;
        }

        inline ERROR_NO writeOneByte( uint8_t data ) {
            return writeOneByte_i2c( _defaultI2cAddress, data );
        }

        ERROR_NO writeAddrAndData_i2c( uint8_t _i2cAddress, uint8_t dataAddr, uint8_t dataValue ) {
            _wire->beginTransmission( _i2cAddress );
            if ( !write( dataAddr ) ) return lastError;
            if ( !write( dataValue ) ) return lastError;
            if ( !endTransmission() ) return lastError;
            return I2C_ERROR_OK;
        }

        inline ERROR_NO writeAddrAndData( uint8_t dataAddr, uint8_t dataValue ) {
            return writeAddrAndData_i2c( _defaultI2cAddress, dataAddr, dataValue );
        }

};

}
