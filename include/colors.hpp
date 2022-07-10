#ifndef COLORS_HPP
#define COLORS_HPP

#include <string>   //std::string

using namespace std;

const string RED = "\033[1;31m",
             WHITE = "\033[0m";

void set_print_color (const string);

#endif