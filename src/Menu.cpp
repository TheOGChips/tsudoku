#include <ncurses.h>
#include "Menu.hpp"
#include "colors.hpp"
#include <sstream>
#include <map>

Menu::Menu (const menu_type type) {
    this->type = type;
    if (this->type == menu_type::MAIN) {
        initscr();
        cbreak();   //TODO: Will need to account for signal handling
        noecho();
        keypad(stdscr, true);
        
        start_color();
        init_pair(MENU_SELECTION, COLOR_BLACK, COLOR_WHITE);
    }
}

Menu::~Menu () {
    if (type == menu_type::MAIN) {
        echo();
        nocbreak();
        endwin();
    }
}

void Menu::display_main_menu (const uint8_t Y, const uint8_t X, main_options opt) {
    const string TITLE = "MAIN MENU";
    const uint8_t Y_CENTER = Y / 2,
                  X_CENTER = X/2 - TITLE.size()/2,
                  NUM_OPTS = 3;
    const string OPT[NUM_OPTS] = { "New Game", "Resume Game", "Show # Finished Games" };
    map<uint8_t, main_options> opt_map;
    opt_map[0] = main_options::NEW_GAME;
    opt_map[1] = main_options::RESUME_GAME;
    opt_map[2] = main_options::SHOW_STATS;
           
    mvprintw(Y_CENTER - 2, X_CENTER, TITLE.c_str());
    for (uint8_t i = 0; i < NUM_OPTS; i++) {
        if (opt == opt_map[i]) attron(COLOR_PAIR(MENU_SELECTION));
        mvprintw(Y_CENTER + i, X_CENTER, OPT[i].c_str());
        if (opt == opt_map[i]) attroff(COLOR_PAIR(MENU_SELECTION));
    }
    refresh();
}

void Menu::display_in_game_menu (const uint8_t Y_EDGE, const uint8_t X_EDGE, in_game_options opt) {
    const uint8_t NUM_OPTS = 3;
    const string TITLE = "IN-GAME MENU",
                 OPT[NUM_OPTS] = { "View the rules of sudoku",
                                   "See game manual", 
                                   "Save current game"};
    map<uint8_t, in_game_options> opt_map;
    opt_map[0] = in_game_options::RULES;
    opt_map[1] = in_game_options::MANUAL;
    opt_map[2] = in_game_options::SAVE_GAME;
           
    mvprintw(Y_EDGE, X_EDGE, TITLE.c_str());
    for (uint8_t i = 0; i < NUM_OPTS; i++) {
        if (opt == opt_map[i]) attron(COLOR_PAIR(MENU_SELECTION));
        mvprintw(Y_EDGE + IN_GAME_MENU_TITLE_SPACING + i + 1, X_EDGE, OPT[i].c_str());
        if (opt == opt_map[i]) attroff(COLOR_PAIR(MENU_SELECTION));
    }
    refresh();
}

void Menu::clear (const uint8_t Y_EDGE, const uint8_t X_EDGE) {
    for (uint8_t y = Y_EDGE + IN_GAME_MENU_TITLE_SPACING + 5; y < getmaxy(stdscr); y++) {
        move(y, X_EDGE);
        clrtoeol();
    }
}

