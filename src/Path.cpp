//
// Created by whj on 6/11/18.
//

#include "Path.h"
#include "Loop.h"
#include "REAL_function_list.h"
#include "debug.h"

namespace le
{
    using namespace std;
    
    CodeCreater paths_pool;
    
    size_t path_count = 0;
    
    Procedure::Procedure(const Variable &v, SgExpression *assign_expr) :
            left(v), right(assign_expr)
    {
        Code::t = procedure;
    }
    
    string Procedure::to_string(unsigned int tab_num) const
    {
        stringstream ss;
        string tab = generate_tab(tab_num);
        ss << endl;
        ss << tab << "{" << endl;
        ss << tab << TAB << "\"type\": " << "\"procedure\"," << endl;
        ss << tab << TAB << "\"content\": ";
        if(right == nullptr)
            ss << "[\"" << left.var_name << "\", " << "NULL";
        else if (is_compound_assign_option(right))
        {
            ss << "[\"" << left.var_name << "\", \"" << get_compound_assign_option_str(right) << "\"]";
        }
        else
        {
            ss << "[\"" << left.var_name << "\", \"" << right->unparseToString() << "\"]";
        }
        ss << endl << tab << "},";
        return ss.str();
    }
    
    string Procedure::to_code(unsigned int tab_num) const
    {
        stringstream ss;
        ss << generate_tab(tab_num);
        if (auto func_call = dynamic_cast<const SgFunctionCallExp *>(right))
        {
            string func_decl_str = func_call->getAssociatedFunctionDeclaration()->unparseToString();
            if (func_decl_str == "public: friend inline class REAL &operator+=(class REAL &x,const class REAL &y);")
            {
                SgExpressionPtrList expr_list = func_call->get_args()->get_expressions();
                ss << left.var_name << "=(" << left.var_name << ")+(" << expr_list[1]->unparseToString() << ");";
                return ss.str();
            }
            else if (func_decl_str ==
                     "public: friend inline class REAL &operator-=(class REAL &x,const class REAL &y);")
            {
                SgExpressionPtrList expr_list = func_call->get_args()->get_expressions();
                ss << left.var_name << "=(" << left.var_name << ")-(" << expr_list[1]->unparseToString() << ");";
                return ss.str();
            }
            else if (func_decl_str ==
                     "public: friend inline class REAL &operator*=(class REAL &x,const class REAL &y);")
            {
                SgExpressionPtrList expr_list = func_call->get_args()->get_expressions();
                ss << left.var_name << "=(" << left.var_name << ")*(" << expr_list[1]->unparseToString() << ");";
                return ss.str();
            }
            else if (func_decl_str ==
                     "public: friend inline class REAL &operator/=(class REAL &x,const class REAL &y);")
            {
                SgExpressionPtrList expr_list = func_call->get_args()->get_expressions();
                ss << left.var_name << "=(" << left.var_name << ")/(" << expr_list[1]->unparseToString() << ");";
                return ss.str();
            }
        }
        
        ss << left.var_name << "=";
        if (right == nullptr)
        { ss << "0;"; }
        else if (is_compound_assign_option(right))
        {
            ss << get_compound_assign_option_str(right);
        }
        else
        {
            ss << right->unparseToString();
        }
        return ss.str();
    }
    
    string CodeBlock::to_string(unsigned int tab_num) const
    {
        stringstream ss;
        string tab = generate_tab(tab_num);
        for (const auto &s : L)
        {
            ss << tab << s->unparseToString() << endl;
        }
        return ss.str();
    }
    
    string CodeBlock::to_code(unsigned int tab_num) const
    {
        return this->to_string(tab_num);
    }
    
    bool belongs_to_codeblock(SgStatement *s)
    {
        // TODO
        return false;
    }
    
    void Path::add_loop(const le::Loop *loop)
    {
        assert(pool == loop->_in_pool);
        codes.push_back(loop);
        current_value_map.clear();
        modify_cur_block();
    }
    
    void Path::modify_cur_value_map(string ref_name, const SgExpression *expr)
    {
        string right_str = expression_to_string_with_value_map(expr, current_value_map);
        if (current_value_map.find(ref_name) != current_value_map.end())
        {
            current_value_map.find(ref_name)->second = right_str;
        }
        else
        {
            current_value_map.insert(pair<string, string>(ref_name, right_str));
        }
    }
    
