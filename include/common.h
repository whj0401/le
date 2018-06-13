//
// Created by whj on 6/13/18.
//

#ifndef CODE_ANALYSE_COMMON_H
#define CODE_ANALYSE_COMMON_H

#include <string>
#include <sstream>
#include "rose.h"

namespace le
{
    extern const std::string TAB;
    
    inline std::string generate_tab(unsigned int tab_num)
    {
        std::stringstream ss;
        for(int i = 0; i < tab_num; ++i) ss << TAB;
        return ss.str();
    }
    
    template <class T>
    T* clone(const SgNode* node)
    {
        if(node == nullptr) return nullptr;
        SgTreeCopy copy;
        return dynamic_cast<T*>(node->copy(copy));
    }
    
    SgVarRefExp* get_func_call_lhs(SgFunctionCallExp* func_call);
    
    inline bool isAtomStatement(const SgStatement *stmt)
    {
        return dynamic_cast<const SgForStatement *>(stmt) ||
               dynamic_cast<const SgWhileStmt *>(stmt) ||
               dynamic_cast<const SgDoWhileStmt *>(stmt) ||
               dynamic_cast<const SgIfStmt *>(stmt) ||
               dynamic_cast<const SgSwitchStatement *>(stmt);
    }
}

#endif //CODE_ANALYSE_COMMON_H
