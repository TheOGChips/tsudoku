#include <ncurses.h>
#include "SavedGameMenu.hpp"

void SavedGameMenu::display_menu (const uint8_t Y_EDGE, const uint8_t X_EDGE, const options) {
    uint8_t display_line = Y_EDGE;
    clear();
    mvprintw(display_line++, X_EDGE, "Saved Games:");
    
    mvprintw(++display_line, X_EDGE, "Press ENTER to continue...");
    refresh();
    while (getch() != KEY_ENTER);
}

options SavedGameMenu::menu () {
    display_menu(TOP_PADDING, LEFT_PADDING, options::NONE);
    return options::NONE;
}
