//
// Created by whj on 6/11/18.
//

#ifndef CODE_ANALYSE_FUNCTION_H
#define CODE_ANALYSE_FUNCTION_H

#include <iostream>
#include <vector>
#include <string>
#include "Path.h"
#include "Loop.h"
#include "common.h"

namespace le
{
    
    using namespace std;
    
    bool isAtomStatement(const SgStatement *stmt);
    
    class Function
    {
    private:
        void add_input_parameter(const Variable &v);
    
        void add_input_parameterlist();
        
        void handle_expression(SgExpression* expr);
        
        void handle_statement(SgStatement* stmt);
        
        void handle_block_statement(SgBasicBlock* block);
        
        void handle_for_statement(SgForStatement* for_stmt);
        
        void handle_while_statement(SgWhileStmt* while_stmt);
        
        void handle_dowhile_statement(SgDoWhileStmt* dowhile_stmt);
        
        void handle_if_statement(SgIfStmt* if_stmt);
        
        void handle_switch_statement(SgSwitchStatement* switch_stmt);
        
        void handle_return_statement(SgReturnStmt* return_stmt);
        
        void handle_var_declaration(SgVariableDeclaration* decl);
        
        void handle_expr_statement(SgExprStatement* expr_s);
        
        void traverse_statements(SgStatementPtrList& stmt_list);
    public:
        string func_name;
        CodeCreater* _in_pool;
        SgFunctionDeclaration* decl;
        VariableTable var_tbl;
        VariableTable input_parameters;
        vector<Path> path_list;
        
        Function(const string & _func_name, SgFunctionDeclaration* _decl, CodeCreater* _pool = &paths_pool);
        
        void add_variable(const Variable &v);
        
        void add_procedure(const string& ref_name, const SgExpression* expr);
        
        void add_constraint(SgExpression* expr, bool is_not);
        
        void add_loop(const Loop& loop);
        
        VariableTable collect_all_var_tbl();
        
        void clear_returned_path();
        
        void merge(const Function & f);
        
        string to_string() const;
    
        string to_code() const;
    
        string to_klee_code_functions();
    };
    
}
#endif //CODE_ANALYSE_FUNCTION_H
