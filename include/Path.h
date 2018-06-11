//
// Created by whj on 6/11/18.
//

#ifndef CODE_ANALYSE_PATH_H
#define CODE_ANALYSE_PATH_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include "rose.h"
#include "Constraint.h"
#include "Variable.h"

namespace le
{
    
    using namespace std;
    
    extern const string TAB;
    
    enum CodeType
    {
        procedure,
        loop,
        codeblock,
    };
    
    class Code
    {
    public:
        CodeType t;
        Code()
        {}
        Code(Code const&)
        {}
        virtual string to_string(unsigned int tab_num = 0)
        {}
        ~Code()
        {}
    };
    
    class Procedure : Code
    {
    public:
        Variable left;
        SgExpression *right;
        
        Procedure(const Variable& v, SgExpression* assign_expr) :
            left(v), right(assign_expr)
        {
            Code::t = procedure;
        }
        
        string to_string(unsigned int tab_num = 0);
        virtual ~Procedure()
        {}
    };
    
    bool belongs_to_codeblock(SgStatement *s);
    
    class CodeBlock : Code
    {
    public:
        string block_id;
        vector<SgStatement *> L;
        
        CodeBlock()
        {
            Code::t = codeblock;
        }
        
        inline void add_statement(SgStatement *s)
        {
            L.push_back(s);
        }
        
        string to_string(unsigned int tab_num = 0);
        
        virtual ~CodeBlock()
        {}
    };
    
    class Path
    {
    public:
        string path_id;
        ConstraintList constraint_list;
        vector<Procedure> procedures;
        bool is_return;
        SgExpression *ret_expr;
        VariableTable var_tbl;
        
        void add_procedure(Variable v, SgExpression *e);
        
        string to_string(unsigned int tab_num = 0);
    };
    
}
#endif //CODE_ANALYSE_PATH_H
