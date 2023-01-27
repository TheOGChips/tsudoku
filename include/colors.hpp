#ifndef COLORS_HPP
#define COLORS_HPP

#include <string>   //std::string

//TODO: Make this an enum
const uint8_t UNKNOWN = 1,
              GIVEN = 2,
              CANDIDATES = 3,
              GUESS = 4,
              MENU_SELECTION = 5;
const std::string color_code[5] = { "n", "u", "r", "y", "g"};

#endif
