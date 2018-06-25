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
        SgExpression *right;
    
        Procedure(const Variable &v, SgExpression *assign_expr);
        
        string to_string(unsigned int tab_num = 0) const;
    
        string to_code(unsigned int tab_num = 0) const;
        virtual ~Procedure()
        {
            delete right;
        }
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
    
        Procedure *create_procedure(const Variable &v, SgExpression *e);
        
        CodeBlock* create_codeblock();
    
        Loop *
        create_loop(const SgStatement *stmt, Function *_func_ptr, Loop *_father_loop, int _dowhile_loop_count = 0);
        
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
    
    class Path_block
    {
    public:
        bool is_procedures;
        map<string, string> value_map;
        bool is_loop;
        Loop *loop_ptr;
        bool is_codeblock;
        CodeBlock *codeblock_ptr;
        
        Path_block() : is_procedures(false), is_loop(false), is_codeblock(false), loop_ptr(nullptr),
                       codeblock_ptr(nullptr)
        {}
        
        string to_string(unsigned int tab_num) const;
    };
    
    class Path
    {
    private:
        map<string, string> current_value_map;
        vector<Path_block> blocks;
    
        void modify_cur_value_map(string ref_name, const SgExpression *expr);
    
        void modify_cur_block();
        
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
    
        void add_procedure(const Variable &v, SgExpression *e);
    
        void add_procedure(const std::string &ref_name, SgExpression *expr);
        
        inline void add_codeblock(CodeBlock* cb)
        {
            if(cb->L.empty()) return;
            assert(pool == cb->_in_pool);
            codes.push_back(cb);
        }
        
        void add_loop(const Loop* loop);
    
        void add_constraint(const SgExpression *expr, bool is_not);
        
        string to_string(unsigned int tab_num = 0) const;
    
        string to_code(unsigned int tab_num = 0) const;
        string to_string_as_initializer() const;
    
        void to_klee_code_functions(const VariableTable &input_parameters);
        
        ~Path();
    };
}

#endif //CODE_ANALYSE_PATH_H
