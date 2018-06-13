//
// Created by whj on 6/11/18.
//

#include <sstream>
#include "Function.h"
#include "debug.h"

namespace le
{
    using namespace std;
    
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
//        print_whole_node(stmt);
        if(stmt == nullptr) return;
        if(auto s = dynamic_cast<SgBasicBlock*>(stmt))
        {
            handle_block_statement(s);
        }
        else if(auto s = dynamic_cast<SgForStatement*>(stmt))
        {
            handle_for_statement(s);
        }
        else if(auto s = dynamic_cast<SgWhileStmt*>(stmt))
        {
            handle_while_statement(s);
        }
        else if(auto s = dynamic_cast<SgDoWhileStmt*>(stmt))
        {
            handle_dowhile_statement(s);
        }
        else if(auto s = dynamic_cast<SgIfStmt*>(stmt))
        {
            handle_if_statement(s);
        }
        else if(auto s = dynamic_cast<SgSwitchStatement*>(stmt))
        {
            // TODO
            handle_switch_statement(s);
        }
        else if(auto s = dynamic_cast<SgReturnStmt*>(stmt))
        {
            handle_return_statement(s);
        }
        else if(auto s = dynamic_cast<SgVariableDeclaration*>(stmt))
        {
            handle_var_declaration(s);
        }
        else if(auto s = dynamic_cast<SgExprStatement*>(stmt))
        {
            handle_expr_statement(s);
        }
        else
        {
            print_err_statement(stmt);
        }
    }
    
    void Function::handle_block_statement(SgBasicBlock *block)
    {
        SgStatementPtrList &stmt_list = block->get_statements();
        for(auto s : stmt_list)
        {
            handle_statement(s);
        }
    }
    
    void Function::handle_for_statement(SgForStatement *for_stmt)
    {
//        print_whole_node(for_stmt);
        Loop* loop = _in_pool->create_loop(for_stmt, this, nullptr);
        add_loop(*loop);
    }
    
    void Function::handle_while_statement(SgWhileStmt *while_stmt)
    {
    
    }
    
    void Function::handle_dowhile_statement(SgDoWhileStmt *dowhile_stmt)
    {
    
    }
    
    void Function::handle_if_statement(SgIfStmt *if_stmt)
    {
        SgStatement *condition_stmt = if_stmt->get_conditional();
        if(auto *expr_stmt = dynamic_cast<SgExprStatement *>(condition_stmt))
        {
            SgExpression *condition = expr_stmt->get_expression();
            handle_expression(condition);
            Function new_func = *this;
            new_func.clear_returned_path();
            
            new_func.add_constraint(condition, false);
            this->add_constraint(condition, true);
            
            this->handle_statement(if_stmt->get_true_body());
            new_func.handle_statement(if_stmt->get_false_body());
            
            this->merge(new_func);
        }
    }
    
    void Function::handle_switch_statement(SgSwitchStatement *switch_stmt)
    {
        // TODO
    }
    
    void Function::handle_var_declaration(SgVariableDeclaration *decl)
    {
        SgInitializedNamePtrList &list = decl->get_variables();
        for (SgInitializedName *n : list)
        {
            SgType *type = n->get_type();
            string var_name = n->get_name();
            // check type must be iRRAM::REAL, do no process on basic type now!
//            if(type->get_mangled() != REAL_MANGLED)
//                return;
            if(auto initializer = dynamic_cast<SgConstructorInitializer*>(n->get_initptr()))
            {
                // for iRRAM::REAL type, initializer with constructor, rather than SgAssignInitializer
                // REAL constructor with 1 parameter
                SgExprListExp* list_exp = initializer->get_args();
                if(list_exp->get_expressions().empty())
                {
                    Variable var(var_name, type, nullptr);
                    add_variable(var);
                }
                else
                {
                    Variable var(var_name, type, list_exp->get_expressions()[0]);
                    add_variable(var);
                }
            }
            else if(auto initializer = dynamic_cast<SgAssignInitializer *>(n->get_initptr()))
            {
                // basic type initializer
                Variable var(var_name, type, initializer->get_operand());
                add_variable(var);
            }
            else
            {
                Variable var(var_name, type);
                add_variable(var);
            }
        }
    }
    
    void Function::handle_expression(SgExpression *expr)
    {
        if(auto func_call = dynamic_cast<SgFunctionCallExp*>(expr))
        {
            string func_decl_str = func_call->getAssociatedFunctionDeclaration()->unparseToString();
            if (func_decl_str == "public: inline REAL &operator=(const class REAL &y);")
            {
                SgExpressionPtrList expr_list = func_call->get_args()->get_expressions();
                SgVarRefExp* caller = get_func_call_lhs(func_call);
                add_procedure(caller->unparseToString(), expr_list[0]);
            }
            else if(func_decl_str == "public: friend inline class REAL operator+(const class REAL &x,const class REAL &y);" ||
                    func_decl_str == "public: friend inline class REAL operator+(const int n,const class REAL &x);" ||
                    func_decl_str == "public: friend inline class REAL operator+(const class REAL &x,const int n);" ||
                    func_decl_str == "public: friend inline class REAL operator+(const double n,const class REAL &x);" ||
                    func_decl_str == "public: friend inline class REAL operator+(const class REAL &x,const double n);")
            {
//                cout << func_call->unparseToString() << endl;
            }
            else if(func_decl_str == "public: friend inline class REAL operator-(const class REAL &x,const class REAL &y);")
            {
            
            }
            else if(func_decl_str == "public: friend inline class REAL operator*(const class REAL &x,const class REAL &y);")
            {
            
            }
            else if(func_decl_str == "public: friend inline class REAL operator/(const class REAL &x,const class REAL &y);")
            {
            
            }
            else if(func_decl_str == "public: inline REAL &operator+=(const class REAL &y);")
            {
            
            }
            else if(func_decl_str == "public: inline REAL &operator++();")
            {
            
            }
        }
    }
    
    void Function::handle_expr_statement(SgExprStatement *expr_s)
    {
//        print_whole_node(expr_s);
        SgExpression *expr = expr_s->get_expression();
        handle_expression(expr);
    }
    
    void Function::handle_return_statement(SgReturnStmt *return_stmt)
    {
        SgExpression *expr = return_stmt->get_expression();
        for(auto &p : path_list)
        {
            if(p.is_return) continue;
            p.is_return = true;
            p.ret_expr = expr;
        }
    }
    
    void Function::traverse_statements(SgStatementPtrList& stmt_list)
    {
        // TODO
    }
    
    Function::Function(const std::string &_func_name, SgFunctionDeclaration* _decl, CodeCreater* _pool) :
            func_name(_func_name), decl(_decl), _in_pool(_pool)
    {
        if(decl == nullptr)
            return;
        path_list.push_back(Path());
        add_input_parameterlist();
    
        SgFunctionDefinition *func_def = decl->get_definition();
        if (func_def == nullptr)
        {
            print_warning_function_no_definition(decl);
            return;
        }
        SgBasicBlock *func_body = func_def->get_body();
        handle_block_statement(func_body);
    }
    
    void Function::add_variable(const le::Variable &v)
    {
        if(var_tbl.has_variable(v.var_name))
            return;
        for(auto &p : path_list)
        {
            if(p.var_tbl.has_variable(v.var_name)) continue;
            p.var_tbl.add_variable(v);
        }
        
        if (v.initExpr == nullptr) return;
        
        // put initial expression to path
        for (auto &p : path_list)
        {
            p.add_procedure(v, v.initExpr);
        }
    }
    
    void Function::add_procedure(const std::string &ref_name, const SgExpression *expr)
    {
        if(var_tbl.has_variable(ref_name))
        {
            const Variable & var = var_tbl.find(ref_name);
            for(auto & path : path_list)
            {
                path.add_procedure(var, expr);
            }
        }
        else
        {
            for(auto & path : path_list)
            {
                if(path.var_tbl.has_variable(ref_name))
                {
                    const Variable & var = path.var_tbl.find(ref_name);
                    path.add_procedure(var, expr);
                }
                else
                {
                    print_err_use_variable_without_declaration(ref_name, path);
                    assert(false);
                }
            }
        }
    }
    
    void Function::add_constraint(SgExpression* expr, bool is_not)
    {
        Constraint tmp(expr, is_not);
        for(auto &path : path_list)
        {
            if(path.is_return) continue;
            path.add_constraint(tmp);
        }
    }
    
    void Function::add_loop(const le::Loop &loop)
    {
        for(auto & p : path_list)
        {
            if(p.can_add_code())
            {
                p.add_loop(&loop);
            }
        }
    }
    
    void Function::clear_returned_path()
    {
        vector<Path> new_path_list;
        for(auto &path : path_list)
        {
            if(path.is_return) continue;
            new_path_list.push_back(path);
        }
        path_list = new_path_list;
    }
    
    VariableTable Function::collect_all_var_tbl()
    {
        VariableTable tmp = var_tbl;
        for(auto & p : path_list)
        {
            for(auto v : p.var_tbl.T)
            {
                tmp.add_variable(v.second);
            }
        }
        return tmp;
    }
    
    void Function::merge(const le::Function &f)
    {
        for(auto &path : f.path_list)
        {
            path_list.push_back(path);
        }
    }
    
    string Function::to_string() const
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