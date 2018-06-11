//
// Created by whj on 6/11/18.
//

#include <sstream>
#include "Function.h"

namespace le
{
    bool isAtomStatement(const SgStatement *stmt)
    {
        return dynamic_cast<const SgForStatement *>(stmt) ||
               dynamic_cast<const SgWhileStmt *>(stmt) ||
               dynamic_cast<const SgDoWhileStmt *>(stmt) ||
               dynamic_cast<const SgIfStmt *>(stmt) ||
               dynamic_cast<const SgSwitchStatement *>(stmt);
    }
    
    void Function::add_input_parameter(const le::Variable &v)
    {
        var_tbl.add_variable(v);
        input_parameters.add_variable(v);
    }
    
    void Function::add_input_parameterlist()
    {
        SgFunctionParameterList *params = decl->get_parameterList();
        for (SgInitializedName *param : params->get_args())
        {
            std::string name = param->get_name().getString();
            SgType *type = param->get_type();
            Variable v(name, type);
            add_input_parameter(v);
        }
    }
    
    void Function::handle_statement(SgStatement *stmt)
    {
    
    }
    
    void Function::traverse_statements(SgStatementPtrList& stmt_list)
    {
        // TODO
    }
    
    Function::Function(const std::string &_func_name, SgFunctionDeclaration* _decl) :
            func_name(_func_name), decl(_decl)
    {
        if(decl == nullptr)
            return;
        path_list.push_back(Path());
        add_input_parameterlist();
    
        SgFunctionDefinition *func_def = decl->get_definition();
        if (func_def == nullptr)
        {
            cout << decl->unparseToString() << " has no definition block!" << endl;
            return;
        }
        SgBasicBlock *func_body = func_def->get_body();
        SgStatementPtrList stmt_list = func_body->get_statements();
        traverse_statements(stmt_list);
    }
    
    void Function::add_variable(const le::Variable &v)
    {
        if(var_tbl.has_variable(v.var_name))
            return;
        for(auto p : path_list)
        {
            if(p.var_tbl.has_variable(v.var_name)) continue;
            p.var_tbl.add_variable(v);
        }
        
        if (v.initExpr == nullptr) return;
        
        // put initial expression to path
        for (auto p : path_list)
        {
            p.add_procedure(v, v.initExpr);
        }
    }
    
    void Function::merge(const le::Function &f)
    {
    
    }
    
    string Function::to_string()
    {
        stringstream ss;
        ss << "\"function_name\": " << "\"" << func_name << "\"" << endl;
        ss << "\"variables\": " << var_tbl.to_string() << endl;
        ss << "\"input_variables\": " << input_parameters.to_string() << endl;
        ss << "\"paths\": [" << endl;
        for(auto p : path_list)
        {
            ss << p.to_string(1);
        }
        ss << "]" << endl;
        return ss.str();
    }
    
}