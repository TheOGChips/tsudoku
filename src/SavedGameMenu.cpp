#include <ncurses.h>
#include "SavedGameMenu.hpp"

using namespace std;

void SavedGameMenu::display_menu (const uint8_t Y_EDGE, const uint8_t X_EDGE, const options) {
    uint8_t display_line = Y_EDGE;
    clear();
    mvprintw(display_line++, X_EDGE, "Saved Games:");
    
    for (list<string>::iterator iter = saved_games.begin(); iter != saved_games.end(); iter++) {
        mvprintw(++display_line, X_EDGE, "%s", iter->c_str());
    }
    
    mvprintw(++display_line, X_EDGE, "Press ENTER to continue...");
    refresh();
    while (getch() != KEY_ENTER);
}

void SavedGameMenu::generate_saved_games_list () {
    using namespace filesystem;
    directory_iterator dir_iter(DIR);
    for (; dir_iter != end(directory_iterator()); dir_iter++) {
        saved_games.push_back(dir_iter->path().stem().string());
    }
    //TODO: Remove "completed_puzzles.txt" from the list
}

options SavedGameMenu::menu () {
    generate_saved_games_list();
    display_menu(TOP_PADDING, LEFT_PADDING, options::NONE);
    return options::NONE;
}
