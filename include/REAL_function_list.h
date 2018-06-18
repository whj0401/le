//
// Created by whj on 6/18/18.
//

#ifndef CODE_ANALYSE_REAL_FUNCTION_LIST_H
#define CODE_ANALYSE_REAL_FUNCTION_LIST_H

#include <vector>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include "rose.h"

namespace le
{
    using namespace std;
    
    extern map<string, int> real_member_func_set;
    extern map<string, int> real_relative_func_set;
    
    void init_member_set();
    
    void init_relative_set();
    
    inline bool find_in_member_func_set(const string &func_decl)
    {
        return (real_member_func_set.find(func_decl) != real_member_func_set.end());
    }
    
    inline bool find_in_relative_func_set(const string &func_decl)
    {
        return (real_relative_func_set.find(func_decl) != real_relative_func_set.end());
    }
    
    SgDotExp *get_member_func_dotExp(SgFunctionCallExp *func_call);
    
    SgExpression *get_member_func_caller(SgFunctionCallExp *func_call);
    
    SgMemberFunctionRefExp *get_member_func_refExp(SgFunctionCallExp *func_call);
    
    SgFunctionRefExp *get_relative_func_refExp(SgFunctionCallExp *func_call);
    
    inline SgExpressionPtrList get_func_parameters(SgFunctionCallExp *func_call)
    {
        return func_call->get_args()->get_expressions();
    }
    
    bool is_compound_assign_option(SgExpression *expr);
    
    string get_compound_assign_option_str(SgExpression *expr);
    
    SgBinaryOp *create_relative_binaryOp(SgExpression *expr);
}

#endif //CODE_ANALYSE_REAL_FUNCTION_LIST_H
