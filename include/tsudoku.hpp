#ifndef TSUDOKU_HPP
#define TSUDOKU_HPP

#include "misc.hpp"

//NOTE: This file keeps a persistent record of how many games the player has successfully finished
const std::string COMPLETED = DIR + "/completed_puzzles.txt";
enum class err_msg { INVALID_ARG, TOO_MANY_ARGS };  //NOTE: Possible types of errors that can be
                                                    //      generated
void create_dir ();
void display_completed_puzzles ();
void print_help ();
void print_err_msg (err_msg);
void delete_saved_games ();

#endif
