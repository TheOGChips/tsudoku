#ifndef COLORS_HPP
#define COLORS_HPP

#include <string>   //std::string

using namespace std;

const string RED = "\e[1;31m",
             WHITE = "\e[0m";
const uint8_t UNKNOWN = 1,
              KNOWN = 2,
              GUESS = 3;

void set_print_color (const string);

#endif