#pragma once

#include "defs.hpp"

#if DEBUG == 1
#define debugBegin(x) Serial.begin(x)
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debugBegin(x)
#define debug(x)
#define debugln(x)
#endif