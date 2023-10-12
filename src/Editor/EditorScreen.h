#pragma once

class EditorScreen {

public:

    // virtual void init();

    virtual void displayBegin();
    virtual void displayEnd();

    virtual void cursorOn();
    virtual void cursorOff();

    virtual void cursorBlinkOn();
    virtual void cursorBlinkOff();

    virtual void displayLeftObscured ( uint8_t col, uint8_t row );
    virtual void displayRightObscured( uint8_t col, uint8_t row );

    virtual void setCursor( uint8_t col, uint8_t row );

    virtual void printStrN( const char *str, uint8_t N, bool clearSpace = false );
    virtual void writeAt( uint8_t col, uint8_t row, uint8_t byte );

    virtual void refresh();
    virtual void refreshPartial();

};