    void Path::modify_cur_block()
    {
        if (codes.empty())
        {
            return;
        }
        if (codes.back()->t == procedure)
        {
            if (blocks.empty() || !blocks.back().is_procedures)
            {
                Path_block procedures_tmp;
                procedures_tmp.is_procedures = true;
                blocks.push_back(procedures_tmp);
            }
            blocks.back().value_map = current_value_map;
        }
        else if (codes.back()->t == loop)
        {
            Path_block loop_tmp;
            loop_tmp.is_loop = true;
            loop_tmp.loop_ptr = (Loop *) (codes.back());
            assert(loop_tmp.loop_ptr != nullptr);
            blocks.push_back(loop_tmp);
        }
        else if (codes.back()->t == codeblock)
        {
        
        }
    }
    
    void Path::add_procedure(const Variable &v, SgExpression *e)
    {
        if (can_add_code())
        {
            codes.push_back(pool->create_procedure(v, e));
            modify_cur_value_map(v.var_name, e);
            modify_cur_block();
        }
    }
    
    void Path::add_procedure(const std::string &ref_name, SgExpression *expr)
    {
        if (var_tbl.has_variable(ref_name))
        {
            const Variable &var = var_tbl.find(ref_name);
            add_procedure(var, expr);
        }
        else
        {
            print_err_use_variable_without_declaration(ref_name, *this);
            assert(false);
        }
    }
    
    void Path::add_constraint(const SgExpression *expr, bool is_not)
    {
        if (!can_add_code()) return;
        Constraint tmp(expr, current_value_map, is_not);
        constraint_list.add_constraint(tmp);
    }
    
    string Path_block::to_string(unsigned int tab_num) const
    {
        stringstream ss;
        string tab = generate_tab(tab_num);
        if (is_procedures)
        {
            ss << endl;
            ss << tab << "{" << endl;
            ss << tab << TAB << "\"type\": " << "\"procedures\"," << endl;
            ss << tab << TAB << "\"content\": [" << endl;
            for (auto iter = value_map.begin(); iter != value_map.end(); ++iter)
            {
                ss << tab << TAB << TAB << "[" << iter->first << ", " << iter->second << "]," << endl;
            }
            ss << tab << TAB << "]" << endl;
            ss << tab << "}";
        }
        else if (is_loop)
        {
            assert(loop_ptr != nullptr);
            ss << loop_ptr->to_string(tab_num);
        }
        else
        {
            assert(codeblock_ptr != nullptr);
            ss << codeblock_ptr->to_string(tab_num);
        }
        return ss.str();
    }
    
    string Path::to_string(unsigned int tab_num) const
    {
        stringstream ss;
        string tab = generate_tab(tab_num);
    
        ss << tab << "{ #" << path_id << endl;
        ss << tab << TAB << "\"variables\": " << var_tbl.to_string() << "," << endl;
        ss << tab << TAB << "\"constraint\": [" << constraint_list.to_string() << "]," << endl;
        ss << tab << TAB << "\"path\": [";
//        for (const auto &p : codes)
//        {
//            ss << p->to_string(tab_num + 1);
//        }
        for (const auto &b : blocks)
        {
            ss << b.to_string(tab_num + 1);
        }
        if (is_return)
        {
            ss << endl << tab << TAB << "{\"return\": " << "\"" << ret_expr->unparseToString() << "\"}";
        }
        else if (can_continue)
        {
            ss << endl << tab << TAB << "{\"continue\"}";
        }
        else if (can_break)
        {
            ss << endl << tab << TAB << "{\"break\"}";
        }
        ss << "]," << endl << tab << "}," << endl;
        return ss.str();
    }
    
    string Path::to_string_as_initializer() const
    {
        stringstream ss;
        ss << "{";
        for(auto & i : codes)
        {
            if(i->t != procedure) continue;
            if(auto p = dynamic_cast<const Procedure*>(i))
            {
                ss << "\"" << p->left.var_name << "\" : ";
                if(p->right == nullptr)
                    ss << "NULL\"";
                else
                    ss << "\"" << p->right->unparseToString() << "\",";
            }
        }
        ss << "}";
        return ss.str();
    }
    
    string Path::to_code(unsigned int tab_num) const
    {
        stringstream ss;
        string tab = generate_tab(tab_num);
        ss << tab << "if(" << constraint_list.to_string() << "){" << endl;
        ss << tab << TAB << "// " << path_id << endl;
        // declare local variable
        ss << var_tbl.to_declaration_code(tab_num + 1);
        for (const auto &p : codes)
        {
            ss << p->to_code(tab_num + 1) << endl;
        }
        if (is_return)
        {
            ss << tab << TAB << "return " << ret_expr->unparseToString() << ";" << endl;
        }
        else if (can_continue)
        {
            ss << tab << TAB << "continue;" << endl;
        }
        else if (can_break)
        {
            ss << tab << TAB << "break;" << endl;
        }
        ss << tab << "}" << endl;
        return ss.str();
    }
    
