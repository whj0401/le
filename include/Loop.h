//
// Created by whj on 6/11/18.
//

#ifndef CODE_ANALYSE_LOOP_H
#define CODE_ANALYSE_LOOP_H

#include "Path.h"

namespace le
{
    enum LoopType
    {
        FOR,
        WHILE,
        DOWHILE
    };
    
    class Function;
    
    class Loop : public Code
    {
    private:
        void handle_statement(SgStatement* stmt, bool is_initial);
        
        void handle_expression(SgExpression* expr, bool is_initial);
        
        void handle_block_statement(SgBasicBlock* block, bool is_initial);
        
        void handle_for_statement(SgForStatement* for_stmt);
        
        void handle_while_statement(SgWhileStmt* while_stmt);
        
        void handle_dowhile_statement(SgDoWhileStmt* dowhile_stmt);
        
        void handle_if_statement(SgIfStmt* if_stmt);
        
        void handle_switch_statement(SgSwitchStatement* switch_stmt);
        
        void handle_return_statement(SgReturnStmt* return_stmt);
        
        void handle_continue_statement(SgContinueStmt* continue_stmt);
        
        void handle_break_statement(SgBreakStmt* break_stmt);
        
        void handle_var_declaration(SgVariableDeclaration* decl, bool is_initial);
        
        void handle_expr_statement(SgExprStatement* expr_s, bool is_initial);
        
        void set_init_loop_path(const SgExpression* condition);
        
        void set_init_loop_var_tbl();
        
        void init_for_statement(const SgForStatement* for_stmt);
        
        void init_while_statement(const SgWhileStmt* while_stmt);
        
        void init_dowhile_statement(const SgDoWhileStmt* dowhile_stmt);
    public:
        string loop_id;
        Function* func_ptr;
        Loop * father_loop;
        LoopType loop_type;
        CodeCreater* _in_pool;
        const SgStatement* loop_stmt;
        VariableTable var_tbl;
        Path initialize;
        vector<Path> path_list;
        
        Loop(const SgStatement* _loop_stmt, Function* _func_ptr, Loop* _father_loop, CodeCreater* _pool = &paths_pool);
        
        void add_variable(const Variable &v, bool is_initial);
        
        void add_procedure(const string& ref_name, const SgExpression* expr, bool is_initial);
    
        void add_constraint(SgExpression *expr, bool is_not);
    
        void add_loop(const Loop& loop);
    
        void clear_path_with_continue_break_return();
    
        VariableTable collect_all_var_tbl();
    
        void merge(const Loop &l);
        
        virtual string to_string(unsigned int tab_num = 0) const;
    
        virtual string to_code(unsigned int tab_num = 0) const;
    };
}

#endif //CODE_ANALYSE_LOOP_H
