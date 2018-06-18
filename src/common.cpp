//
// Created by whj on 6/13/18.
//


#include "common.h"

namespace le
{
    const std::string TAB = "    ";
    
    std::map<VariantT, std::string> operator_str_map;
    
    void init_operator_str_map()
    {
        operator_str_map[V_SgEqualityOp] = "==";
        operator_str_map[V_SgLessThanOp] = "<";
        operator_str_map[V_SgGreaterThanOp] = ">";
        operator_str_map[V_SgNotEqualOp] = "!=";
        operator_str_map[V_SgLessOrEqualOp] = "<=";
        operator_str_map[V_SgGreaterOrEqualOp] = ">=";
        operator_str_map[V_SgAddOp] = "+";
        operator_str_map[V_SgSubtractOp] = "-";
        operator_str_map[V_SgMultiplyOp] = "*";
        operator_str_map[V_SgDivideOp] = "/";
        operator_str_map[V_SgIntegerDivideOp] = "/";
        operator_str_map[V_SgModOp] = "%";
        operator_str_map[V_SgAndOp] = "&&";
        operator_str_map[V_SgOrOp] = "||";
        operator_str_map[V_SgBitXorOp] = "^";
        operator_str_map[V_SgBitAndOp] = "&";
        operator_str_map[V_SgBitOrOp] = "|";
        operator_str_map[V_SgCommaOpExp] = ",";
        operator_str_map[V_SgLshiftOp] = "<<";
        operator_str_map[V_SgRshiftOp] = ">>";
        operator_str_map[V_SgNotOp] = "!";
        operator_str_map[V_SgAssignOp] = "=";
        operator_str_map[V_SgPointerDerefExp] = "*";
        operator_str_map[V_SgAddressOfOp] = "&";
        operator_str_map[V_SgPlusAssignOp] = "+=";
        operator_str_map[V_SgMinusAssignOp] = "-=";
        operator_str_map[V_SgAndAssignOp] = "&&=";
        operator_str_map[V_SgIorAssignOp] = "||=";
        operator_str_map[V_SgMultAssignOp] = "*=";
        operator_str_map[V_SgDivAssignOp] = "/=";
        operator_str_map[V_SgModAssignOp] = "%=";
        operator_str_map[V_SgXorAssignOp] = "^=";
        operator_str_map[V_SgLshiftAssignOp] = "<<=";
    }
    
    std::string get_operator_str(VariantT type)
    {
        auto op = operator_str_map.find(type);
        assert(op != operator_str_map.end());
        return op->second;
    }
    
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
    
    bool write_stream_to_file(std::string out_file, std::stringstream &ss)
    {
        std::ofstream out;
        out.open(out_file, std::ofstream::out);
        if (out)
        {
            out << ss.str();
            out.close();
            return true;
        }
        return false;
    }
    
    void add_expr_to_stream(std::stringstream &os, SgExpression *expr)
    {
        if (expr == nullptr)
        {
            os << "[NULL]";
            return;
        }
        
        // if expr is a constant, print its value
        if (SgValueExp *value = dynamic_cast<SgValueExp *>(expr))
        {
            os << value->get_constant_folded_value_as_string();
            return;
        }
        
        // if expr is a variable, print its name
        if (SgVarRefExp *var = dynamic_cast<SgVarRefExp *>(expr))
        {
            os << var->get_symbol()->get_name().getString();
            return;
        }
        
        // if expr is a binary expression
        if (SgBinaryOp *binOp = dynamic_cast<SgBinaryOp *>(expr))
        {
            // array expression is handled differently from other binary operator
            if (SgPntrArrRefExp *arrRef = dynamic_cast<SgPntrArrRefExp *>(binOp))
            {
                add_expr_to_stream(os, arrRef->get_lhs_operand());
                os << "[";
                add_expr_to_stream(os, arrRef->get_rhs_operand());
                os << "]";
            }
            else
            {
                os << "(";
                add_expr_to_stream(os, binOp->get_lhs_operand());
                os << " " << get_operator_str(binOp->variantT()) << " ";
                add_expr_to_stream(os, binOp->get_rhs_operand());
                os << ")";
            }
        }
        else if (SgUnaryOp *unaryOp = dynamic_cast<SgUnaryOp *>(expr))
        {
            // unary operation
            os << "(";
            if (dynamic_cast<SgPlusPlusOp *>(unaryOp))
            {
                // ++
                add_expr_to_stream(os, unaryOp->get_operand());
                os << " + 1";
            }
            else if (dynamic_cast<SgMinusMinusOp *>(unaryOp))
            {
                // --
                add_expr_to_stream(os, unaryOp->get_operand());
                os << " - 1";
            }
            else if (SgCastExp *castExp = dynamic_cast<SgCastExp *>(unaryOp))
            {
                // cast
                add_expr_to_stream(os, castExp->get_operand());
            }
            else
            {
                os << get_operator_str(unaryOp->variantT());
                add_expr_to_stream(os, unaryOp->get_operand());
            }
            os << ")";
        }
        else if (SgAssignInitializer *initializer = dynamic_cast<SgAssignInitializer *>(expr))
        {
            // assign initializer
            SgExpression *operand = initializer->get_operand();
            add_expr_to_stream(os, operand);
        }
            // added by wanghuaijin
        else if (SgFunctionCallExp *call = dynamic_cast<SgFunctionCallExp *>(expr))
        {
            os << call->unparseToString();
        }
        else if (SgConstructorInitializer *init = dynamic_cast<SgConstructorInitializer *>(expr))
        {
            os << init->unparseToString();
        }
        else
        {
            std::stringstream ss;
            ss << expr->class_name() << " unsupported in Printer::add_expr_to_stream\n";
        }
    }
}