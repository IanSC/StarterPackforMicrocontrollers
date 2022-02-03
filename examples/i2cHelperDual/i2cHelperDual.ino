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
#include <Wire.h>

#define i2cAddress  0x36
#define dataAddress 0x0C

// defaults to using "Wire"
i2cHelper iHelper = i2cHelper( i2cAddress );

LCD_i2c lcd = LCD_i2c( iHelper );

//LCD_i2c lcd = LCD_i2c();
//i2cHelper iHelper = i2cHelper( Wire1, i2cAddress );

//
// SETUP
//
    
    void setup() {
        Serial.begin( 115200 );
        while( !Serial );

        // Wire1.begin(16,17);

        Wire.begin();
        iHelper.setTimeoutInMs( 100 );
        iHelper.setFrequency( 800000 );

        lcd.begin( 16, 2 );
    }

//
// LOOP
//

    uint16_t counter = 0;
    uint16_t errCount1 = 0, errCount2 = 0;
    
    void loop() {

        uint16_t result1, result2;
        //uint8_t result1, result2;

        lcd.printfAt( 0, 0, "%5d", counter );
        counter++;

        //
        // STYLE 1 - get error and data
        //
        
        ERROR_NO err = iHelper.readTwoBytes_SameAddr_HiLo( dataAddress, result1 );
        // ERROR_NO err = iHelper.readOneByte( dataAddress, result1 );
        if ( err != i2cHelper::ERR_I2C_OK ) {
            SerialPrintf( "(1) %s\n", i2cHelper::errorMessage( err ) );
            lcd.printAt( 6, 0, "*****" );
            errCount1++;
            lcd.printfAt( 12, 0, "%5d", errCount1 );
        } else {
            SerialPrintf( "(1) %d = %d\n", counter, result1 );
            lcd.printfAt( 6, 0, "%5d", result1 );
        }

        //
        // STYLE 2 - get data and check error
        //

        result2 = iHelper.readTwoBytes_SameAddr_HiLo( dataAddress );
        // result2 = iHelper.readOneByte( dataAddress );
        if ( iHelper.lastError != i2cHelper::ERR_I2C_OK ) {
            SerialPrintf( "(2) %s\n", i2cHelper::errorMessage( iHelper.lastError ) );
            lcd.printAt( 6, 1, "*****" );
            errCount2++;
            lcd.printfAt( 12, 1, "%5d", errCount2 );
        } else {            
            SerialPrintf( "(2) %d = %d\n", counter, result2 );
            lcd.printfAt( 6, 1, "%5d", result2 );
        }

        // if this is added, possible to recover from i2c errors
        // try disconnecting and reconnecting any/all
        // i2c wires (even power cables)
        if ( lcd.recoverIfHasError() ) {
            // recover lcd first, since it needs to
            // reset on error, eg. power disconnected
            // must reset to 4bit mode
            Serial.println( "i2c recovered" );
            lcd.printfAt( 12, 0, "%5d", errCount1 );
            lcd.printfAt( 12, 1, "%5d", errCount2 );
        }
        if ( iHelper.recoverIfHasError() ) {
            // returns true if there was an error
            // and recovery was attempted
            Serial.println( "i2c recovered" );
        }

        delay(50);
    }
