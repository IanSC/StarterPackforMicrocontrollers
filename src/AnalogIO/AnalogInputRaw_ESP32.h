//  use faster routines for ESP32
//
//  ESP32 DevKitC v1, x1000 reads
//      Arduino analogRead()    91 ms
//      ESP32 style             45 ms
//
//  SEEED XIAO ESP32-S3, x1000 reads
//      Arduino analogRead()    56 ms
//      ESP32 style             24 ms
//
//  OLD:
//      Style           Code                               Time on ESP32 DevKitC
//      ESP32 native    v = adc1_get_raw( ADC_CHANNEL1 )   55 ms
//      Arduino         v = analogRead( PIN )              80 ms
//
//  if speed is close in future updates, this can be removed
//
//  tested only on limited boards
//  use flag to skip using these routines in case of errors, eg. use analogRead() only
//  define the flag before adding headers, specifically <AnalogInputRaw.h>
//
//  #define SP_ANALOGINPUT_SKIP_ESP32_ADC

#pragma once

#include <Arduino.h>

// from adc.h
#if CONFIG_IDF_TARGET_ESP32
    #define SP_ANALOGINPUT_USE_ESP32_ADC
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3 // TODO ESP32-S3 channels are wrong IDF-1776
    #define SP_ANALOGINPUT_USE_ESP32_ADC
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
    #define SP_ANALOGINPUT_USE_ESP32_ADC
#endif // CONFIG_IDF_TARGET_*

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3 // TODO ESP32-S3 channels are wrong IDF-1776
    #define SP_ANALOGINPUT_USE_ESP32_ADC
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
    #define SP_ANALOGINPUT_USE_ESP32_ADC
#endif

#if defined(SP_ANALOGINPUT_SKIP_ESP32_ADC)
    #undef SP_ANALOGINPUT_USE_ESP32_ADC
#endif

#if defined(SP_ANALOGINPUT_USE_ESP32_ADC)

// FOR ESP32 NATIVE READ
#include <driver/adc.h>

namespace StarterPack {

class AnalogInputRaw_ESP32 {

    //
    // READ ESP NATIVE METHOD
    //
    
    // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html

    private:

        uint8_t adcPort = 0;
        union {
            adc1_channel_t ADC_CHANNEL1;
            adc2_channel_t ADC_CHANNEL2;
        } adcChannel;

    protected:

        // /Users/<yourname>/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32c3/include/driver/include/driver/adc.h

        void ESP32_mapGPIOtoADC( int8_t PIN ) {
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
            case 36: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_0; break;
            case 37: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_1; break;
            case 38: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_2; break;
            case 39: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_3; break;
            case 32: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_4; break;
            case 33: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_5; break;
            case 34: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_6; break;
            case 35: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_7; break;            
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
            case  1: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_0; break;
            case  2: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_1; break;
            case  3: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_2; break;

            // https://esp32.com/viewtopic.php?t=27370
            // but ADC2 cannot be used with WiFi
            // case  4: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_3; break;

            case  5: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_4; break;
            case  6: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_5; break;
            case  7: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_6; break;
            case  8: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_7; break;
            case  9: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_8; break;
            case 10: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_9; break;
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
            case 0: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_0; break;
            case 1: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_1; break;
            case 2: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_2; break;
            case 3: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_3; break;
            case 4: adcPort = 1; adcChannel.ADC_CHANNEL1 = ADC1_CHANNEL_4; break;
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
            case  4: adcPort = 2; adcChannel.ADC_CHANNEL2 = ADC2_CHANNEL_0; break;
            case  0: adcPort = 2; adcChannel.ADC_CHANNEL2 = ADC2_CHANNEL_1; break;

            // conflicts with ADC1 ???
            // case  2: adcPort = 2; adcChannel.ADC_CHANNEL2 = ADC2_CHANNEL_2; break;

            case 15: adcPort = 2; adcChannel.ADC_CHANNEL2 = ADC2_CHANNEL_3; break;
            case 13: adcPort = 2; adcChannel.ADC_CHANNEL2 = ADC2_CHANNEL_4; break;
            case 12: adcPort = 2; adcChannel.ADC_CHANNEL2 = ADC2_CHANNEL_5; break;
            case 14: adcPort = 2; adcChannel.ADC_CHANNEL2 = ADC2_CHANNEL_6; break;
            case 27: adcPort = 2; adcChannel.ADC_CHANNEL2 = ADC2_CHANNEL_7; break;
            case 25: adcPort = 2; adcChannel.ADC_CHANNEL2 = ADC2_CHANNEL_8; break;
            case 26: adcPort = 2; adcChannel.ADC_CHANNEL2 = ADC2_CHANNEL_9; break;            
        #elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
            // /**** `adc2_channel_t` will be deprecated functions, combine into `adc_channel_t` ********/
            // typedef enum {
            //     ADC2_CHANNEL_0 = 0, /*!< ADC2 channel 0 is GPIO5 */
            //     ADC2_CHANNEL_MAX,
            // } adc2_channel_t;
            case 5: adcPort = 2; adcChannel.ADC_CHANNEL2 = ADC2_CHANNEL_0; break;
        #endif
            }
            if ( adcPort == 1 ) {
                adc1_config_width( ADC_WIDTH_BIT_12 );
                adc1_config_channel_atten( adcChannel.ADC_CHANNEL1, ADC_ATTEN_DB_11 );
            } else if ( adcPort == 2 )
                adc2_config_channel_atten( adcChannel.ADC_CHANNEL2, ADC_ATTEN_DB_11 );
        }

    //
    // READ
    //
    protected:

        int ESP32_readADC() {

            // ESP IDF style

            if ( adcPort == 1 )
                return adc1_get_raw( adcChannel.ADC_CHANNEL1 );
            else if ( adcPort == 2 ) {
                // just ignore error and pass whatever the function returns
                int rawValue;
                //esp_err_t err = 
                adc2_get_raw( adcChannel.ADC_CHANNEL2, ADC_WIDTH_BIT_12, &rawValue );
                // ESP_OK                if success
                // ESP_ERR_TIMEOUT       ADC2 is being used by other controller and the request timed out
                // ESP_ERR_INVALID_STATE The controller status is invalid. Please try again
                return rawValue;
            }
            return 0;
        }

};

}

#endif
