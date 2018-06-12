//
// Created by whj on 6/11/18.
//

#include "Path.h"

namespace le
{
    using namespace std;
    
    const string TAB = "    ";
    
    CodeCreater paths_pool;
    
    string Procedure::to_string(unsigned int tab_num) const
    {
        stringstream ss;
        if(right == nullptr)
            ss << "[\"" << left.var_name << "\", " << "NULL";
        else
            ss << "[\"" << left.var_name << "\", \"" << right->unparseToString() << "\"]";
        return ss.str();
    }
    
    string CodeBlock::to_string(unsigned int tab_num) const
    {
        stringstream ss;
        string tab = "";
        for (int i = 0; i < tab_num; ++i)
            tab += TAB;
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
    
    string Path::to_string(unsigned int tab_num) const
    {
        stringstream ss;
        string tab = "";
        for (int i = 0; i < tab_num; ++i)
            tab += TAB;
        
        ss << tab << "{" << endl;
        ss << tab << TAB << "\"variables\": " << var_tbl.to_string() << endl;
        ss << tab << TAB << "\"constraint\": [" << constraint_list.to_string() << "]," << endl;
        ss << tab << TAB << "\"content\": [";
        for (const auto p : codes)
        {
            ss << p->to_string() << ", ";
        }
        if (is_return)
        {
            ss << "[\"return\": " << "\"" << ret_expr->unparseToString() << "\"]";
        }
        ss << "]," << endl << tab << "}," << endl;
        return ss.str();
    }
    
    Path::~Path()
    {
    
    }
}