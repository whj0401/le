//
// Created by whj on 6/11/18.
//

#ifndef CODE_ANALYSE_CONSTRAINT_H
#define CODE_ANALYSE_CONSTRAINT_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
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
        bool str_represent;
        string expr_str;
        
        Constraint(const SgExpression *_e, bool _is_true = true) :
                is_true(_is_true), str_represent(false)
        {
            e = _e;
        }
    
        Constraint(const SgExpression *_e, const map<string, string> &value_map, bool _is_true = true) :
                is_true(_is_true), str_represent(true)
        {
            e = _e;
            expr_str = expression_to_string_with_value_map(e, value_map);
            if (!is_true)
            {
                expr_str = string("!(") + expr_str + ")";
            }
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
            if (str_represent)
            {
                return expr_str;
            }
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
            if (L.empty()) return "(true)";
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
