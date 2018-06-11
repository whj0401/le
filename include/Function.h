//
// Created by whj on 6/11/18.
//

#ifndef CODE_ANALYSE_FUNCTION_H
#define CODE_ANALYSE_FUNCTION_H

#include <iostream>
#include <vector>
#include <string>
#include "Path.h"

namespace le
{
    
    using namespace std;
    
    bool isAtomStatement(const SgStatement *stmt);
    
    class Function
    {
    private:
        void add_input_parameter(const Variable &v);
    
        void add_input_parameterlist();
        
        void handle_statement(SgStatement* stmt);
        
        void handle_block_statement(SgBasicBlock* block);
        
        void handle_for_statement(SgForStatement* for_stmt);
        
        void handle_while_statement(SgWhileStmt* while_stmt);
        
        void handle_dowhile_statement(SgDoWhileStmt* dowhile_stmt);
        
        void handle_if_statement(SgIfStmt* if_stmt);
        
        void handle_switch_statement(SgSwitchStatement* switch_stmt);
        
        void handle_return_statement(SgReturnStmt* return_stmt);
        
        void traverse_statements(SgStatementPtrList& stmt_list);
    public:
        string func_name;
        SgFunctionDeclaration* decl;
        VariableTable var_tbl;
        VariableTable input_parameters;
        vector<Path> path_list;
        
        Function(const string & _func_name, SgFunctionDeclaration* _decl);
        
        void add_variable(const Variable &v);
        
        void merge(const Function & f);
        
        string to_string();
    };
    
}
#endif //CODE_ANALYSE_FUNCTION_H