void Menu::display_rules (const uint8_t Y_EDGE, const uint8_t X_EDGE) {
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

void Menu::display_manual (const uint8_t Y_EDGE, const uint8_t X_EDGE) {
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

void Menu::screen_reader (const uint8_t Y_EDGE, const uint8_t X_EDGE, string str,
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

main_options Menu::main_menu () {
    uint8_t x_max,
            y_max;
    getmaxyx(stdscr, y_max, x_max);
    while (y_max < WINDOW_YMIN or x_max < WINDOW_XMIN) {
        uint8_t x_curr,
                y_curr;
        ::clear();
        string msg1 = "The current window is too small",
               msg4 = "Resize the terminal window and press Enter twice to continue";
        stringstream msg2,
                     msg3;
        msg2 << "Required dimensions: " << WINDOW_XMIN+0 << " x " << WINDOW_YMIN+0;
        msg3 << "Current dimensions: " << x_max+0 << " x " << y_max+0;
        mvprintw(y_max/2,     x_max/2 - msg1.size()/2,       msg1.c_str());
        mvprintw(y_max/2 + 2, x_max/2 - msg2.str().size()/2, msg2.str().c_str());
        mvprintw(y_max/2 + 3, x_max/2 - msg3.str().size()/2, msg3.str().c_str());
        mvprintw(y_max/2 + 5, x_max/2 - msg4.size()/2,       msg4.c_str());
        refresh();
        getmaxyx(stdscr, y_max, x_max);
        while (getch() != KEY_ENTER);   //NOTE: For some reason, the Enter key needs to be pressed
    }                                   //      twice here
    ::clear();
    
    string msg1 = "The window is now an appropriate size",
           msg2 = "Press Enter to continue";
    mvprintw(y_max/2,     x_max/2 - msg1.size()/2, msg1.c_str());
    mvprintw(y_max/2 + 1, x_max/2 - msg2.size()/2, msg2.c_str());
    refresh();
    while (getch() != KEY_ENTER);
    ::clear();
    
    curs_set(0);
    main_options opt = main_options::NEW_GAME;
    display_main_menu(y_max, x_max, opt);
    
    uint16_t input;
    do {
        input = getch();
        switch (input) {
            case KEY_DOWN:  display_main_menu(y_max, x_max, ++opt);
                            break;
                            
            case KEY_UP:    display_main_menu(y_max, x_max, --opt);
                            break;
                
            default:;
        }
    } while (input != KEY_ENTER);
    ::clear();
    curs_set(1);
    return opt;
}

void Menu::in_game_menu () {    
    curs_set(0);
    in_game_options opt = in_game_options::RULES;
    uint16_t input;
    do {
        refresh();
        display_in_game_menu(TOP_PADDING, IN_GAME_MENU_LEFT_EDGE, opt);
        input = getch();
        switch (input) {
            case KEY_DOWN:  ++opt;
                            break;
                           
            case KEY_UP:    --opt;
                            break;
                            
            //TODO: Case for Enter key
            case KEY_ENTER: if (opt == in_game_options::RULES) {
                                clear(TOP_PADDING, IN_GAME_MENU_LEFT_EDGE);
                                display_rules(TOP_PADDING, IN_GAME_MENU_LEFT_EDGE);
                            }
                            else if (opt == in_game_options::MANUAL) {
                                clear(TOP_PADDING, IN_GAME_MENU_LEFT_EDGE);
                                display_manual(TOP_PADDING, IN_GAME_MENU_LEFT_EDGE);
                            }
                            else {  //NOTE: opt will never be NONE based on this logic
                                //TODO: Saving games
                            }
            default:;
        }
    } while (tolower(input) != 'm');
    display_in_game_menu(TOP_PADDING, IN_GAME_MENU_LEFT_EDGE, in_game_options::NONE);
    curs_set(1);
}

main_options operator ++ (main_options& opt) {
    return opt = (opt == main_options::NEW_GAME) ? main_options::RESUME_GAME : main_options::SHOW_STATS;
}

main_options operator -- (main_options& opt) {
    return opt = (opt == main_options::SHOW_STATS) ? main_options::RESUME_GAME : main_options::NEW_GAME;
}

in_game_options operator ++ (in_game_options& opt) {
    return opt = (opt == in_game_options::RULES) ? in_game_options::MANUAL : in_game_options::SAVE_GAME;
}

in_game_options operator -- (in_game_options& opt) {
    return opt = (opt == in_game_options::SAVE_GAME) ? in_game_options::MANUAL : in_game_options::RULES;
}
