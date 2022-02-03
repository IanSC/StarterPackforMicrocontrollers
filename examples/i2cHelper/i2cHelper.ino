//=====================================
//  StarterPack i2cHelper Example
//  2022-01 isc
//=====================================
//
//  i2cHelper
//  ---------
//  - catch i2c errors
//  - support for recovery
//    disconnect/reconnect VCC, GND, SDA. SCL
//  - defines error codes

#include <StarterPack.h> // include all
#include <i2cHelper.h>   // ... or only those used in this project

using namespace StarterPack;

#define i2cAddress  0x33
#define dataAddress 0x10

// defaults to using "Wire"
i2cHelper iHelper = i2cHelper( i2cAddress );

//
// SETUP
//
    
    void setup() {
        Serial.begin( 115200 );
        while( !Serial );

        Wire.begin();
        iHelper.setTimeoutInMs( 100 );
        iHelper.setFrequency( 800000 );
    }

//
// LOOP
//

    int counter = 0;
    
    void loop() {

        byte result;

        //
        // STYLE 1 - get error and data
        //
        Serial.print( counter++ );
        ERROR_NO err = iHelper.readOneByte( dataAddress, result );
        if ( err != i2cHelper::ERR_I2C_OK ) {
            Serial.println( i2cHelper::errorMessage( err ) );
        } else {
            Serial.println( result );
        }

        //
        // STYLE 2 - get data and check error
        //
        Serial.print( counter++ );
        result = iHelper.readOneByte( dataAddress );
        if ( iHelper.lastError != i2cHelper::ERR_I2C_OK ) {
            Serial.println( i2cHelper::errorMessage( iHelper.lastError ) );
        } else {
            Serial.println( result );
        }

        // if this is added, possible to recover from i2c errors
        // try disconnecting and reconnecting any/all
        // i2c wires (even power cables)
        if ( iHelper.recoverIfHasError() ) {
            // returns true if there was an error
            // and recovery was attempted
            Serial.println( "i2c recovered" );
        }
    }
