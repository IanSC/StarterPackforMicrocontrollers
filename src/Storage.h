#pragma once
#include <Preferences.h>

#include <UserInterface.h>
#include <LCDUtility.h>

class Storage {

        const char *PREF_FILE;    // = "DATA";
        uint8_t     VALID_FLAG;   // = 0x23;
        uint8_t     INVALID_FLAG; // = 0x34;

    public:

        Storage( const char *PREF_FILE = "DATA", uint8_t VALID_FLAG = 0x23, uint8_t INVALID_FLAG = 0x34 ) {
            this->PREF_FILE = PREF_FILE;
            this->VALID_FLAG = VALID_FLAG;
            this->INVALID_FLAG = INVALID_FLAG;
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
            uint8_t input = StarterPack::LCDUtility::chooseNoYes( 0, "Erase Data?", 1 );

            if ( input == 2 ) {
                if ( wipeCore() ) {
                    ui::dialogBox( "RESET", "data erased" );
                    return wipeResult::wOkay;
                } else {
                    ui::showError( ERR_SOURCE, ERR_STORAGE_WIPE, "error erasing data" );
                    return wipeResult::wError;
                }
            } else {
                ui::LCD->printStrAtRow( 3, "CANCELLED" );
                ui::LCD->displayAll();
                ui::waitForAnyKey();
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
                if ( p.getUChar( validityKey, INVALID_FLAG ) != VALID_FLAG ) {
                    r = result::invalid;
                    break;
                }
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
                if ( p.getUChar( validityKey, INVALID_FLAG ) != VALID_FLAG ) {
                    r = result::invalid;
                    break;
                }
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

// const char *Storage::PREF_FILE    = "DATA";
// uint8_t     Storage::VALID_FLAG   = 0x23;
// uint8_t     Storage::INVALID_FLAG = 0x34;
