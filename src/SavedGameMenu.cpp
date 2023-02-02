#include <ncurses.h>
#include "SavedGameMenu.hpp"
#include "colors.hpp"

using namespace std;

void SavedGameMenu::display_menu (const uint8_t Y_EDGE, const uint8_t X_EDGE, const options) {
    uint8_t display_line = Y_EDGE;
    clear();
    mvprintw(display_line++, X_EDGE, "Saved Games:");
    for (list_iter iter = saved_games.begin(); iter != saved_games.end(); iter++) {
        if (*selection == *iter) attron(COLOR_PAIR(MENU_SELECTION));
        mvprintw(++display_line, X_EDGE, "%s", iter->c_str());
        if (*selection == *iter) attroff(COLOR_PAIR(MENU_SELECTION));
    }
    refresh();
}

void SavedGameMenu::generate_saved_games_list () {
    using namespace filesystem;
    typedef directory_iterator dir_iter;
    
    for (dir_iter iter(DIR); iter != end(dir_iter()); iter++) {
        if (iter->path().extension() != ".txt") {
            saved_games.push_back(iter->path().stem().string());
        }
    }
    saved_games.sort();
}

void SavedGameMenu::select_saved_game () {
    uint16_t input,
             size_offset;
    selection = saved_games.begin();
    
    curs_set(false);
    if (saved_games.empty()) mvprintw(TOP_PADDING, LEFT_PADDING, "You have no saved games.");
    else {
        do {
            display_menu(TOP_PADDING, LEFT_PADDING, options::NONE);
            
            input = getch();
            if (input == KEY_DOWN and *selection != saved_games.back()) selection++;
            else if (input == KEY_UP and *selection != saved_games.front()) selection--;
        } while (input != KEY_ENTER);
        mvprintw(TOP_PADDING + saved_games.size() + 2, LEFT_PADDING, "You selected %s", selection->c_str());
    }
    mvprintw(TOP_PADDING + saved_games.size() + 3, LEFT_PADDING, "Press ENTER to continue...");
    refresh();
    while (getch() != KEY_ENTER);
    curs_set(true);
}

options SavedGameMenu::menu () {
    generate_saved_games_list();
    select_saved_game();
    //TODO: Read in saved game based on selection
    return options::NONE;
}