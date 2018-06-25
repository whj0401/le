//
// Created by whj on 6/18/18.
//

#include "REAL_function_list.h"

namespace le
{
    using namespace std;
    
    map<string, int> real_member_func_set;
    map<string, int> real_relative_func_set;
    
    typedef pair<string, int> FUNC_PARANUM;
    
    void init_member_set()
    {
        real_member_func_set.clear();
        real_member_func_set.insert(FUNC_PARANUM("public: inline REAL &operator=(const class REAL &y);", 1));
    }
    
    void init_relative_set()
    {
        real_relative_func_set.clear();
        // +
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator+(const class REAL &x,const class REAL &y);",
                             2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator+(const int n,const class REAL &x);", 2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator+(const class REAL &x,const int n);", 2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator+(const double n,const class REAL &x);", 2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator+(const class REAL &x,const double n);", 2));
        // -
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator-(const class REAL &x,const class REAL &y);",
                             2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator-(const int n,const class REAL &x);", 2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator-(const class REAL &x,const int n);", 2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator-(const double n,const class REAL &x);", 2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator-(const class REAL &x,const double n);", 2));
        // *
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator*(const class REAL &x,const class REAL &y);",
                             2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator*(const int n,const class REAL &x);", 2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator*(const class REAL &x,const int n);", 2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator*(const double n,const class REAL &x);", 2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator*(const class REAL &x,const double n);", 2));
        // /
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator/(const class REAL &x,const class REAL &y);",
                             2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator/(const int n,const class REAL &x);", 2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator/(const class REAL &x,const int n);", 2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator/(const double n,const class REAL &x);", 2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL operator/(const class REAL &x,const double n);", 2));
        
        // +=, -=, *=, /=
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL &operator+=(class REAL &x,const class REAL &y);", 2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL &operator-=(class REAL &x,const class REAL &y);", 2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL &operator*=(class REAL &x,const class REAL &y);", 2));
        real_relative_func_set.insert(
                FUNC_PARANUM("public: friend inline class REAL &operator/=(class REAL &x,const class REAL &y);", 2));
    }
    
    SgDotExp *get_member_func_dotExp(SgFunctionCallExp *func_call)
    {
        auto suc_list = func_call->get_traversalSuccessorContainer();
        for (auto n : suc_list)
        {
            if (auto dot_exp = dynamic_cast<SgDotExp *>(n))
            {
                return dot_exp;
            }
        }
        return nullptr;
    }
    
    SgExpression *get_member_func_caller(SgFunctionCallExp *func_call)
    {
        SgDotExp *dot_exp = get_member_func_dotExp(func_call);
        if (dot_exp == nullptr) return nullptr;
        return dot_exp->get_lhs_operand();
    }
    
    SgMemberFunctionRefExp *get_member_func_refExp(SgFunctionCallExp *func_call)
    {
        SgDotExp *dot_exp = get_member_func_dotExp(func_call);
        if (dot_exp == nullptr) return nullptr;
        
        if (auto ref = dynamic_cast<SgMemberFunctionRefExp *>(dot_exp->get_rhs_operand()))
        {
            return ref;
        }
        auto suc_list = dot_exp->get_traversalSuccessorContainer();
        for (auto n : suc_list)
        {
            if (auto ref = dynamic_cast<SgMemberFunctionRefExp *>(n))
            {
                return ref;
            }
        }
        return nullptr;
    }
    
    SgFunctionRefExp *get_relative_func_refExp(SgFunctionCallExp *func_call)
    {
        auto suc_list = func_call->get_traversalSuccessorContainer();
        for (auto n : suc_list)
        {
            if (auto ref = dynamic_cast<SgFunctionRefExp *>(n))
            {
                return ref;
            }
        }
        return nullptr;
    }
    
    bool is_compound_assign_option(SgExpression *expr)
    {
        if (auto func_call = dynamic_cast<SgFunctionCallExp *>(expr))
        {
            string func_decl_str = func_call->getAssociatedFunctionDeclaration()->unparseToString();
            if (find_in_relative_func_set(func_decl_str))
            {
                const SgFunctionRefExp *ref_exp = get_relative_func_refExp(func_call);
                if (ref_exp->unparseToString() == "+=" ||
                    ref_exp->unparseToString() == "-=" ||
                    ref_exp->unparseToString() == "*=" ||
                    ref_exp->unparseToString() == "/=")
                {
                    return true;
                }
            }
        }
        else if (auto compound_op = dynamic_cast<SgCompoundAssignOp *>(expr))
        {
            return true;
        }
            // plusplus, minusminus put here temporarily
        else if (auto pp_op = dynamic_cast<SgPlusPlusOp *>(expr))
        {
            return true;
        }
        else if (auto mm_op = dynamic_cast<SgMinusMinusOp *>(expr))
        {
            return true;
        }
        return false;
    }
    
    string get_compound_assign_option_str(SgExpression *expr)
    {
        if (auto func_call = dynamic_cast<SgFunctionCallExp *>(expr))
        {
            string func_decl_str = func_call->getAssociatedFunctionDeclaration()->unparseToString();
            if (find_in_relative_func_set(func_decl_str))
            {
                SgFunctionRefExp *ref_exp = get_relative_func_refExp(func_call);
                string op = ref_exp->unparseToString();
                if (op == "+=" ||
                    op == "-=" ||
                    op == "*=" ||
                    op == "/=")
                {
                    SgExpressionPtrList expr_list = get_func_parameters(func_call);
                    return expr_list[0]->unparseToString() + op.substr(0, 1) + expr_list[1]->unparseToString();
                }
            }
        }
        else if (auto compound_op = dynamic_cast<SgCompoundAssignOp *>(expr))
        {
            string tmp = compound_op->unparseToString();
            auto iter = tmp.find('=');
            tmp = tmp.erase(iter, 1);
            return tmp;
        }
            // plusplus, minusminus are put here temporarily
        else if (auto pp_op = dynamic_cast<SgPlusPlusOp *>(expr))
        {
            SgExpression *ref = pp_op->get_operand_i();
            return ref->unparseToString() + " + 1";
        }
        else if (auto mm_op = dynamic_cast<SgMinusMinusOp *>(expr))
        {
            SgExpression *ref = mm_op->get_operand_i();
            return ref->unparseToString() + " - 1";
        }
        return expr->unparseToString();
    }
    
    template<class T>
    T *create_binary_option(SgExpression *lhs, SgExpression *rhs, SgType *type)
    {
        return new T(lhs, rhs, type);
    }
    
    SgBinaryOp *create_relative_binaryOp(SgExpression *expr)
    {
        if (auto func_call = dynamic_cast<SgFunctionCallExp *>(expr))
        {
            string func_decl_str = func_call->getAssociatedFunctionDeclaration()->unparseToString();
            if (find_in_relative_func_set(func_decl_str))
            {
                const SgFunctionRefExp *ref_exp = get_relative_func_refExp(func_call);
                SgExpressionPtrList expr_list = get_func_parameters(func_call);
                if (ref_exp->unparseToString() == "+=")
                {
                    return create_binary_option<SgAddOp>(expr_list[0], expr_list[1], ref_exp->get_type());
                }
                else if (ref_exp->unparseToString() == "-=")
                {
                    return create_binary_option<SgSubtractOp>(expr_list[0], expr_list[1], ref_exp->get_type());
                }
                else if (ref_exp->unparseToString() == "*=")
                {
                    return create_binary_option<SgMultiplyOp>(expr_list[0], expr_list[1], ref_exp->get_type());
                }
                else if (ref_exp->unparseToString() == "/=")
                {
                    return create_binary_option<SgDivideOp>(expr_list[0], expr_list[1], ref_exp->get_type());
                }
            }
        }
        else if (auto compound_op = dynamic_cast<SgCompoundAssignOp *>(expr))
        {
            if (compound_op->variantT() == V_SgPlusAssignOp)
            {
                return create_binary_option<SgAddOp>(compound_op->get_lhs_operand(), compound_op->get_rhs_operand(),
                                                     compound_op->get_type());
            }
            else if (compound_op->variantT() == V_SgMinusAssignOp)
            {
                return create_binary_option<SgSubtractOp>(compound_op->get_lhs_operand(),
                                                          compound_op->get_rhs_operand(), compound_op->get_type());
            }
        }
        return nullptr;
    }
}