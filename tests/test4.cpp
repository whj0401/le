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
            if (i < j)
            {
                sum = sum + i * j;
            }
            else if (i > j)
            {
                sum = sum - i * j;
            }
            else
            {
                continue;
            }
        }
    }
    return sum;
}