#pragma once

// or just add specific headers to your project
// in any case the compiler will remove unused functions
// so uploaded code won't get any bigger

#include <AnalogIO.h>

#include <DigitalIO.h>
#include <DigitalIOGroup.h>

#include <MatrixKeypad.h>
#include <MatrixKeypad1Key.h>

#include <Debouncer.h>

#include <UserInterface.h>

// #include <UserInput_Analog.h>
// #include <UserInput_Digital.h>
//#include <UserInput_MatrixKeypad.h>

#include <LCD_wired.h>
#include <LCDBuffered_wired.h>
#include <LCD_i2c.h>
#include <LCDBuffered_i2c.h>

#include <LCDUtility.h>
#include <LCDMenu.h>

#include <LCDEditorAlpha.h>
#include <LCDEditorNumeric.h>

#include <LCDBitmap.h>

#include <Throttle.h>
#include <ChangeObserver.h>

#include <spSemaphore.h>
#include <spUtility.h>

#include <blinkSOS.h>

#include <Encoder.h>
#include <Storage.h>
#include <PropertyEditor.h>

namespace StarterPack {
}
