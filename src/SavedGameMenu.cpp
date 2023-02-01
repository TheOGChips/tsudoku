#include <ncurses.h>
#include "SavedGameMenu.hpp"
#include "colors.hpp"

using namespace std;

void SavedGameMenu::display_menu (const uint8_t Y_EDGE, const uint8_t X_EDGE, const options) {
    uint8_t display_line = Y_EDGE;
    uint16_t input;
    curs_set(false);
    clear();
    mvprintw(display_line++, X_EDGE, "Saved Games:");
    //TODO: Add an edge case for saved games list being empty
    
    typedef list<string>::iterator list_iter;
    list_iter selection = saved_games.begin();
    //uint8_t count = 0;
    do {
        static const uint8_t LIST_DISPLAY_START = display_line;
        
        for (list_iter iter = saved_games.begin(); iter != saved_games.end(); iter++) {
            if (*selection == *iter) attron(COLOR_PAIR(MENU_SELECTION));
            mvprintw(++display_line, X_EDGE, "%s", iter->c_str());
            if (*selection == *iter) attroff(COLOR_PAIR(MENU_SELECTION));
        }
        refresh();
        
        input = getch();
        if (input == KEY_DOWN and *selection != saved_games.back()) {
            selection++;
            display_line = LIST_DISPLAY_START;
        }
        else if (input == KEY_UP and *selection != saved_games.front()) {
            selection--;
            display_line = LIST_DISPLAY_START;
        }
        else if (input != KEY_ENTER) display_line = LIST_DISPLAY_START;
    } while (input != KEY_ENTER);
    
    //TODO: Try to split this function up into select and display functions
    mvprintw(++display_line, X_EDGE, "You selected %s", selection->c_str());
    mvprintw(++display_line, X_EDGE, "Press ENTER to continue...");
    refresh();
    while (getch() != KEY_ENTER);
    curs_set(true);
}

void SavedGameMenu::generate_saved_games_list () {
    using namespace filesystem;
    typedef directory_iterator dir_iter;
    
    for (dir_iter iter(DIR); iter != end(dir_iter()); iter++) {
        if (iter->path().extension() != ".txt") {
            saved_games.push_back(iter->path().stem().string());
        }
    }
}
/*
void SavedGameMenu::input () {
    uint16_t input = getch();
    switch (input) {
        case KEY_DOWN:
        case KEY_UP:
    }
}
*/
options SavedGameMenu::menu () {
    generate_saved_games_list();
    display_menu(TOP_PADDING, LEFT_PADDING, options::NONE);
    return options::NONE;
}
