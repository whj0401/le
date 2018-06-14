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
#include "common.h"

namespace le
{
    
    using namespace std;
    
    enum CodeType
    {
        procedure,
        loop,
        codeblock,
    };
    
    class CodeCreater;
    class Path;
    class Function;
    
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
    
        virtual string to_code(unsigned int tab_num = 0) const
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
    
        string to_code(unsigned int tab_num = 0) const;
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
    
        string to_code(unsigned int tab_num = 0) const;
        virtual ~CodeBlock()
        {}
    };
    
    class Loop;
    
    class CodeCreater
    {
    private:
        size_t p_i;
        size_t cb_i;
        size_t l_i;
    
        vector<Procedure*> p_list;
        vector<CodeBlock*> cb_list;
        vector<Loop*> l_list;
    public:
        CodeCreater() : p_i(0), cb_i(0), l_i(0)
        {}
        
        Procedure* create_procedure(const Variable & v, const SgExpression *e);
        
        CodeBlock* create_codeblock();
        
        Loop* create_loop(const SgStatement *stmt, Function* _func_ptr, Loop* _father_loop);
        
        ~CodeCreater();
        
    };
    
    extern size_t path_count;
    
    class procedure_block
    {
    public:
        int block_id;
        size_t begin_codes_idx;
        size_t end_codes_idx; // not include
        bool finished;
        vector<string> left_real_value_name_list;
        
        void add_klee_out_code(stringstream &ss, unsigned int tab_num) const;
    };
    
    class Path
    {
    private:
        void generate_procedure_block_list();
    public:
        CodeCreater* pool;
        string path_id;
        ConstraintList constraint_list;
        vector<const Code*> codes;
        bool is_return;
        SgExpression *ret_expr;
        VariableTable var_tbl;
        bool can_break;
        bool can_continue;
        vector<procedure_block> procedure_block_list;
        
        Path(CodeCreater* _pool = &paths_pool) :
                pool(_pool), is_return(false), ret_expr(nullptr), can_break(false), can_continue(false)
        {
            path_id = string("__path__") + std::to_string(path_count);
            path_count++;
        }
        
        inline bool can_add_code()
        {
            return (!is_return && !can_break && !can_continue);
        }
        
        inline void add_procedure(const Variable & v, const SgExpression *e)
        {
            if(can_add_code())
                codes.push_back(pool->create_procedure(v, e));
        }
        
        inline void add_codeblock(CodeBlock* cb)
        {
            if(cb->L.empty()) return;
            assert(pool == cb->_in_pool);
            codes.push_back(cb);
        }
        
        void add_loop(const Loop* loop);
        
        inline void add_constraint(const Constraint& c)
        {
            constraint_list.add_constraint(c);
        }
        
        string to_string(unsigned int tab_num = 0) const;
    
        string to_code(unsigned int tab_num = 0) const;
        string to_string_as_initializer() const;
    
        string to_klee_code_functions(const VariableTable &input_parameters);
        
        ~Path();
    };
}

#endif //CODE_ANALYSE_PATH_H
