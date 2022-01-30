// note: unused compoents will automatically be
// removed from final uploaded code
// so importing the whole package won't make
// your code bigger

//
// OPTION 1-A - import namespace, all components
//
#include <StarterPack.h>
using namespace StarterPack;
DigitalIO dIO1 = DigitalIO( 10 );

//
// OPTION 1-B - import namespace, specific component
//
#include <DigitalIO.h>
using namespace StarterPack;
DigitalIO dIO2 = DigitalIO( 10 );

//
// OPTION 2-A - alias, all components
//
#include <StarterPack.h>
namespace sp = StarterPack;
sp::DigitalIO dIO3 = sp::DigitalIO( 10 );

//
// OPTION 2-B - alias, specific components
//
#include <DigitalIO.h>
namespace sp = StarterPack;
sp::DigitalIO dIO4 = sp::DigitalIO( 10 );

//
// OPTION 3-A - fully qualified, all components
//
#include <StarterPack.h>
StarterPack::DigitalIO dIO5 = StarterPack::DigitalIO( 10 );

//
// OPTION 3-B - fully qualified, specific component
//
#include <DigitalIO.h>
StarterPack::DigitalIO dIO6 = StarterPack::DigitalIO( 10 );

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.print( "Debouncer = " );
    Serial.println( sizeof( Debouncer ) );
    Serial.print( "Debouncer Settings = " );
    Serial.println( sizeof( Debouncer::Settings ) );
    Serial.print( "ChangeObserver = " );
    Serial.println( sizeof( ChangeObserver ) );
    Serial.print( "DigitalIO = " );
    Serial.println( sizeof( DigitalIO ) );
    Serial.print( "AnalogIO = " );
    Serial.println( sizeof( AnalogIO ) );
    Serial.print( "MatrixKeypad = " );
    Serial.println( sizeof( MatrixKeypad ) );
    
    while(1);
}

void loop() {
}
