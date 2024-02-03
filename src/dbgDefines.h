
// #define DEBUG_DIAGNOSE

#if defined( DEBUG_DIAGNOSE )
    #if !defined(DBG_SPC)
        #define DBG_SPC     "   "
    #endif
    #define _SPC()          Serial.print(DBG_SPC);
    #define _DBG(x)         Serial.print(DBG_SPC); Serial.print(x)
    #define  DBG(x)                                Serial.print(x)
    #define _DBG_(x)        Serial.print(DBG_SPC); Serial.println(x)
    #define  DBG_(x)                               Serial.println(x)
    #define _DBG_HEX(x)     Serial.print(DBG_SPC); Serial.print(x,16)
    #define  DBG_HEX(x)                            Serial.print(x,16)
    #define  DBG_DUMP(x)    x
#else
    #define _SPC()
    #define _DBG(x)
    #define  DBG(x)
    #define _DBG_(x)
    #define  DBG_(x)
    #define _DBG_HEX(x)
    #define  DBG_HEX(x)
    #define  DBG_DUMP(x)
#endif