    void procedure_block::add_klee_out_code(std::stringstream &ss, unsigned int tab_num) const
    {
        string tab = generate_tab(tab_num);
        for (const auto &n : this->left_real_value_name_list)
        {
            ss << tab << "klee_output(\"" << n << "\", " << n << ".value);" << endl;
        }
    }
    
    void Path::generate_procedure_block_list()
    {
        // a block consist of continues procedures, loop or code block will split two blocks.
        size_t cur_idx = 0;
        size_t codes_size = codes.size();
        for (; cur_idx < codes_size; ++cur_idx)
        {
            if (codes[cur_idx]->t == procedure)
            {
                if (procedure_block_list.empty() || procedure_block_list.back().finished)
                {
                    procedure_block tmp;
                    tmp.block_id = procedure_block_list.size();
                    tmp.begin_codes_idx = cur_idx;
                    tmp.end_codes_idx = cur_idx + 1;
                    tmp.finished = false;
                    procedure_block_list.push_back(tmp);
                }
                else
                {
                    procedure_block_list.back().end_codes_idx = cur_idx + 1;
                }
                auto p = dynamic_cast<const Procedure *>(codes[cur_idx]);
                if (p->left.is_real_type())
                {
                    procedure_block_list.back().left_real_value_name_list.push_back(p->left.var_name);
                }
            }
            else
            {
                procedure_block_list.back().finished = true;
            }
        }
        if (!procedure_block_list.empty()) procedure_block_list.back().finished = true;
    }
    
    string klee_code_include = "#include \"REAL.h\"\n"
                               "#include \"klee.h\"\n"
                               "#include \"klee_expression.h\"\n"
                               "using namespace iRRAM;\n";
    string file_prefix = "./.klee_code/";
    string file_postfix = ".cc";
    
    void Path::to_klee_code_functions(const le::VariableTable &input_parameters)
    {
        stringstream ss;
        if (codes.empty())
        {
            // empty code do nothing, only return, break or continue
//            ss << "// path: " << path_id << endl;
//            ss << "void " << path_id << input_parameters.to_parameterlist() << "{}" << endl;
            return;
        }
        generate_procedure_block_list();
        for (auto &block : procedure_block_list)
        {
            string func_name = path_id + "__" + std::to_string(block.block_id);
            ss << "// path: " << path_id << endl;
            ss << "// blcock: " << block.block_id << endl;
            ss << klee_code_include;
            ss << endl;
            ss << "void " << func_name << input_parameters.to_parameterlist() << "{" << endl;
            ss << input_parameters.to_make_real_klee_symbolic_code(1) << endl;
            ss << var_tbl.to_declaration_code(1) << endl;
            for (size_t i = block.begin_codes_idx; i < block.end_codes_idx; ++i)
            {
                ss << TAB << codes[i]->to_code() << endl;
            }
            block.add_klee_out_code(ss, 1);
            ss << "}" << endl << endl;
            // correspond main function
            ss << "int main(){" << endl;
            ss << input_parameters.to_declaration_code(1);
            ss << TAB << func_name << input_parameters.to_variables_reference_list() << ";" << endl;
            ss << TAB << "return 0;" << endl;
            ss << "}" << endl;
    
            bool tmp = write_stream_to_file(file_prefix + func_name + file_postfix, ss);
            if (!tmp) cout << "Generating klee code has failed!" << endl;
            ss.clear();
        }
        for (auto &code : codes)
        {
            if (code->t == loop)
            {
                Loop *l = (Loop *) code;
                l->to_klee_code_functions();
            }
        }
    }
    
    Path::~Path()
    {
    
    }
    
    Procedure *CodeCreater::create_procedure(const le::Variable &v, SgExpression *e)
    {
        p_list.push_back(new Procedure(v, e));
        p_i++;
        return p_list[p_i - 1];
    }
    
    CodeBlock* CodeCreater::create_codeblock()
    {
        cb_list.push_back(new CodeBlock());
        cb_list[cb_i]->_in_pool = this;
        cb_i++;
        return cb_list[cb_i - 1];
    }
    
    Loop *
    CodeCreater::create_loop(const SgStatement *stmt, Function *_func_ptr, Loop *_father_loop, int _dowhile_loop_count)
    {
        Loop *tmp = new Loop(stmt, _func_ptr, _father_loop, this, _dowhile_loop_count);
        l_list.push_back(tmp);
        l_list[l_i]->_in_pool = this;
        l_i++;
        return l_list[l_i - 1];
    }
    
    CodeCreater::~CodeCreater()
    {
        for(auto &i : p_list)
        {
            delete i;
            i = nullptr;
        }
        for(auto &i : cb_list)
        {
            delete i;
            i = nullptr;
        }
        for(auto &i : l_list)
        {
            delete i;
            i = nullptr;
        }
    }
}