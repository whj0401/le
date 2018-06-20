//
// Created by whj on 6/11/18.
//

#include "Path.h"
#include "Loop.h"
#include "REAL_function_list.h"

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
    }
    
    string Path::to_string(unsigned int tab_num) const
    {
        stringstream ss;
        string tab = generate_tab(tab_num);
    
        ss << tab << "{ #" << path_id << endl;
        ss << tab << TAB << "\"variables\": " << var_tbl.to_string() << "," << endl;
        ss << tab << TAB << "\"constraint\": [" << constraint_list.to_string() << "]," << endl;
        ss << tab << TAB << "\"path\": [";
        for (const auto &p : codes)
        {
            ss << p->to_string(tab_num + 1);
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