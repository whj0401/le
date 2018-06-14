//
// Created by whj on 6/11/18.
//

#include "Variable.h"
#include "common.h"

namespace le
{
    using namespace std;
    
    const int T_REAL = 10001;
    
    const string REAL_MANGLED = "iRRAM__scope__class_REAL";
    
    string VariableTable::to_string() const
    {
        stringstream ss;
        ss << "{";
        for(const auto &v : T)
        {
            v.second.add_to_stringstream(ss);
            ss << ", ";
        }
        ss << "}";
        return ss.str();
    }
    
    string VariableTable::to_parameterlist() const
    {
        if (T.empty()) return "()";
        stringstream ss;
        ss << "(";
        unsigned long idx = 0;
        unsigned long last_idx = T.size() - 1;
        for (const auto &v : T)
        {
            ss << v.second.type_node->unparseToString() << " " << v.second.var_name;
            if (idx == last_idx) break;
            idx++;
            ss << ", ";
        }
        ss << ")";
        return ss.str();
    }
    
    string VariableTable::to_declaration_code(unsigned int tab_num) const
    {
        stringstream ss;
        string tab = generate_tab(tab_num);
        for (const auto &v : T)
        {
            ss << tab << v.second.to_declaration() << endl;
        }
        return ss.str();
    }
}

