// combine multiple key press to a single value
//
// ex. 4 arrow keys
//     return diagonals if multiple keys are pressed
//
// example: U,D,L,R - up,down,left,right
//      InputCombiner c;
//      c.addKeyCombination("UL","W");
//      c.addKeyCombination("UR","X");
//      c.addKeyCombination("DL","Y");
//      c.addKeyCombination("DR","Z");
//      ...
//      auto keyList = getFromInput();
//      auto key = c.combineKeys(keyList);
//      switch( key ) {
//      case 'U': ... up        if keyList="U"
//      case 'W': ... up/left,  if keyList="UL" or "LU"
//      case 'X': ... up/right, if keyList="UR" or "RU" 
//      } 

#pragma once

#include <stdint.h>
#include <string.h>

#include <Utility/spStr.h>
#include <Utility/spVector.h>

namespace StarterPack {

template<typename T>
class InputCombiner {

    private:

        typedef char*    IN;
        typedef T        OUT;
        static constexpr OUT INACTIVE_KEY = 0;

        // typedef uint8_t KEY;
        // static constexpr KEY INACTIVE_KEY = 0;

    //
    // SETTINGS
    //
    private:

        // if 2 or more keys are presssed at the same time
        //    return another key instead
        // ex. [up] and [left] --> [diagUL]
        struct keyCombination {
            const char *combination;
            OUT result;
        };
        StarterPack::spVector<keyCombination> keyCombinationList;

    public:

        void clearKeyCombinations() {
            keyCombinationList.deletePayload = true;
            keyCombinationList.clear();
        }

        void addKeyCombination( const char *combination, OUT result ) {
            auto e = new keyCombination;
            e->combination = combination;
            e->result = result;
            // possible to sort first to speed up comparison
            //    but need to allocate memory since const, so nevermind
            // StarterPack::sortCharsInString(e->combination);
            keyCombinationList.insert(e);
        }

        void addKeyCombination( const char key1, const char key2, OUT result ) {
            auto keys = new char[3];
            // char *keys = malloc(3);
            // if ( keys == nullptr )
            //     Serial.println("xx");
            keys[0] = key1;
            keys[1] = key2;
            keys[2] = 0;
            addKeyCombination(keys, result);
        }

    //
    // ACTION
    //
    public:

        OUT actionCombineKeys(const IN keysPressed) {

            // combine multiple keys into single key

            if ( keysPressed == nullptr )
                return INACTIVE_KEY;

            switch( strlen( keysPressed ) ) {
            case 0:
                return INACTIVE_KEY;
            case 1:
                return keysPressed[0];
            default:
                keyCombination *e = keyCombinationList.getFirst();
                while ( e != nullptr ) {
                    // check if same keys are pressed
                    if (StarterPack::Str::matchUnordered(keysPressed,e->combination))
                        return e->result;
                    e = keyCombinationList.getNext();
                }
                return INACTIVE_KEY;
            }

        }

};

}
