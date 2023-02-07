#ifndef COLORS_HPP
#define COLORS_HPP

#include <string>   //std::string

enum colors { UNKNOWN = 1, GIVEN, CANDIDATES_Y, CANDIDATES_B, GUESS, MENU_SELECTION };
const std::string color_code[6] = { "n", "u", "r", "y", "b", "g" };

#endif
