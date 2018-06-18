#include <iostream>
#include "rose.h"
#include "Program.h"
#include "REAL_function_list.h"

int main(int argc, char** argv)
{
    le::init_operator_str_map();
    le::init_member_set();
    le::init_relative_set();
    le::run_project(argc, argv);
    return 0;
}