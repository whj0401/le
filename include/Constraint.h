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
#include "common.h"

namespace le
{
    
    using namespace std;
    
    class Constraint
    {
    public:
        bool is_true;
        const SgExpression *e;
        
        Constraint(const SgExpression *_e, bool _is_true = true) :
                is_true(_is_true)
        {
            e = _e;
        }
        
//        Constraint(const Constraint& c)
//        {
//            e = clone<SgExpression>(c.e);
//            is_true = c.is_true;
//        }
//
//        Constraint& operator= (const Constraint& c)
//        {
//            e = clone<SgExpression>(c.e);
//            is_true = c.is_true;
//            return *this;
//        }
        
        inline string to_string() const
        {
            if(is_true)
                return e->unparseToString();
            else
                return string("!(") + e->unparseToString() + ")";
        }
        
        ~Constraint()
        {}
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
