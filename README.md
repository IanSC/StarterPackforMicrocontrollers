# StarterPack for Microcontrollers

Library containing all the basic elements to get into MCU development.

Includes:
- Digital Input

    Built in debouncing and button handling.

    ```
    DigitalIO dIO1( 2, DigitalIO::ACTIVE_LOW | DigitalIO::WITH_PULLUP );
    dIO1.getKeyDown();
    ```

- Analog Input

    Supports analog reading or as resistor network button.

    Assignable mapping function.
    
    Integrated filtering for smooth readings using ResposiveAnalogRead.
    ```
    dd
    ```


- digital/analog input, debouncing, I2C, LCD display (wired and i2c), LCD menu. Debouncing of digital or analog buttons, supports KeyUp, KeyDown and Repeating. Buffered LCD display for blazing fast updates. Simplify I2C with recovery from disconnected I2c wires/power.