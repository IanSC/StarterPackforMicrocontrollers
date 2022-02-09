#pragma once

// or just add specific headers to your project
// in any case the compiler will remove unused functions
// so uploaded code won't get any bigger

#include <AnalogIO.h>
#include <DigitalIO.h>
#include <MatrixKeypad.h>
#include <Debouncer.h>

#include <UserInput.h>
#include <UserInput_Analog.h>
#include <UserInput_Digital.h>
//#include <UserInput_MatrixKeypad.h>

#include <MatrixKeypad.h>
#include <MatrixKeypadMulti.h>

#include <LCD_wired.h>
#include <LCDBuffered_wired.h>
#include <LCD_i2c.h>
#include <LCDBuffered_i2c.h>

#include <LCDUtility.h>
#include <LCDMenu.h>

#include <Throttle.h>
#include <ChangeObserver.h>

#include <spSemaphore.h>
#include <spUtility.h>

namespace StarterPack {
}
