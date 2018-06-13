//
// Created by whj on 6/11/18.
//

#ifndef CODE_ANALYSE_VARIABLE_H
#define CODE_ANALYSE_VARIABLE_H

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include "rose.h"

namespace le{

    using namespace std;
    
    extern const int T_REAL;
    
    extern const string REAL_MANGLED;
    
    class Variable
    {
    public:
        string var_name;
        SgType * type_node;
        SgExpression * initExpr;
        int type_T;
        
        Variable(const string& _var_name, SgType* _type_node, SgExpression* _initExpr = nullptr) :
                var_name(_var_name), type_node(_type_node), initExpr(_initExpr)
        {
            type_T = type_node->variantT();
            if(type_T == T_CLASS && type_node->get_mangled() == REAL_MANGLED)
            {
                type_T = T_REAL;
            }
        }
        
        inline bool is_real_type()
        {
            return type_T == T_REAL;
        }
        
        inline void add_to_stringstream(stringstream& ss) const
        {
            ss << "\"" << var_name << "\": " << type_node->get_mangled() << "";
        }
    };
    
    class VariableTable
    {
    public:
        map<string, Variable> T;
        
        VariableTable()
        {}
        
        inline bool has_variable(const string & name)
        {
            return T.find(name) != T.end();
        }
    
        inline void add_variable(const Variable& v)
        {
            if(has_variable(v.var_name)) return;
            T.insert(pair<string, Variable>(v.var_name, v));
        }
        
        inline const Variable& find(const string & name)
        {
            return T.find(name)->second;
        }
        
        string to_string() const;
    };
    
}
#endif //CODE_ANALYSE_VARIABLE_H
