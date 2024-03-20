// map key press to logical values
//
// ex. matrix keypad with 16 keys will give 1 to 16
//     map it as '1','2','3','A',
//               '4','5','6','B',
//               '7','8','9','C',
//               '*','0','#','D'
//
//     +---+---+---+---+
//     | 1 | 2 | 3 | A |
//     +---+---+---+---+
//     | 4 | 5 | 6 | B |
//     +---+---+---+---+
//     | 7 | 8 | 9 | C |
//     +---+---+---+---+
//     | * | 0 | # | D |
//     +---+---+---+---+
//         ||||||||
//         rrrrcccc
//         12341234
//
//  example:
//      InputMapper map;
//      map.assignKeymap("123A456B789C*0#D");
//      ...
//      auto key = map.mapKey(number);

#pragma once

#include <stdint.h>
#include <string.h>

#include <InputHelper/InputFilterInterface.h>

namespace StarterPack {

template<typename DATA_TYPE, typename OUT_DATA_TYPE>
class InputKeyMapper : public InputFilterInterface<OUT_DATA_TYPE> {

    private:

        // typedef DATA_TYPE      SCANCODE;
        // typedef OUT_DATA_TYPE  KEY;
        static constexpr OUT_DATA_TYPE INACTIVE_KEY = 0;

    //
    // FILTER BASE
    //
    public:
        inline DATA_TYPE actionApplyFilter( DATA_TYPE value ) override {
            return actionMapKey(value);
        }

    //
    // SETTINGS
    //
    private:

        const char *keyMap = nullptr;

    public:

        inline void assignKeymap( const char *keyMap ) {
            this->keyMap = keyMap;
        }
        const char * getKeymap() {
            return keyMap;
        }

    //
    // ACTION
    //
    public:
    
        OUT_DATA_TYPE actionMapKey( const DATA_TYPE scanCode ) {
            // 1-based
            // eg. scanCode=1, will give keyMap[0]
            //    scanCode     : 123456789...
            //    keyMap index : 012345678...
            if ( scanCode == 0 )
                return INACTIVE_KEY;
            if ( keyMap == nullptr )
                return scanCode;

            // keyMap might not be NULL terminated
            //    so don't bother checking length
            // eg. static constexpr char keymap[] = {
            //        '1', '2', '3', 
            //        '4', '5', '6',
            //        '7', '8', '9',
            //         ui::cESCAPE, '0', ui::cENTER, 0 }; <--- forgot NULL
            //     keypad.assignKeymap(keymap);
            // if ( strlen( keyMap ) < scanCode )
            //     return scanCode;

            return keyMap[ scanCode-1 ];
        }

};

}
