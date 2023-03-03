#ifndef DIFFICULTYMENU_HPP
#define DIFFICULTYMENU_HPP

#include "Menu.hpp"
#include "colors.hpp"

/* NOTE:
 * Class: DifficultyMenu
 * Purpose: Menu-derived class that displays the names of difficulty levels before starting a new
 *          game and records the user's choice.
 * Private variables:
 *      diff -> The difficulty setting chosen for the current game (if the user chose to start a new
 *              game).
 */
class DifficultyMenu : public Menu {
    private:
        difficulty_level diff;
        
        void display_menu (const cell, const options);
        void set_difficulty_level (const difficulty_level);
        
    public:
        DifficultyMenu () {}    //NOTE: Default constructor. Does nothing.
        ~DifficultyMenu () {}   //NOTE: Default destructor. Does nothing.
        options menu ();
        difficulty_level get_difficulty_level ();
};

#endif
