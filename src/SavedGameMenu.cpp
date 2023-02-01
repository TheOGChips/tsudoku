#include <ncurses.h>
#include "SavedGameMenu.hpp"

using namespace std;

void SavedGameMenu::display_menu (const uint8_t Y_EDGE, const uint8_t X_EDGE, const options) {
    uint8_t display_line = Y_EDGE;
    clear();
    mvprintw(display_line++, X_EDGE, "Saved Games:");
    
    typedef list<string>::iterator list_iter;
    for (list_iter iter = saved_games.begin(); iter != saved_games.end(); iter++) {
        mvprintw(++display_line, X_EDGE, "%s", iter->c_str());
    }
    
    mvprintw(++display_line, X_EDGE, "Press ENTER to continue...");
    refresh();
    while (getch() != KEY_ENTER);
}

void SavedGameMenu::generate_saved_games_list () {
    using namespace filesystem;
    typedef directory_iterator dir_iter;
    
    for (dir_iter iter(DIR); iter != end(directory_iterator()); iter++) {
        saved_games.push_back(iter->path().stem().string());
    }
    //TODO: Remove "completed_puzzles.txt" from the list
}

options SavedGameMenu::menu () {
    generate_saved_games_list();
    display_menu(TOP_PADDING, LEFT_PADDING, options::NONE);
    return options::NONE;
}
