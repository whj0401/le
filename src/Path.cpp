//
// Created by whj on 6/11/18.
//

#include "Path.h"
#include "Loop.h"

namespace le
{
    using namespace std;
    
    CodeCreater paths_pool;
    
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
        else
            ss << "[\"" << left.var_name << "\", \"" << right->unparseToString() << "\"]";
        ss << endl << tab << "},";
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
        
        ss << tab << "{" << endl;
        ss << tab << TAB << "\"variables\": " << var_tbl.to_string() << "," << endl;
        ss << tab << TAB << "\"constraint\": [" << constraint_list.to_string() << "]," << endl;
        ss << tab << TAB << "\"path\": [";
        for (const auto p : codes)
        {
            ss << p->to_string(tab_num + 1);
        }
        if (is_return)
        {
            ss << tab << TAB << "{\"return\": " << "\"" << ret_expr->unparseToString() << "\"}";
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
    
    Path::~Path()
    {
    
    }
    
    Procedure* CodeCreater::create_procedure(const le::Variable &v, const SgExpression *e)
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
    
    Loop* CodeCreater::create_loop(const SgStatement *stmt, Function* _func_ptr, Loop* _father_loop)
    {
        Loop * tmp = new Loop(stmt, _func_ptr, _father_loop);
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