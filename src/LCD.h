#pragma once

// needed only if developing new interface
// #include <LCD/LCD_HD44780.h>

// base class, needed only if developing variants
// #include <LCD/LCDBuffered.h>

// wired connection type
// seldom used so don't include by default
// #include <LCD/LCD_wired.h>
// #include <LCD/LCDBuffered_wired.h>

// i2c LCD display, non-buffered and buffered
#include <LCD/LCD_i2c.h>
#include <LCD/LCDBuffered_i2c.h>

// custom character support
#include <LCDSupport/LCDBitmap.h>
