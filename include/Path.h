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
#include <assert.h>
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
    
    class CodeCreater;
    
    extern CodeCreater paths_pool;
    
    class Code
    {
    public:
        CodeType t;
        Code()
        {}
        Code(Code const&)
        {}
        virtual string to_string(unsigned int tab_num = 0) const
        {}
        ~Code()
        {}
    };
    
    class Procedure : public Code
    {
    public:
        Variable left;
        const SgExpression *right;
        
        Procedure(const Variable& v, const SgExpression* assign_expr) :
            left(v), right(assign_expr)
        {
            Code::t = procedure;
        }
        
        string to_string(unsigned int tab_num = 0) const;
        virtual ~Procedure()
        {}
    };
    
    bool belongs_to_codeblock(SgStatement *s);
    
    class CodeBlock : public Code
    {
    public:
        CodeCreater* _in_pool;
        vector<SgStatement *> L;
        
        CodeBlock()
        {
            Code::t = codeblock;
        }
        
        inline void add_statement(SgStatement *s)
        {
            L.push_back(s);
        }
        
        string to_string(unsigned int tab_num = 0) const;
        
        virtual ~CodeBlock()
        {}
    };
    
    class CodeCreater
    {
    private:
        size_t p_i;
        size_t cb_i;
        size_t l_i;
    
        vector<Procedure*> p_list;
        vector<CodeBlock*> cb_list;
    public:
        CodeCreater() : p_i(0), cb_i(0), l_i(0)
        {}
        
        inline Procedure* create_procedure(const Variable & v, const SgExpression *e)
        {
            p_list.push_back(new Procedure(v, e));
            p_i++;
            return p_list[p_i - 1];
        }
        
        inline CodeBlock* create_codeblock()
        {
            cb_list.push_back(new CodeBlock());
            cb_list[cb_i]->_in_pool = this;
            cb_i++;
            return cb_list[cb_i - 1];
        }
        
        ~CodeCreater()
        {
            for(auto &i : p_list)
            {
                delete i;
                i = nullptr;
            }
            for(auto &i : cb_list)
            {
                delete i;
                i = nullptr;
            }
        }
        
    };
    
    class Path
    {
    public:
        CodeCreater* pool;
        string path_id;
        ConstraintList constraint_list;
        vector<Code*> codes;
        bool is_return;
        SgExpression *ret_expr;
        VariableTable var_tbl;
        
        Path(CodeCreater* _pool = &paths_pool) : pool(_pool), is_return(false), ret_expr(nullptr)
        {}
        
        inline void add_procedure(const Variable & v, const SgExpression *e)
        {
            codes.push_back(pool->create_procedure(v, e));
        }
        
        inline void add_codeblock(CodeBlock* cb)
        {
            if(cb->L.empty()) return;
            assert(pool == cb->_in_pool);
            codes.push_back(cb);
        }
        
        string to_string(unsigned int tab_num = 0) const;
        
        ~Path();
    };
    
}
#endif //CODE_ANALYSE_PATH_H
