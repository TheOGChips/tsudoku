#include <ncurses.h>
#include "InGameMenu.hpp"
#include "colors.hpp"
#include <map>

void InGameMenu::display_menu (const uint8_t Y_EDGE, const uint8_t X_EDGE, const uint8_t opt) {
    const uint8_t NUM_OPTS = 3;
    const string TITLE = "IN-GAME MENU",
                 OPT[NUM_OPTS] = { "View the rules of sudoku",
                                   "See game manual", 
                                   "Save current game"};
    map<uint8_t, igm_options> opt_map;
    opt_map[0] = igm_options::RULES;
    opt_map[1] = igm_options::MANUAL;
    opt_map[2] = igm_options::SAVE_GAME;
           
    mvprintw(Y_EDGE, X_EDGE, TITLE.c_str());
    for (uint8_t i = 0; i < NUM_OPTS; i++) {
        if (igm_options(opt) == opt_map[i]) attron(COLOR_PAIR(MENU_SELECTION));
        mvprintw(Y_EDGE + IN_GAME_MENU_TITLE_SPACING + i + 1, X_EDGE, OPT[i].c_str());
        if (igm_options(opt) == opt_map[i]) attroff(COLOR_PAIR(MENU_SELECTION));
    }
    refresh();
}

void InGameMenu::clear (const uint8_t Y_EDGE, const uint8_t X_EDGE) {
    for (uint8_t y = Y_EDGE + IN_GAME_MENU_TITLE_SPACING + 5; y < getmaxy(stdscr); y++) {
        move(y, X_EDGE);
        clrtoeol();
    }
}

void InGameMenu::display_rules (const uint8_t Y_EDGE, const uint8_t X_EDGE) {
    const string TITLE = "RULES FOR PLAYING SUDOKU",
                 RULES_INTRO = string("Sudoku is a puzzle game using the numbers 1-9. The ") +
                               "puzzle board is a 9x9 grid that can be broken up evenly in 3 " +
                               "different ways: rows, columns, and 3x3 sub-grids. A completed " +
                               "sudoku puzzle is one where each cell contains a number, but with " +
                               "the following restrictions:",
                 RULES_ROWS = "1. Each row can only contain one each of the numbers 1-9",
                 RULES_COLUMNS = "2. Each column can only contain one each of the numbers 1-9",
                 RULES_SUBMATRIX = "3. Each sub-grid can only contain one each of the numbers 1-9";
                 
    const uint8_t NUM_RULES = 4;
    string rules_text[NUM_RULES] = { RULES_INTRO, RULES_ROWS, RULES_COLUMNS, RULES_SUBMATRIX };
    uint8_t display_offset = 5;
    
    mvprintw(Y_EDGE + IN_GAME_MENU_TITLE_SPACING + display_offset++, X_EDGE, TITLE.c_str());
    for (uint8_t i = 0; i < NUM_RULES; i++) {
        display_offset++;
        screen_reader(Y_EDGE, X_EDGE, rules_text[i], display_offset);
    }
}

