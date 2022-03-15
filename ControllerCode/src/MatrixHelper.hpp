#ifndef __INCLUDED_MATRIX_HELPER__
#define __INCLUDED_MATRIX_HELPER__

#include "defs.hpp"

class CMatrixHelper{

private:
    // This is the mappings for which LED offset links to which LEDs
    const static int XYMappings[];

public:

    static int XY(int x, int y);
};





#endif