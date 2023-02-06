#include <ncurses.h>
#include "DifficultyMenu.hpp"
#include <map>

using namespace std;

difficulty_level operator ++ (difficulty_level& diff, int) {
    switch (diff) {
        case difficulty_level::EASY: diff = difficulty_level::MEDIUM;
                                     break;
                                     
        case difficulty_level::MEDIUM: diff = difficulty_level::HARD;
                                       break;
                                       
        case difficulty_level::HARD: diff = difficulty_level::EXPERT;
                                     break;
                                     
        default: diff = difficulty_level::EXPERT;
    }
    return diff;
}

difficulty_level operator -- (difficulty_level& diff, int) {
    switch (diff) {
        case difficulty_level::EXPERT: diff = difficulty_level::HARD;
                                     break;
                                     
        case difficulty_level::HARD: diff = difficulty_level::MEDIUM;
                                       break;
                                       
        case difficulty_level::MEDIUM: diff = difficulty_level::EASY;
                                     break;
                                     
        default: diff = difficulty_level::EASY;
    }
    return diff;
}

void DifficultyMenu::display_menu (const uint8_t Y_EDGE, const uint8_t X_EDGE, const options) {
    const uint8_t NUM_DIFFS = 4;
    const string TITLE = "CHOOSE DIFFICULTY SETTING",
                 DIFFS[NUM_DIFFS] = { "Easy", "Medium", "Hard", "Expert"};
    
    map<uint8_t, difficulty_level> diff_map;
    diff_map[0] = difficulty_level::EASY;
    diff_map[1] = difficulty_level::MEDIUM;
    diff_map[2] = difficulty_level::HARD;
    diff_map[3] = difficulty_level::EXPERT;
           
    mvprintw(Y_EDGE, X_EDGE, "%s", TITLE.c_str());
    for (uint8_t i = 0; i < NUM_DIFFS; i++) {
        if (diff == diff_map[i]) attron(COLOR_PAIR(MENU_SELECTION));
        mvprintw(Y_EDGE + i + 2, X_EDGE, "%s", DIFFS[i].c_str());
        if (diff == diff_map[i]) attroff(COLOR_PAIR(MENU_SELECTION));
    }
    refresh();
}

void DifficultyMenu::set_difficulty_level (const difficulty_level diff) {
    this->diff = diff;
}

options DifficultyMenu::menu () {
    curs_set(false);
    diff = difficulty_level::EASY;
    uint16_t input;
    do {
        refresh();
        display_menu(TOP_PADDING, LEFT_PADDING, options::NONE);
        input = getch();
        switch (input) {
            case KEY_DOWN: diff++;
                           break;
                           
            case KEY_UP: diff--;
                         break;
                         
            default:;
        }
    } while (input != KEY_ENTER);
    curs_set(true);
    
    return options::NONE;
}

difficulty_level DifficultyMenu::get_difficulty_level () {
    return diff;
}
