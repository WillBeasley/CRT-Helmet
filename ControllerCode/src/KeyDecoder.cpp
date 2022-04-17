#include "KeyDecoder.hpp"



CKeyDecoder::T_KEY_STATE_STRUCT CKeyDecoder::GetButtonStates(uint32_t KeyStates){

    KeyStateUnion un;
    un.Data = KeyStates;
    return un.States;

}