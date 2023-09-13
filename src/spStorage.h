#pragma once
#include <Preferences.h>
#include <nvs_flash.h>

#include <UserInterface.h>
#include <LCDSupport/LCDUtility.h>

// nvs_open failed: NOT_INITIALIZED
// https://github.com/espressif/arduino-esp32/issues/3421
//
// nvs_open failed: NOT_FOUND

// Preferences p;

namespace StarterPack {

class spStorage {

        const char *PREF_FILE;    // = "DATA";
        uint8_t     VALID_FLAG;   // = 0x23;
        uint8_t     INVALID_FLAG; // = 0x34;
        
    public:

        spStorage( const char *PREF_FILE = "DATA", uint8_t VALID_FLAG = 0x23, uint8_t INVALID_FLAG = 0x34 ) {
            this->PREF_FILE = PREF_FILE;
            this->VALID_FLAG = VALID_FLAG;
            this->INVALID_FLAG = INVALID_FLAG;
            ESP_ERROR_CHECK( nvs_flash_init() );
            // open as read/write to force creation
            // if still missing and opened as read only
            //    error: [Preferences.cpp:50] begin(): nvs_open failed: NOT_FOUND
            // ex. spStorage a;
            //     a.isValid("db") --> nvs_open failed: NOT_FOUND
            //     if storage has not been created yet
            //
            // also check if key exists, when opening db as read only
            // ex. Preferences p;
            //     p.begin("db",true);
            //     p.getUChar("key",10); ==> will hang, if "key" does not exist
            //     so check first:
            //        if ( p.isKey("key") ...
            Preferences p;
            if ( !p.begin( PREF_FILE, false ) ) {
                // failed to create ???
            }
            p.end();
        }

    //
    // WIPE
    //
    public:

