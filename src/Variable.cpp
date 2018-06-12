//
// Created by whj on 6/11/18.
//

#include "Variable.h"

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
}

