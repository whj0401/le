//
// Created by whj on 6/13/18.
//

#include "Loop.h"
#include "Function.h"
#include "debug.h"
#include "REAL_function_list.h"

namespace le
{
    void Loop::handle_statement(SgStatement *stmt, bool is_initial)
    {
        if(stmt == nullptr) return;
        if(auto s = dynamic_cast<SgBasicBlock*>(stmt))
        {
            handle_block_statement(s, is_initial);
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
        else if(auto s = dynamic_cast<SgContinueStmt*>(stmt))
        {
            handle_continue_statement(s);
        }
        else if(auto s = dynamic_cast<SgBreakStmt*>(stmt))
        {
            handle_break_statement(s);
        }
        else if(auto s = dynamic_cast<SgReturnStmt*>(stmt))
        {
            handle_return_statement(s);
        }
        else if(auto s = dynamic_cast<SgVariableDeclaration*>(stmt))
        {
            handle_var_declaration(s, is_initial);
        }
        else if(auto s = dynamic_cast<SgExprStatement*>(stmt))
        {
            handle_expr_statement(s, is_initial);
        }
        else
        {
            print_err_statement(stmt);
        }
    }
    
    void Loop::handle_expression(SgExpression *expr, bool is_initial)
    {
        if(auto func_call = dynamic_cast<SgFunctionCallExp*>(expr))
        {
            string func_decl_str = func_call->getAssociatedFunctionDeclaration()->unparseToString();
            if (find_in_member_func_set(func_decl_str))
            {
                SgExpressionPtrList expr_list = get_func_parameters(func_call);
                SgMemberFunctionRefExp *ref_exp = get_member_func_refExp(func_call);
                if (ref_exp->unparseToString() == "=")
                {// now only has assign value operator= in member function set
                    SgExpression *caller = get_member_func_caller(func_call);
                    add_procedure(caller->unparseToString(), expr_list[0], is_initial);
                    handle_expression(expr_list[0], is_initial);
                }
            }
            else if (find_in_relative_func_set(func_decl_str))
            {
                SgExpressionPtrList expr_list = get_func_parameters(func_call);
                SgFunctionRefExp *ref_exp = get_relative_func_refExp(func_call);
                if (ref_exp->unparseToString() == "+" ||
                    ref_exp->unparseToString() == "-" ||
                    ref_exp->unparseToString() == "*" ||
                    ref_exp->unparseToString() == "/")
                {
                    for (auto e : expr_list)
                    {
                        handle_expression(e, is_initial);
                    }
                }
                else if (ref_exp->unparseToString() == "+=" ||
                         ref_exp->unparseToString() == "-=" ||
                         ref_exp->unparseToString() == "*=" ||
                         ref_exp->unparseToString() == "/=")
                {
                    add_procedure(expr_list[0]->unparseToString(), func_call, is_initial);
                    handle_expression(expr_list[1], is_initial);
                }
            }
            else
            {
                // unknown function call
                // do nothing
            }
        }
        else
        {
            if (auto assign_op = dynamic_cast<SgAssignOp *>(expr))
            {
                SgExpression *ref = assign_op->get_lhs_operand();
                SgExpression *value = assign_op->get_rhs_operand();
                add_procedure(ref->unparseToString(), value, is_initial);
                handle_expression(value, is_initial);
            }
            else if (auto compound_op = dynamic_cast<SgCompoundAssignOp *>(expr))
            {
                SgExpression *ref = compound_op->get_lhs_operand();
                add_procedure(ref->unparseToString(), expr, is_initial);
                handle_expression(compound_op->get_rhs_operand(), is_initial);
            }
            else if (auto pp_op = dynamic_cast<SgPlusPlusOp *>(expr))
            {
                SgExpression *ref = pp_op->get_operand_i();
                add_procedure(ref->unparseToString(), expr, is_initial);
                handle_expression(ref, is_initial);
            }
            else if (auto mm_op = dynamic_cast<SgMinusMinusOp *>(expr))
            {
                SgExpression *ref = pp_op->get_operand_i();
                add_procedure(ref->unparseToString(), expr, is_initial);
                handle_expression(ref, is_initial);
            }
        }
    }
    
    void Loop::handle_block_statement(SgBasicBlock *block, bool is_initial)
    {
        SgStatementPtrList &stmt_list = block->get_statements();
        for(auto s : stmt_list)
        {
            handle_statement(s, is_initial);
        }
    }
    
    void Loop::handle_for_statement(SgForStatement *for_stmt)
    {
        Loop* loop = _in_pool->create_loop(for_stmt, func_ptr, this);
        add_loop(*loop);
    }
    
    void Loop::handle_while_statement(SgWhileStmt *while_stmt)
    {
        Loop *loop = _in_pool->create_loop(while_stmt, func_ptr, this);
        add_loop(*loop);
    }
    
    void Loop::handle_dowhile_statement(SgDoWhileStmt *dowhile_stmt)
    {
        Loop *loop1 = _in_pool->create_loop(dowhile_stmt, func_ptr, this, 1);
        add_loop(*loop1);
        Loop *loop2 = _in_pool->create_loop(dowhile_stmt, func_ptr, this, 2);
        add_loop(*loop2);
    }
    
    void Loop::handle_if_statement(SgIfStmt *if_stmt)
    {
        SgStatement *condition_stmt = if_stmt->get_conditional();
        if (auto *expr_stmt = dynamic_cast<SgExprStatement *>(condition_stmt))
        {
            SgExpression *condition = expr_stmt->get_expression();
            handle_expression(condition, false);
            Loop new_loop = *this;
            new_loop.clear_path_with_continue_break_return();
        
            new_loop.add_constraint(condition, false);
            this->add_constraint(condition, true);
        
            this->handle_statement(if_stmt->get_true_body(), false);
            new_loop.handle_statement(if_stmt->get_false_body(), false);
        
            this->merge(new_loop);
        }
    }
    
    void Loop::handle_switch_statement(SgSwitchStatement *switch_stmt)
    {
        // TODO
    }
    
    void Loop::handle_continue_statement(SgContinueStmt *continue_stmt)
    {
        for(auto & path : path_list)
        {
            if(path.can_add_code())
                path.can_continue = true;
        }
    }
    
    void Loop::handle_break_statement(SgBreakStmt *break_stmt)
    {
        for(auto & path : path_list)
        {
            if(path.can_add_code())
                path.can_break = true;
        }
    }
    
    void Loop::handle_return_statement(SgReturnStmt *return_stmt)
    {
        SgExpression *expr = return_stmt->get_expression();
        for(auto &p : path_list)
        {
            if(p.is_return) continue;
            p.is_return = true;
            p.ret_expr = expr;
        }
    }
    
    void Loop::handle_var_declaration(SgVariableDeclaration *decl, const bool is_initial)
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
                    add_variable(var, is_initial);
                }
                else
                {
                    Variable var(var_name, type, list_exp->get_expressions()[0]);
                    add_variable(var, is_initial);
                }
            }
            else if(auto initializer = dynamic_cast<SgAssignInitializer *>(n->get_initptr()))
            {
                // basic type initializer
                Variable var(var_name, type, initializer->get_operand());
                add_variable(var, is_initial);
            }
            else
            {
                Variable var(var_name, type);
                add_variable(var, is_initial);
            }
        }
    }
    
    void Loop::handle_expr_statement(SgExprStatement *expr_s, bool is_initial)
    {
//        print_whole_node(expr_s);
        SgExpression *expr = expr_s->get_expression();
        handle_expression(expr, is_initial);
    }
    
    void Loop::set_init_loop_path(const SgExpression *condition)
    {
        if(condition == nullptr)
        {
            path_list.push_back(Path());
            return;
        }
        // if has a test condition, add two path to loop_list, "enter loop" and "break loop"
        Path enter_loop;
        Constraint enter_con(condition, true);
        enter_loop.add_constraint(enter_con);
        path_list.push_back(enter_loop);
    
        Path break_loop;
        Constraint break_con(condition, false);
        break_loop.add_constraint(break_con);
        break_loop.can_break = true;
        path_list.push_back(break_loop);
    }
    
    void Loop::set_init_loop_var_tbl()
    {
        if(father_loop == nullptr)
        {
            var_tbl = func_ptr->collect_all_var_tbl();
        }
        else
        {
            var_tbl = father_loop->collect_all_var_tbl();
        }
    }
    
    void Loop::init_for_statement(const SgForStatement *for_stmt)
    {
        set_init_loop_var_tbl();
        SgForInitStatement *init_block = for_stmt->get_for_init_stmt();
        SgStatementPtrList &init_stmts = init_block->get_init_stmt();
        for(auto s : init_stmts)
        {
            handle_statement(s, true);
        }
    
        SgStatement *test_stmt = for_stmt->get_test();
        SgExpression *condition = nullptr;
        // condition may be null e.g. for(;;) {}
        if (auto expr_stmt = dynamic_cast<SgExprStatement *>(test_stmt))
        {
            condition = expr_stmt->get_expression();
        }
        
        set_init_loop_path(condition);
    
        SgStatement *body_stmt = for_stmt->get_loop_body();
        handle_statement(body_stmt, false);
    
        SgExpression *inc_expr = for_stmt->get_increment();
        // add increment operation to tail of paths(none break, none return)
        vector<size_t> continue_idx;
        for(int i = 0; i < path_list.size(); ++i)
        {
            if(path_list[i].can_continue)
            {
                continue_idx.push_back(i);
                path_list[i].can_continue = false;
            }
        }
        handle_expression(inc_expr, false);
        for(auto i : continue_idx)
        {
            path_list[i].can_continue = true;
        }
    }
    
    void Loop::init_while_statement(const SgWhileStmt *while_stmt)
    {
        set_init_loop_var_tbl();
        SgStatement *test_stmt = while_stmt->get_condition();
        SgExpression *condition = nullptr;
        if (auto expr_stmt = dynamic_cast<SgExprStatement *>(test_stmt))
        {
            condition = expr_stmt->get_expression();
        }
        set_init_loop_path(condition);
    
        SgStatement *body_stmt = while_stmt->get_body();
        handle_statement(body_stmt, false);
    }
    
    void Loop::init_dowhile_statement1(const SgDoWhileStmt *dowhile_stmt)
    {
        // this will be a while true loop(first execution of do-while loop is always true)
        set_init_loop_var_tbl();
        set_init_loop_path(nullptr);
        SgStatement *body_stmt = dowhile_stmt->get_body();
        handle_statement(body_stmt, false);
    
        SgStatement *test_stmt = dowhile_stmt->get_condition();
        SgExpression *condition = nullptr;
        if (auto expr_stmt = dynamic_cast<SgExprStatement *>(test_stmt))
        {
            condition = expr_stmt->get_expression();
        }
        // do-while loop condition cannot be null
        assert(condition != nullptr);
        Constraint break_loop_constraint(condition);
        
        for (auto &p : path_list)
        {
            // every path must disobey the condition of do-while loop
            p.add_constraint(break_loop_constraint);
            if (p.is_return) continue;
            if (p.can_break) continue;
            // all paths will be run once
            p.can_break = true;
            p.can_continue = false;
        }
        Path all_break_path;
        all_break_path.can_break = true;
        path_list.push_back(all_break_path);
    }
    
    void Loop::init_dowhile_statement2(const SgDoWhileStmt *dowhile_stmt)
    {
        // this will be a simple while loop, just change the do-while to while
        set_init_loop_var_tbl();
        SgStatement *test_stmt = dowhile_stmt->get_condition();
        SgExpression *condition = nullptr;
        if (auto expr_stmt = dynamic_cast<SgExprStatement *>(test_stmt))
        {
            condition = expr_stmt->get_expression();
        }
        set_init_loop_path(condition);
        // all path obey the do-while condition
        SgStatement *body_stmt = dowhile_stmt->get_body();
        handle_statement(body_stmt, false);
    }
    
    Loop::Loop(const SgStatement *_loop_stmt, Function *_func_ptr, Loop *_father_loop, CodeCreater *_pool,
               int _dowhile_loop_count) :
            loop_stmt(_loop_stmt), func_ptr(_func_ptr), father_loop(_father_loop), _in_pool(_pool)
    {
        Code::t = loop;
        if (auto s = dynamic_cast<const SgForStatement *>(loop_stmt))
        {
            loop_type = FOR;
            init_for_statement(s);
        }
        else if (auto s = dynamic_cast<const SgWhileStmt *>(loop_stmt))
        {
            loop_type = WHILE;
            init_while_statement(s);
        }
        else if (auto s = dynamic_cast<const SgDoWhileStmt *>(loop_stmt))
        {
            loop_type = DOWHILE;
            // do while loop will execute at least once, we make while(true) loop at first
            // this while(true) loop will execute with paths in do-while loop without check the constraint of do-while loop
            // and they all have a break
            if (_dowhile_loop_count == 1)
            {
                init_dowhile_statement1(s);
            }
                // then we will run a while loop, with constraint of the actual do-while loop
            else if (_dowhile_loop_count == 2)
            {
                init_dowhile_statement2(s);
            }
            else
            {
                assert(false);
            }
        }
        else
        {
            assert(false);
        }
    }
    
    void Loop::add_variable(const le::Variable &v, const bool is_initial)
    {
        if(var_tbl.has_variable(v.var_name))
            return;
        if(is_initial)
        {
            var_tbl.add_variable(v);
        }
        else
        {
            for(auto &p : path_list)
            {
                if(p.var_tbl.has_variable(v.var_name)) continue;
                p.var_tbl.add_variable(v);
            }
        }
    
        if(v.initExpr == nullptr) return;
        if(is_initial)
        {
            initialize.add_procedure(v, v.initExpr);
        }
        else
        {
            // put initial expression to path
            for (auto &p : path_list)
            {
                p.add_procedure(v, v.initExpr);
            }
        }
    }
    
    void Loop::add_procedure(const std::string &ref_name, SgExpression *expr, bool is_initial)
    {
        if(var_tbl.has_variable(ref_name))
        {
            const Variable & var = var_tbl.find(ref_name);
            if(is_initial)
            {
                initialize.add_procedure(var, expr);
            }
            else
            {
                for(auto & path : path_list)
                {
                    path.add_procedure(var, expr);
                }
            }
        }
        else
        {
            if(is_initial)
            {
                // the value used in initial block must have been declared.
                print_err_use_variable_without_declaration(ref_name, initialize);
                assert(false);
            }
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
    
    void Loop::add_loop(const le::Loop &loop)
    {
        for(auto & p : path_list)
        {
            if(p.can_add_code())
            {
                p.add_loop(&loop);
            }
        }
    }
    
    void Loop::add_constraint(SgExpression *expr, bool is_not)
    {
        Constraint tmp(expr, is_not);
        for (auto &path : path_list)
        {
            if (path.is_return) continue;
            path.add_constraint(tmp);
        }
    }
    
    VariableTable Loop::collect_all_var_tbl()
    {
        VariableTable tmp = var_tbl;
        for(auto & p : path_list)
        {
            if (!p.can_add_code()) continue;
            for(auto v : p.var_tbl.T)
            {
                tmp.add_variable(v.second);
            }
        }
        return tmp;
    }
    
    void Loop::clear_path_with_continue_break_return()
    {
        vector<Path> new_path_list;
        for (auto &path : path_list)
        {
            if (path.can_add_code())
            {
                new_path_list.push_back(path);
            }
        }
        path_list = new_path_list;
    }
    
    void Loop::merge(const le::Loop &l)
    {
        for (auto &path : l.path_list)
        {
            this->path_list.push_back(path);
        }
    }
    
    string Loop::to_string(unsigned int tab_num) const
    {
        stringstream ss;
        string tab = generate_tab(tab_num);
        ss << endl;
        ss << tab << "{" << endl;
        ss << tab << TAB << "\"type\": " << "\"loop\"," << endl;
        ss << tab << TAB << "\"content\": " << "{" << endl;
        ss << tab << TAB << TAB << "\"variables\": " << var_tbl.to_string() << endl;
        ss << tab << TAB << TAB << "\"initialize\": " << initialize.to_string_as_initializer() << endl;
        ss << tab << TAB << TAB << "\"loop_body\": [" << endl;
        for (const auto &p : path_list)
        {
            ss << p.to_string(tab_num + 3);
        }
        ss << tab << TAB << TAB << "]" << endl;
        ss << tab << TAB << "}" << endl;
        ss << tab << "}";
        return ss.str();
    }
    
    string Loop::to_code(unsigned int tab_num) const
    {
        stringstream ss;
        string tab = generate_tab(tab_num);
        ss << endl;
        ss << tab << "{" << endl;
        ss << tab << TAB << "\"type\": " << "\"loop\"," << endl;
        ss << tab << TAB << "\"content\": " << "{" << endl;
        ss << tab << TAB << TAB << "\"variables\": " << var_tbl.to_string() << endl;
        ss << tab << TAB << TAB << "\"initialize\": " << initialize.to_string_as_initializer() << endl;
        ss << tab << TAB << TAB << "\"loop_body\": [" << endl;
        for (const auto &p : path_list)
        {
            ss << p.to_code(tab_num + 3);
        }
        ss << tab << TAB << TAB << "]" << endl;
        ss << tab << TAB << "}" << endl;
        ss << tab << "}";
        return ss.str();
    }
    
    void Loop::to_klee_code_functions()
    {
        for (auto &p : path_list)
        {
            p.to_klee_code_functions(var_tbl);
        }
    }
}