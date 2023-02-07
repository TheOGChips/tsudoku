#ifndef DIFFICULTYMENU_HPP
#define DIFFICULTYMENU_HPP

#include "Menu.hpp"
#include "colors.hpp"

class DifficultyMenu : public Menu {
    private:
        difficulty_level diff;
        
        void display_menu (const uint8_t, const uint8_t, const options);
        void set_difficulty_level (const difficulty_level);
        
    public:
        DifficultyMenu () {}
        ~DifficultyMenu () {}
        options menu ();
        difficulty_level get_difficulty_level ();
};

#endif
