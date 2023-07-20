#pragma once

#include <Arduino.h>

// FOR ESP32 NATIVE READ
#include <driver/adc.h>

namespace StarterPack {

class AnalogInputBase_ESP32 {

    //
    // READ ESP NATIVE METHOD
    //
    
    // FOR ESP32 DevKitC
    //
    //  Style           Code                               Time on ESP32 DevKitC
    //  ESP32 native    v = adc1_get_raw( ADC_CHANNEL1 )   55 ms
    //  Arduino         v = analogRead( PIN )              80 ms
    //
    //  if speed is close in the future updates, this section can be removed
    //
    // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html

    protected:

        uint8_t ADC_PORT = 0;
        adc1_channel_t ADC_CHANNEL1;
        adc2_channel_t ADC_CHANNEL2;

        // /Users/ianco/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32c3/include/driver/include/driver/adc.h

        inline void mapGPIOtoADC(uint8_t PIN) {
            // don't know how to map GPIO to ADC channel
            // ~/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/driver/driver/adc.h
            switch( PIN ) {

        #if CONFIG_IDF_TARGET_ESP32
            // /**** `adc1_channel_t` will be deprecated functions, combine into `adc_channel_t` ********/
            // typedef enum {
            //     ADC1_CHANNEL_0 = 0, /*!< ADC1 channel 0 is GPIO36 */
            //     ADC1_CHANNEL_1,     /*!< ADC1 channel 1 is GPIO37 */
            //     ADC1_CHANNEL_2,     /*!< ADC1 channel 2 is GPIO38 */
            //     ADC1_CHANNEL_3,     /*!< ADC1 channel 3 is GPIO39 */
            //     ADC1_CHANNEL_4,     /*!< ADC1 channel 4 is GPIO32 */
            //     ADC1_CHANNEL_5,     /*!< ADC1 channel 5 is GPIO33 */
            //     ADC1_CHANNEL_6,     /*!< ADC1 channel 6 is GPIO34 */
            //     ADC1_CHANNEL_7,     /*!< ADC1 channel 7 is GPIO35 */
            //     ADC1_CHANNEL_MAX,
            // } adc1_channel_t;
            case 36: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_0; break;
            case 37: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_1; break;
            case 38: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_2; break;
            case 39: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_3; break;
            case 32: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_4; break;
            case 33: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_5; break;
            case 34: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_6; break;
            case 35: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_7; break;            
        #elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3 // TODO ESP32-S3 channels are wrong IDF-1776
            // /**** `adc1_channel_t` will be deprecated functions, combine into `adc_channel_t` ********/
            // typedef enum {
            //     ADC1_CHANNEL_0 = 0, /*!< ADC1 channel 0 is GPIO1  */
            //     ADC1_CHANNEL_1,     /*!< ADC1 channel 1 is GPIO2  */
            //     ADC1_CHANNEL_2,     /*!< ADC1 channel 2 is GPIO3  */
            //     ADC1_CHANNEL_3,     /*!< ADC1 channel 3 is GPIO4  */
            //     ADC1_CHANNEL_4,     /*!< ADC1 channel 4 is GPIO5  */
            //     ADC1_CHANNEL_5,     /*!< ADC1 channel 5 is GPIO6  */
            //     ADC1_CHANNEL_6,     /*!< ADC1 channel 6 is GPIO7  */
            //     ADC1_CHANNEL_7,     /*!< ADC1 channel 7 is GPIO8  */
            //     ADC1_CHANNEL_8,     /*!< ADC1 channel 8 is GPIO9  */
            //     ADC1_CHANNEL_9,     /*!< ADC1 channel 9 is GPIO10 */
            //     ADC1_CHANNEL_MAX,
            // } adc1_channel_t;
            case  1: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_0; break;
            case  2: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_1; break;
            case  3: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_2; break;
            case  4: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_3; break;
            case  5: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_4; break;
            case  6: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_5; break;
            case  7: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_6; break;
            case  8: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_7; break;
            case  9: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_8; break;
            case 10: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_9; break;
        #elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
            // /**** `adc1_channel_t` will be deprecated functions, combine into `adc_channel_t` ********/
            // typedef enum {
            //     ADC1_CHANNEL_0 = 0, /*!< ADC1 channel 0 is GPIO0 */
            //     ADC1_CHANNEL_1,     /*!< ADC1 channel 1 is GPIO1 */
            //     ADC1_CHANNEL_2,     /*!< ADC1 channel 2 is GPIO2 */
            //     ADC1_CHANNEL_3,     /*!< ADC1 channel 3 is GPIO3 */
            //     ADC1_CHANNEL_4,     /*!< ADC1 channel 4 is GPIO4 */
            //     ADC1_CHANNEL_MAX,
            // } adc1_channel_t;
            case 0: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_0; break;
            case 1: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_1; break;
            case 2: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_2; break;
            case 3: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_3; break;
            case 4: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_4; break;
        #endif // CONFIG_IDF_TARGET_*

        #if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3 // TODO ESP32-S3 channels are wrong IDF-1776
            // /**** `adc2_channel_t` will be deprecated functions, combine into `adc_channel_t` ********/
            // typedef enum {
            //     ADC2_CHANNEL_0 = 0, /*!< ADC2 channel 0 is GPIO4  (ESP32), GPIO11 (ESP32-S2) */
            //     ADC2_CHANNEL_1,     /*!< ADC2 channel 1 is GPIO0  (ESP32), GPIO12 (ESP32-S2) */
            //     ADC2_CHANNEL_2,     /*!< ADC2 channel 2 is GPIO2  (ESP32), GPIO13 (ESP32-S2) */
            //     ADC2_CHANNEL_3,     /*!< ADC2 channel 3 is GPIO15 (ESP32), GPIO14 (ESP32-S2) */
            //     ADC2_CHANNEL_4,     /*!< ADC2 channel 4 is GPIO13 (ESP32), GPIO15 (ESP32-S2) */
            //     ADC2_CHANNEL_5,     /*!< ADC2 channel 5 is GPIO12 (ESP32), GPIO16 (ESP32-S2) */
            //     ADC2_CHANNEL_6,     /*!< ADC2 channel 6 is GPIO14 (ESP32), GPIO17 (ESP32-S2) */
            //     ADC2_CHANNEL_7,     /*!< ADC2 channel 7 is GPIO27 (ESP32), GPIO18 (ESP32-S2) */
            //     ADC2_CHANNEL_8,     /*!< ADC2 channel 8 is GPIO25 (ESP32), GPIO19 (ESP32-S2) */
            //     ADC2_CHANNEL_9,     /*!< ADC2 channel 9 is GPIO26 (ESP32), GPIO20 (ESP32-S2) */
            //     ADC2_CHANNEL_MAX,
            // } adc2_channel_t;            
            case  4: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_0; break;
            case  0: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_1; break;
            case  2: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_2; break;
            case 15: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_3; break;
            case 13: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_4; break;
            case 12: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_5; break;
            case 14: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_6; break;
            case 27: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_7; break;
            case 25: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_8; break;
            case 26: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_9; break;            
        #elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
            // /**** `adc2_channel_t` will be deprecated functions, combine into `adc_channel_t` ********/
            // typedef enum {
            //     ADC2_CHANNEL_0 = 0, /*!< ADC2 channel 0 is GPIO5 */
            //     ADC2_CHANNEL_MAX,
            // } adc2_channel_t;
            case 5: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_0; break;
        #endif
            }
            if ( ADC_PORT == 1 ) {
                adc1_config_width( ADC_WIDTH_BIT_12 );
                adc1_config_channel_atten( ADC_CHANNEL1, ADC_ATTEN_DB_11 );
            } else if ( ADC_PORT == 2 )
                adc2_config_channel_atten( ADC_CHANNEL2, ADC_ATTEN_DB_11 );
        }

/*
        inline void mapGPIOtoADC() {
            // don't know how to map GPIO to ADC channel
            // ~/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/driver/driver/adc.h
            switch( PIN ) {
            case 36: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_0; break;
            case 37: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_1; break;
            case 38: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_2; break;
            case 39: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_3; break;
            case 32: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_4; break;
            case 33: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_5; break;
            case 34: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_6; break;
            case 35: ADC_PORT = 1; ADC_CHANNEL1 = ADC1_CHANNEL_7; break;
            case  4: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_0; break;
            case  0: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_1; break;
            case  2: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_2; break;
            case 15: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_3; break;
            case 13: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_4; break;
            case 12: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_5; break;
            case 14: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_6; break;
            case 27: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_7; break;
            case 25: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_8; break;
            case 26: ADC_PORT = 2; ADC_CHANNEL2 = ADC2_CHANNEL_9; break;
            }
            if ( ADC_PORT == 1 ) {
                adc1_config_width( ADC_WIDTH_BIT_12 );
                adc1_config_channel_atten( ADC_CHANNEL1, ADC_ATTEN_DB_11 );
            } else if ( ADC_PORT == 2 )
                adc2_config_channel_atten( ADC_CHANNEL2, ADC_ATTEN_DB_11 );
        }
*/

    //
    // READ
    //
    protected:

        inline int readESPStyle() {

            // ESP IDF style

            if ( ADC_PORT == 1 )
                return adc1_get_raw( ADC_CHANNEL1 );
            else if ( ADC_PORT == 2 ) {
                // just ignore error and pass whatever the function returns
                int rawValue;
                //esp_err_t err = 
                adc2_get_raw( ADC_CHANNEL2, ADC_WIDTH_BIT_12, &rawValue );
                // ESP_OK                if success
                // ESP_ERR_TIMEOUT       ADC2 is being used by other controller and the request timed out
                // ESP_ERR_INVALID_STATE The controller status is invalid. Please try again
                return rawValue;
            }
            return 0;
        }

};

}
