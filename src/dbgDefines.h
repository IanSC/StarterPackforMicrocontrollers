
// #define DEBUG_DIAGNOSE

#if defined( DEBUG_DIAGNOSE )
    #if !defined(DBG_SPC)
        #define DBG_SPC     "   "
    #endif

    #define _SPC()              Serial.print(DBG_SPC)
    
    // DBG
    #define  DBG(x)                     Serial.print  (x)
    #define _DBG(x)             _SPC(); Serial.print  (x)
    #define  DBG_(x)                    Serial.println(x)
    #define _DBG_(x)            _SPC(); Serial.println(x)

    // HEX
    #define  DBG_HEX(x)                 Serial.print  (x,16)
    #define _DBG_HEX(x)         _SPC(); Serial.print  (x,16)
    #define  DBG_HEX_(x)                Serial.println(x,16)
    #define _DBG_HEX_(x)        _SPC(); Serial.println(x,16)

    // DUMP
    #define  DBG_DUMP(x)        x

    // IF
    #define  DBG_IF(c,x)        if (c) {         Serial.print  (x); }
    #define _DBG_IF(c,x)        if (c) { _SPC(); Serial.print  (x); }
    #define  DBG_IF_(c,x)       if (c) {         Serial.println(x); }
    #define _DBG_IF_(c,x)       if (c) { _SPC(); Serial.println(x); }

    // IF HEX
    #define  DBG_HEX_IF(c,x)    if (c) {         Serial.print  (x,16); }
    #define _DBG_HEX_IF(c,x)    if (c) { _SPC(); Serial.print  (x,16); }
    #define  DBG_HEX_IF_(c,x)   if (c) {         Serial.println(x,16); }
    #define _DBG_HEX_IF_(c,x)   if (c) { _SPC(); Serial.println(x,16); }

    // IF DUMP
    #define  DBG_DUMP_IF(c,x)   if (c) { x }

#else
    #define _SPC()

    #define  DBG(x)
    #define _DBG(x)
    #define  DBG_(x)
    #define _DBG_(x)
    
    #define  DBG_HEX(x)
    #define _DBG_HEX(x)
    #define  DBG_HEX_(x)
    #define _DBG_HEX_(x)

    #define  DBG_DUMP(x)

    #define  DBG_IF(c,x)
    #define _DBG_IF(c,x)
    #define  DBG_IF_(c,x)
    #define _DBG_IF_(c,x)

    #define  DBG_HEX_IF(c,x)
    #define _DBG_HEX_IF(c,x)
    #define  DBG_HEX_IF_(c,x)
    #define _DBG_HEX_IF_(c,x)

    #define  DBG_DUMP_IF(c,x)
#endif
