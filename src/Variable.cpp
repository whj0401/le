//
// Created by whj on 6/11/18.
//

#include "Variable.h"

namespace le
{
    using namespace std;
    
    const int T_REAL = 10001;
    
    string VariableTable::to_string()
    {
        stringstream ss;
        ss << "{";
        for(auto v : T)
        {
            v.second.add_to_stringstream(ss);
            ss << ", ";
        }
        ss << "}";
        return ss.str();
    }
}

