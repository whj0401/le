//
// Created by whj on 6/13/18.
//


#include "common.h"

namespace le
{
    const std::string TAB = "    ";
    
    SgVarRefExp* get_func_call_lhs(SgFunctionCallExp* func_call)
    {
        auto suc_list = func_call->get_traversalSuccessorContainer();
        for(auto n : suc_list)
        {
            if(auto dot_exp = dynamic_cast<SgDotExp*>(n))
            {
                SgExpression* left = dot_exp->get_lhs_operand();
                return dynamic_cast<SgVarRefExp*>(left);
            }
        }
        return nullptr;
    }
}