        bool wipeCore() {
            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, false ) )
                    break;
                r = p.clear();
                break;
            }
            p.end();
            return r;
        }

        enum wipeResult {
            wOkay,
            wError,
            wCancelled
        };

        wipeResult wipeWithPrompt( uint8_t ERR_SOURCE, uint8_t ERR_STORAGE_WIPE ) {
            namespace ui = StarterPack::UserInterface;

            ui::LCD->clear();
            uint8_t input = StarterPack::spLCD::optionChooser::chooseNoYes( "Erase Data?" );
            // uint8_t input = StarterPack::LCDUtility::promptNoYes( "Erase Data?" )->prompt();

            if ( input == 2 ) {
                if ( wipeCore() ) {
                    ui::promptDialog1( "RESET", "data erased" );
                    return wipeResult::wOkay;
                } else {
                    ui::promptErrorDialog1( ERR_SOURCE, ERR_STORAGE_WIPE, "error erasing data" );
                    return wipeResult::wError;
                }
            } else {
                ui::LCD->printStrAtRow( 3, "CANCELLED" );
                // ui::LCD->displayAll();
                ui::LCD->refresh();
                ui::waitForAnyKeyPressed();
                return wipeResult::wCancelled;
            }
        }

    //
    // VALIDITY
    //
    public:

        enum result {
            okay,
            invalid,
            //restored, // extra for callers
            error
        };

        result isValid( const char *validityKey ) {
            Preferences p;
            result r = result::error;
            while( true ) {
                if ( !p.begin( PREF_FILE, true ) )
                    break;
                if ( !p.isKey( validityKey ) )
                    break;
                if ( p.getUChar( validityKey, INVALID_FLAG ) != VALID_FLAG ) {
                    r = result::invalid;
                    break;
                }
                r = result::okay;
                break;
            }
            p.end();
            return r;
        }

        bool setValid( const char *validityKey ) {
            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, false ) )
                    break;
                if ( p.putUChar( validityKey, VALID_FLAG ) != 1 )
                    break;
                r = true;
                break;
            }
            p.end();
            return r;
        }

        bool setInvalid( const char *validityKey ) {
            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, false ) )
                    break;
                if ( p.putUChar( validityKey, INVALID_FLAG ) != 1 )
                    break;
                r = true;
                break;
            }
            p.end();
            return r;
        }

    //
    // READ
    //
    public:

        template <typename T>
        bool readData( const char *dataKey, T &data ) {
            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, true ) )
                    break;
                if ( !p.isKey( dataKey ) )
                    break;                    
                if ( p.getBytes( dataKey, &data, sizeof(T) ) != sizeof(T) )
                    break;
                r = true;
                break;
            }
            p.end();
            return r;
        }

        template <typename T>
        result readData( const char *validityKey, const char *dataKey, T &data ) {
            Preferences p;
            result r = result::error;
            while( true ) {
                if ( !p.begin( PREF_FILE, true ) )
                    break;
                if ( !p.isKey( validityKey ) )
                    break;
                if ( p.getUChar( validityKey, INVALID_FLAG ) != VALID_FLAG ) {
                    r = result::invalid;
                    break;
                }
                if ( !p.isKey( dataKey ) )
                    break;
                if ( p.getBytes( dataKey, &data, sizeof(T) ) != sizeof(T) )
                    break;
                r = result::okay;
                break;
            }
            p.end();
            return r;
        }

    //
    // SAVE
    //
    public:

        template <typename T>
        bool saveData( const char *dataKey, T &data ) {
            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, false ) )
                    break;
                if ( p.putBytes( dataKey, &data, sizeof(data) ) != sizeof(data) )
                    break;
                r = true;
                break;
            }
            p.end();
            return r;
        }

        template <typename T>
        bool saveData( const char *validityKey, const char *dataKey, T &data ) {
            // https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/
            // https://github.com/espressif/arduino-esp32/issues/2497
            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, false ) )
                    break;
                if ( p.putBytes( dataKey, &data, sizeof(data) ) != sizeof(data) )
                    break;
                if ( p.putUChar( validityKey, VALID_FLAG ) != 1 )
                    break;
                r = true;
                break;
            }
            p.end();
            return r;
        }

    //
    // ARRAY
    //

        template <typename T, size_t S>
        bool readArray( const char *dataKey, T (&data)[S] ) {
            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, true ) )
                    break;
                if ( !p.isKey( dataKey ) )
                    break;                    
                if ( p.getBytes( dataKey, &data, sizeof(data) ) != sizeof(data) )
                    break;
                r = true;
                break;
            }
            p.end();
            return r;
        }

        template <typename T, size_t S>
        result readArray( const char *validityKey, const char *dataKey, T (&data)[S] ) {
            Preferences p;
            result r = result::error;
            while( true ) {
                if ( !p.begin( PREF_FILE, true ) )
                    break;
                if ( !p.isKey( validityKey ) )
                    break;
                if ( p.getUChar( validityKey, INVALID_FLAG ) != VALID_FLAG ) {
                    r = result::invalid;
                    break;
                }
                if ( !p.isKey( dataKey ) )
                    break;
                if ( p.getBytes( dataKey, &data, sizeof(data) ) != sizeof(data) )
                    break;
                r = result::okay;
                break;
            }
            p.end();
            return r;
        }

        template <typename T, size_t S>
        bool saveArray( const char *dataKey, T (&data)[S] ) {
            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, false ) )
                    break;
                if ( p.putBytes( dataKey, &data, sizeof(data) ) != sizeof(data) )
                    break;
                r = true;
                break;
            }
            p.end();
            return r;
        }

        template <typename T, size_t S>
        bool saveArray( const char *validityKey, const char *dataKey, T (&data)[S] ) {
            // https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/
            // https://github.com/espressif/arduino-esp32/issues/2497
            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, false ) )
                    break;
                if ( p.putBytes( dataKey, &data, sizeof(data) ) != sizeof(data) )
                    break;
                if ( p.putUChar( validityKey, VALID_FLAG ) != 1 )
                    break;
                r = true;
                break;
            }
            p.end();
            return r;
        }

};

}

// const char *Storage::PREF_FILE    = "DATA";
// uint8_t     Storage::VALID_FLAG   = 0x23;
// uint8_t     Storage::INVALID_FLAG = 0x34;
