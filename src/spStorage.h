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

// #define DEBUG_DIAGNOSE
#include <dbgDefines.h>

namespace StarterPack {

class spStorage {

        const char *PREF_FILE;    // = "DATA";
        // uint8_t     VALID_FLAG;   // = 0x23;
        // uint8_t     INVALID_FLAG; // = 0x34;
        
    public:

        spStorage( const char * PREF_FILE = "DATA" ) { //}, uint8_t VALID_FLAG = 0x23, uint8_t INVALID_FLAG = 0x34 ) {
            this->PREF_FILE = PREF_FILE;
            // this->VALID_FLAG = VALID_FLAG;
            // this->INVALID_FLAG = INVALID_FLAG;
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

        enum class wipeResult {
            okay,
            error,
            cancelled
        };

        wipeResult wipeWithPrompt( uint8_t errSource, uint8_t errCode ) {
            namespace ui = StarterPack::UserInterface;

            ui::LCD->clear();
            uint8_t input = StarterPack::spLCD::optionChooser::chooseNoYes( "Erase Data?" );
            // uint8_t input = StarterPack::LCDUtility::promptNoYes( "Erase Data?" )->prompt();

            if ( input == 2 ) {
                if ( wipeCore() ) {
                    ui::promptDialog1( "RESET", "data erased" );
                    return wipeResult::okay;
                } else {
                    ui::promptErrorDialog1( errSource, errCode, "error erasing data" );
                    return wipeResult::error;
                }
            } else {
                ui::LCD->printStrAtRow( 3, "CANCELLED" );
                // ui::LCD->displayAll();
                ui::LCD->refresh();
                ui::waitForAnyKeyPressed();
                return wipeResult::cancelled;
            }
        }

    // //
    // // VALIDITY
    // //
    // public:

    //     enum result {
    //         okay,
    //         invalid,
    //         //restored, // extra for callers
    //         error
    //     };

    //     result isValid( const char * validityKey ) {
    //         Preferences p;
    //         result r = result::error;
    //         while( true ) {
    //             if ( !p.begin( PREF_FILE, true ) )
    //                 break;
    //             if ( !p.isKey( validityKey ) )
    //                 break;
    //             if ( p.getUChar( validityKey, INVALID_FLAG ) != VALID_FLAG ) {
    //                 r = result::invalid;
    //                 break;
    //             }
    //             r = result::okay;
    //             break;
    //         }
    //         p.end();
    //         return r;
    //     }

        static constexpr uint8_t VALID_FLAG   = B10101010;
        static constexpr uint8_t INVALID_FLAG = B01010101;

        bool isValid( const char * key ) {
            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, true ) )
                    break;
                if ( !p.isKey( key ) )
                    break;
                if ( p.getUChar( key, INVALID_FLAG ) != VALID_FLAG )
                    break;
                r = true;
                break;
            }
            p.end();
            return r;
        }

        bool setValid( const char * key ) {
            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, false ) )
                    break;
                if ( p.putUChar( key, VALID_FLAG ) != 1 )
                    break;
                r = true;
                break;
            }
            p.end();
            return r;
        }

        bool setInvalid( const char * key ) {
            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, false ) )
                    break;
                if ( p.putUChar( key, INVALID_FLAG ) != 1 )
                    break;
                r = true;
                break;
            }
            p.end();
            return r;
        }

    //
    // UTILITY
    //
    public:

        bool keyExists( const char * key ) {
            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, true ) ) {
                    DBG( "keyExists: begin error " ); DBG_( key );
                    break;
                }
                r = p.isKey( key );
                break;
            }
            p.end();
            return r;
        }

        bool removeKey( const char * key ) {
            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, false ) ) break;
                r = p.remove( key );
                break;
            }
            p.end();
            return r;
        }

        size_t getFreeEntries() {
            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, false ) ) break;
                r = p.freeEntries();
                break;
            }
            p.end();
            return r;
        }

    //
    // DATA
    //
    public:

        template <typename T>
        bool readData( const char * key, T * data ) {
            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, true ) ) {
                    DBG( "storageRead: begin error " ); DBG_( key );
                    break;
                }
                if ( !p.isKey( key ) ) {
                    DBG( "storageRead: missing key " ); DBG_( key );
                    break;
                }
                if ( p.getBytes( key, data, sizeof(T) ) != sizeof(T) ) {
                    DBG( "storageRead: read error " ); DBG_( key );
                    break;
                }
                r = true;
                break;
            }
            p.end();
            return r;
        }

        template <typename T>
        bool saveData( const char * key, T * data ) {

            // https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/
            // https://github.com/espressif/arduino-esp32/issues/2497

            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, false ) ) {
                    DBG( "storageSave: begin error " ); DBG_( key );
                    break;
                }
                if ( p.putBytes( key, data, sizeof(T) ) != sizeof(T) ) {
                    DBG( "storageSave: save error " ); DBG_( key );
                    p.remove( key );
                    break;
                }
                r = true;
                break;
            }
            p.end();
            return r;
        }

    //
    // ARRAY
    //
    public:

        bool arrayExists( const char * key ) {

            char *KEY = new char[ strlen(key)+3 ];
            strcpy( KEY+2, key );
            KEY[1] = '_';

            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, true ) ) break;
                // === DATA ===
                KEY[0]='D'; if ( !p.isKey( KEY ) ) break;
                // === SIZE ===
                KEY[0]='S'; if ( !p.isKey( KEY ) ) break;
                r = true;
                break;
            }
            p.end();
            delete[] KEY;
            return r;
        }

        bool deleteArray( const char * key ) {
            char * KEY = new char[ strlen(key)+3 ];
            strcpy( KEY+2, key );
            KEY[1] = '_';

            Preferences p;
            bool r = true;
            while( true ) {
                if ( !p.begin( PREF_FILE, true ) ) {
                    r = false;
                    break;
                }
                // === DATA ===
                KEY[0]='D'; if ( !p.remove( KEY ) ) r = false;
                // === SIZE ===
                KEY[0]='S'; if ( !p.remove( KEY ) ) r = false;
                break;
            }
            p.end();
            delete[] KEY;
            return r;
        }

        template <typename T, size_t S, typename TS>
        bool readArray( const char * key, T (&data)[S], TS * usedSize ) {

            char * KEY = new char[ strlen(key)+3 ];
            strcpy( KEY+2, key );
            KEY[1] = '_';

            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, true ) ) {
                    DBG("storageReadArray: begin error "); DBG_( key );
                    break;
                }
                // === DATA ===
                KEY[0]='D';
                if ( !p.isKey( KEY ) ) {
                    DBG("storageReadArray: missing key "); DBG_(KEY);
                    break;
                }
                if ( p.getBytes( KEY, &data, sizeof(data) ) != sizeof(data) ) {
                    DBG("storageReadArray: getBytes() error "); DBG_(KEY);
                    break;
                }
                // === SIZE ===
                KEY[0]='S';                
                if ( !p.isKey( KEY ) ) {
                    DBG("storageReadArray: missing key "); DBG_(KEY);
                    break;
                }
                if ( p.getBytes( KEY, usedSize, sizeof(TS) ) != sizeof(TS) ) {
                    DBG("storageReadArray: getBytes() error "); DBG_(KEY);
                    break;
                }
                r = true;
                break;
            }
            p.end();
            delete[] KEY;
            return r;
        }

        template <typename T, size_t S, typename TS>
        bool saveArray( const char * key, T (&data)[S], TS usedSize ) {            

            char * KEY = new char[ strlen(key)+3 ];
            strcpy( KEY+2, key );
            KEY[1] = '_';

            Preferences p;
            bool r = false;
            while( true ) {
                if ( !p.begin( PREF_FILE, false ) ) {
                    DBG("storageSaveArray: begin error "); DBG_( key );
                    break;
                }
                // === DATA ===
                KEY[0]='D';
                if ( p.putBytes( KEY, &data, sizeof(data) ) != sizeof(data) ) {
                    DBG("storageSaveArray: putBytes() error "); DBG_(KEY);
                    p.remove( KEY );
                    break;
                }
                // === SIZE ===
                KEY[0]='S';
                if ( p.putBytes( KEY, &usedSize, sizeof(TS) ) != sizeof(TS) ) {
                    DBG("storageSaveArray: putBytes() error "); DBG_(KEY);
                    p.remove( KEY );
                    break;
                }
                r = true;
                break;
            }
            p.end();
            delete[] KEY;
            return r;
        }

};

}

#include <dbgDefinesOff.h>
