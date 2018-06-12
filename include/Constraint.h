//
// Created by whj on 6/11/18.
//

#ifndef CODE_ANALYSE_CONSTRAINT_H
#define CODE_ANALYSE_CONSTRAINT_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "rose.h"

namespace le
{
    
    using namespace std;
    
    class Constraint
    {
    public:
        SgExpression *e;
        
        Constraint(SgExpression *_e) :
                e(_e)
        {}
        
        inline string to_string() const
        {
            e->unparseToString();
        }
    };
    
    class ConstraintList
    {
    public:
        vector<Constraint> L;
        
        void add_constraint(const Constraint &c)
        {
            L.push_back(c);
        }
        
        string to_string() const
        {
            stringstream ss;
            for (const auto &i : L)
            {
                ss << "(" + i.to_string() + ")&&";
            }
            string res = ss.str();
            return res.substr(0, res.length() - 2);
        }
    };
    
}
#endif //CODE_ANALYSE_CONSTRAINT_H
