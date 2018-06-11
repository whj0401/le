#include "iRRAM.h"
#include <iostream>

iRRAM::REAL f(iRRAM::REAL x)
{
    return 2 * x;
}

//iRRAM::REAL test1(iRRAM::REAL a, int b)
//{
//    for(int i = 0; i < b; ++i)
//    {
//        if(a < b / 2)
//            a = f(a);
//        else if(a > b / 2)
//            a /= 1.1;
//        else
//            a *= a;
//    }
//    iRRAM::cout << a << std::endl;
//    return a;
//}

iRRAM::REAL test2(iRRAM::REAL a, iRRAM::REAL b)
{
    bool c1 = false;
    iRRAM::REAL c2, c3 = 1;
    c1 = (a < b);
    c2 = a + b;
    c3 = c2;
    c3 = 2 + a;
};