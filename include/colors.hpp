#ifndef COLORS_HPP
#define COLORS_HPP

#include <string>   //std::string

/* NOTE: This file contains constants used for identifying NCurses color codes.
 * 
 *  colors -> enum constants used for identifying NCurses COLOR_PAIRs when printing to the screen
 *  color_code -> array used to identify NCurses COLOR_PAIRs when writing/reading a saved game to
 *                from a file
 */
enum colors { UNKNOWN = 1, GIVEN, CANDIDATES_Y, CANDIDATES_B, GUESS, MENU_SELECTION };
const std::string color_code[6] = { "n", "u", "r", "y", "b", "g" };

#endif
