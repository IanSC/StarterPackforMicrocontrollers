// combine multiple key press to single value
//
// ex. 4 arrow keys
//     return diagonals if multiple keys are pressed
//
// example: U,D,L,R - up,down,left,right
//      InputCombiner c;
//      c.addKeyCombination("UL","1");
//      c.addKeyCombination("UR","2");
//      c.addKeyCombination("DL","3");
//      c.addKeyCombination("DR","4");
//      ...
//      auto keyList = getFromInput();
//      auto key = c.combineKeys(keyList);
//      switch( key ) {
//      case 'U': ... up
//      case '1': ... up/left
//      case '2': ... up/right
//      } 

#pragma once

#include <inttypes.h>
#include <string.h>

#include <spUtility.h>
#include <spVector.h>

namespace StarterPack {

class InputCombiner {

    private:

        typedef uint8_t KEY;
        static constexpr KEY INACTIVE_KEY = 0;

    //
    // SETTINGS
    //
    protected:

        // if 2 or more keys are presssed at the same time
        //    return another key instead
        // ex. [up] and [left] --> [diagUL]
        struct keyCombination {
            const char *combination;
            KEY result;
        };
        StarterPack::spVector<keyCombination> keyCombinationList;

    public:

        void addKeyCombination( const char *combination, char result ) {
            auto e = new keyCombination;
            e->combination = combination;
            e->result = result;
            // possible to sort first to speed up comparison
            //    but need to allocate memory since const, so nevermind
            // StarterPack::sortCharsInString(e->combination);
            keyCombinationList.insert(e);
        }

    //
    // ACTION
    //
    public:

        KEY actionCombineKeys(const char *keysPressed) {

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
                    if (StarterPack::areCharsInString(keysPressed,e->combination))
                        return e->result;
                    e = keyCombinationList.getNext();
                }
                return INACTIVE_KEY;
            }

        }

};

}
