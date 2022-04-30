#ifndef __INCLUDED_KEY_DECODER__
#define __INCLUDED_KEY_DECODER__

#include <Arduino.h>
#include "defs.hpp"

class CKeyDecoder{
public:
    // We want a struct that we can overlay on the active keys, to make life easier when checking which ones are being pressed
    //
    struct T_KEY_STATE_STRUCT
    {
        bool A:1;
        bool B:1;
        bool C:1;
        bool D:1;
        bool E:1;
        bool F:1;
        bool G:1;
        bool H:1;
        bool I:1;
        bool J:1;
        bool K:1;
        bool L:1;
        bool M:1;
        bool N:1;
        bool O:1;
        bool P:1;
        bool JoySW:1;
        uint16_t  extra:15;
    };
    
    union KeyStateUnion
    {
        uint32_t Data;
        T_KEY_STATE_STRUCT States;
    };
    
    static T_KEY_STATE_STRUCT GetButtonStates(uint32_t KeyStates);
    



};

#endif