//=====================================
//  StarterPack i2cHelper Dual Example
//  2022-01 isc
//=====================================
//
//  i2cHelper
//  ---------
//  - connect to two i2c devices
//  - catch i2c errors
//  - support for recovery
//    disconnect/reconnect VCC, GND, SDA. SCL
//  - defines error codes

#include <StarterPack.h> // include all
#include <i2cHelper.h>   // ... or only those used in this project
#include <LCD_i2c.h>
using namespace StarterPack;

#define i2cAddress  0x36
#define dataAddress 0x0C

// defaults to using "Wire"
i2cHelper iHelper = i2cHelper( i2cAddress );

LCD_i2c lcd = LCD_i2c( iHelper );

//
// SETUP
//
    
    void setup() {
        Serial.begin( 115200 );
        while( !Serial );

        Wire.begin();
        iHelper.setTimeoutInMs( 100 );
        iHelper.setFrequency( 800000 );

        lcd.begin( 16, 2 );
    }

//
// LOOP
//

    uint16_t counter = 0;
    
    void loop() {

        uint16_t result;

        //
        // STYLE 1 - get error and data
        //
        Serial.print( counter );
        Serial.print( " - " );
        lcd.setCursor( 0, 0 );
        lcd.print( counter );
        lcd.print( "       " );
        
        ERROR_NO err = iHelper.readTwoBytes_SameAddr_HiLo( dataAddress, result );
        // ERROR_NO err = iHelper.readOneByte( dataAddress, result );
        lcd.setCursor( 8, 0 );
        if ( err != i2cHelper::ERR_I2C_OK ) {
            Serial.println( i2cHelper::errorMessage( err ) );
            lcd.print( "********" );
        } else {
            Serial.println( result );
            lcd.print( result );
            lcd.print( "       " );
        }
        counter++;

        //
        // STYLE 2 - get data and check error
        //
        Serial.print( counter++ );
        Serial.print( " - " );
        lcd.setCursor( 0, 1 );
        lcd.print( counter );
        lcd.print( "       " );

        result = iHelper.readTwoBytes_SameAddr_HiLo( dataAddress );
        // result = iHelper.readOneByte( dataAddress );
        lcd.setCursor( 8, 1 );
        if ( iHelper.lastError != i2cHelper::ERR_I2C_OK ) {
            Serial.println( i2cHelper::errorMessage( err ) );
            lcd.print( "********" );
        } else {
            Serial.println( result );
            lcd.print( result );
            lcd.print( "       " );
        }
        counter++;

        // if this is added, possible to recover from i2c errors
        // try disconnecting and reconnecting any/all
        // i2c wires (even power cables)
        if ( lcd.recoverIfHasError() ) {
            // recover lcd first, since it needs to
            // reset on error, eg. power disconnected
            // must reset to 4bit mode
            Serial.println( "i2c recovered" );
        }
        if ( iHelper.recoverIfHasError() ) {
            // returns true if there was an error
            // and recovery was attempted
            Serial.println( "i2c recovered" );
        }

        delay(50);
    }