void InGameMenu::display_manual (const uint8_t Y_EDGE, const uint8_t X_EDGE) {
    const string TITLE = "TSUDOKU GAME MANUAL",
                 MANUAL_INTRO = string("Red numbers are hints provided for you when the puzzle ") +
                                "has been generated. The number of hints present corresponds to " +
                                "the difficulty level you have chosen. Cells with white '?' " +
                                "symbols are empty cells which you must solve for to complete " +
                                "the puzzle. To enter a number 1-9 into an empty cell, simply " +
                                "move the cursor over to an empty cell and type the number. The " +
                                "'?' symbol will be replaced with the number you entered, which " +
                                "will be green in color. To remove a number from one of these " +
                                "cells, move the cursor over the cell and press either the " +
                                "Backspace or Delete keys; the green number will be replaced " +
                                "with a '?' symbol again. The eight blank cells surrounding each " +
                                "sudoku puzzle cell are available as a note-taking area when " +
                                "analyzing what numbers should go in that particular cell; " +
                                "numbers entered in these cells will appear yellow in color. " +
                                "Numbers in these cells can also be removed by pressing either " +
                                "the Backspace or Delete keys while the cursor is over the cell. " +
                                "You cannot enter anything in the note-taking cells surrounding " +
                                "puzzle cells with red numbers. BEWARE: Entering a number in a " +
                                "'?' occupied cell will also erase your notes in the eight " +
                                "surrounding cells. This action cannot be undone.",
                 MANUAL_M = "m/M -> Enter/Exit the in-game manual",
                 MANUAL_Q = "q/Q -> Quit the game without saving",
                 MANUAL_DIR_KEYS = "Up, Down, Left, Right -> Navigate the sudoku board",
                 MANUAL_NUM = "1-9 -> Places number in cell highlighted by cursor",
                 MANUAL_ENTER = "Enter -> Evaluate the puzzle. Analysis will appear below puzzle.";
                 
    const uint8_t NUM_MANUAL = 6;
    string manual_text[NUM_MANUAL] = { MANUAL_INTRO, MANUAL_M, MANUAL_Q, MANUAL_DIR_KEYS,
                                       MANUAL_NUM, MANUAL_ENTER };
    uint8_t display_offset = 5;
    
    mvprintw(Y_EDGE + IN_GAME_MENU_TITLE_SPACING + display_offset++, X_EDGE, TITLE.c_str());
    for (uint8_t i = 0; i < NUM_MANUAL; i++) {
        display_offset++;
        screen_reader(Y_EDGE, X_EDGE, manual_text[i], display_offset);
    }
}

void InGameMenu::screen_reader (const uint8_t Y_EDGE, const uint8_t X_EDGE, string str,
                                uint8_t& display_offset) {
    string display_str;
    while (not str.empty()) {
        size_t space_pos = str.find_first_of(' ');
        
        if (space_pos == string::npos) {
            display_str += str;
            str.clear();
            mvprintw(Y_EDGE + IN_GAME_MENU_TITLE_SPACING + display_offset++, X_EDGE,
                     display_str.c_str());
        }
        else if (display_str.size() + space_pos + 1 < IN_GAME_MENU_DISPLAY_SPACING - 3) {
            display_str += str.substr(0, space_pos + 1);
            str = str.substr(space_pos + 1);
        }
        else {
            mvprintw(Y_EDGE + IN_GAME_MENU_TITLE_SPACING + display_offset++, X_EDGE,
                     display_str.c_str());
            display_str.clear();
        }
    }
    display_str.clear();
}

uint8_t InGameMenu::menu () {    
    curs_set(0);
    igm_options opt = igm_options::RULES;
    uint16_t input;
    do {
        refresh();
        display_menu(TOP_PADDING, IN_GAME_MENU_LEFT_EDGE, static_cast<uint8_t>(opt));
        input = getch();
        switch (input) {
            case KEY_DOWN:  ++opt;
                            break;
                           
            case KEY_UP:    --opt;
                            break;
                            
            //TODO: Case for Enter key
            case KEY_ENTER: if (opt == igm_options::RULES) {
                                clear(TOP_PADDING, IN_GAME_MENU_LEFT_EDGE);
                                display_rules(TOP_PADDING, IN_GAME_MENU_LEFT_EDGE);
                            }
                            else if (opt == igm_options::MANUAL) {
                                clear(TOP_PADDING, IN_GAME_MENU_LEFT_EDGE);
                                display_manual(TOP_PADDING, IN_GAME_MENU_LEFT_EDGE);
                            }
                            else {  //NOTE: opt will never be NONE based on this logic
                                //TODO: Saving games
                            }
            default:;
        }
    } while (tolower(input) != 'm');
    opt = igm_options::NONE;
    display_menu(TOP_PADDING, IN_GAME_MENU_LEFT_EDGE, static_cast<uint8_t>(opt));
    curs_set(1);
    
    return static_cast<uint8_t>(opt);
}

igm_options operator ++ (igm_options& opt) {
    return opt = (opt == igm_options::RULES) ? igm_options::MANUAL : igm_options::SAVE_GAME;
}

igm_options operator -- (igm_options& opt) {
    return opt = (opt == igm_options::SAVE_GAME) ? igm_options::MANUAL : igm_options::RULES;
}
