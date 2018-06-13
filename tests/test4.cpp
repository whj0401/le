//
// Created by whj on 6/13/18.
//

#include "iRRAM.h"

using namespace iRRAM;

REAL f(REAL a, REAL b)
{
    REAL sum = 0;
    for(REAL i = 0; i < a; i+=REAL(1))
    {
        for(REAL j = 0; j < b; j+=REAL(1))
        {
            sum = sum + i * j;
        }
    }
    return sum;
}