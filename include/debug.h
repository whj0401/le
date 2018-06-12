//
// Created by whj on 6/12/18.
//

#ifndef CODE_ANALYSE_DEBUG_H
#define CODE_ANALYSE_DEBUG_H

#include <iostream>
#include <string>
#include <sstream>
#include "rose.h"

namespace le
{
    using namespace std;
    
    void print_statement(SgStatement* s)
    {
        cout << s->unparseToString() << endl;
    }
    
    void print_err_statement(SgStatement* stmt)
    {
        cout << "Unsupported statement: " << endl;
        cout << stmt->class_name() << " : " << stmt->unparseToString() << endl;
    }
    
    void print_whole_node(SgNode* s, stringstream& ss, unsigned int tab_num = 0)
    {
        auto list = s->get_traversalSuccessorContainer();
        for(int i = 0; i < tab_num; ++i)
            ss << "\t";
        ss << s->unparseToString() << " : " << s->class_name() << endl;
        for(auto i : list)
        {
            print_whole_node(i, ss, tab_num + 1);
        }
    }
    
    void print_whole_node(SgNode* s)
    {
        stringstream ss;
        print_whole_node(s, ss);
        cout << ss.str() << endl;
    }
    
    void print_err_use_variable_without_declaration(const string &var_name, const Path &path)
    {
        cout << "no correspond variable in this path" << endl;
        cout << "reference name: " << var_name << endl;
        cout << "path: " << path.to_string() << endl;
    }
    
    void print_warning_function_no_definition(SgFunctionDeclaration* decl)
    {
        cout << decl->unparseToString() << " has no definition block!" << endl;
    }
}

#endif //CODE_ANALYSE_DEBUG_H
