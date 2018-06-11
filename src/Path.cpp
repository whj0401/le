//
// Created by whj on 6/11/18.
//

#include "Path.h"

namespace le
{
    using namespace std;
    
    const string TAB = "    ";
    
    string Procedure::to_string(unsigned int tab_num)
    {
        stringstream ss;
        ss << "[\"" << left.var_name << "\", \"" << right->unparseToString() << "\"]";
        return ss.str();
    }
    
    string CodeBlock::to_string(unsigned int tab_num)
    {
        stringstream ss;
        string tab = "";
        for (int i = 0; i < tab_num; ++i)
            tab += TAB;
        for (auto s : L)
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
    
    void Path::add_procedure(le::Variable v, SgExpression *e)
    {
        Procedure tmp(v, e);
        procedures.push_back(tmp);
    }
    
    string Path::to_string(unsigned int tab_num)
    {
        stringstream ss;
        string tab = "";
        for (int i = 0; i < tab_num; ++i)
            tab += TAB;
        
        ss << tab << "{" << endl;
        ss << tab << TAB << "\"constraint\": [" << constraint_list.to_string() << "]," << endl;
        ss << tab << TAB << "\"content\": [";
        for (auto p : procedures)
        {
            ss << p.to_string() << ", ";
        }
        if (is_return)
        {
            ss << "[\"return\": " << "\"" << ret_expr->unparseToString() << "\"";
        }
        ss << "]," << endl << tab << "}," << endl;
        return ss.str();
    }
}