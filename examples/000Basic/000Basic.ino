// include all copoennts
#include <StarterPack.h>
// note: unused compoents will automatically be
// removed in the final code by the compiler

#include <AnalogIO.h>

//
// option 1 - fully qualified
//
StarterPack::DigitalIO dIO1 = StarterPack::DigitalIO( 10 );

//
// option 2 - alias
//
namespace sp = StarterPack;
sp::DigitalIO dIO2 = sp::DigitalIO( 10 );

//
// option 3 - import all
//
using namespace StarterPack;
DigitalIO dIO3 = DigitalIO( 10 );

void setup() {
}

void loop() {
}
