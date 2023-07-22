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

#include <inttypes.h>
#include <string.h>

namespace StarterPack {

class InputMapper {

    private:

        typedef uint8_t KEY;
        static constexpr KEY INACTIVE_KEY = 0;

    //
    // SETTINGS
    //
    private:

        const char *keyMap = nullptr;

    public:

        inline void assignKeymap( const char *keyMap ) {
            this->keyMap = keyMap;
        }

    //
    // ACTION
    //
    public:
    
        KEY actionMapKey( const uint8_t scanCode ) {
            // 1-based
            // eg. scanCode=1, will give keyMap[0]
            //    scanCode     : 123456789...
            //    keyMap index : 012345678...
            if ( scanCode == 0 )
                return INACTIVE_KEY;
            if ( keyMap == nullptr )
                return scanCode;
            if ( strlen( keyMap ) < scanCode )
                return scanCode;
            return keyMap[ scanCode-1 ];
        }

};

}
