#include <ncurses.h>
#include "InGameMenu.hpp"
#include "colors.hpp"
#include <map>      //std::map
#include <fstream>  //std::ofstream, std::ifstream

using namespace std;

/* NOTE:
 * Name: Operator ++ (post-increment, overloaded)
 * Purpose: Shorthand convenience for changing in-game menu options.
 * Parameters:
 *      opt -> The previously highlighted in-game menu option to update.
 *      (unused int) -> Tells the compiler to overload the post-increment operator.
 */
options operator ++ (options& opt, int) {
    return opt = (opt == options::RULES) ? options::MANUAL : options::SAVE_GAME;
}

/* NOTE:
 * Name: Operator -- (post-decrement, overloaded)
 * Purpose: Shorthand convenience for changing in-game menu options.
 * Parameters:
 *      opt -> The previously highlighted in-game option to update.
 *      (unused int) -> Tells the compiler to overload the post-decrement operator.
 */
options operator -- (options& opt, int) {
    return opt = (opt == options::SAVE_GAME) ? options::MANUAL : options::RULES;
}

/* NOTE:
 * Name: Class Constructor
 * Purpose: Initializes the display matrix, so the in-game menu can track any changes made during
 *          gameplay.
 * Parameters:
 *      display_matrix -> The display matrix from the main gameplay loop in Sudoku.
 */
InGameMenu::InGameMenu (uint8_t display_matrix[DISPLAY_MATRIX_ROWS][DISPLAY_MATRIX_COLUMNS]) {
    for (uint8_t i = 0; i < DISPLAY_MATRIX_COLUMNS; i++) {
        this->display_matrix[i] = display_matrix[i];
    }
}

/* NOTE:
 * Name: display_menu (pure virtual override)
 * Purpose: Displays the in-game menu. The currently selected option is always highlighted. The
 *          in-game menu is re-rendered each time the user uses the Up/Down keys to highlight a
 *          different option.
 * Parameters:
 *      EDGE -> Starting cell the in-game menu will display at. The menu title should display on the
 *              line below the top padding and the column after the vertical divider.
 *      OPT -> The currently highlighted main menu option.
 */
void InGameMenu::display_menu (const cell EDGE, const options OPT) {
    const uint8_t NUM_OPTS = 3;
    const string TITLE = "IN-GAME MENU",
                 OPTS[NUM_OPTS] = { "View the rules of sudoku",
                                    "See game manual", 
                                    "Save current game" };
    map<uint8_t, options> opt_map;
    opt_map[0] = options::RULES;
    opt_map[1] = options::MANUAL;
    opt_map[2] = options::SAVE_GAME;
           
    mvprintw(EDGE.first, EDGE.second, "%s", TITLE.c_str());
    for (uint8_t i = 0; i < NUM_OPTS; i++) {
        if (OPT == opt_map[i]) attron(COLOR_PAIR(MENU_SELECTION));
        mvprintw(EDGE.first + IN_GAME_MENU_TITLE_SPACING + i + 1, EDGE.second,
                 "%s", OPTS[i].c_str());
        if (OPT == opt_map[i]) attroff(COLOR_PAIR(MENU_SELECTION));
    }
    refresh();
}

/* NOTE:
 * Name: clear (NCurses library function overload)
 * Purpose: Clears the in-game menu display area.
 * Parameters:
 *      EDGE -> Line and column to start clearing from.
 */
void InGameMenu::clear (const cell EDGE) {
    for (uint8_t y = EDGE.first + IN_GAME_MENU_TITLE_SPACING + NUM_OPTS + 2; y < getmaxy(stdscr);
         y++) {
        move(y, EDGE.second);
        clrtoeol();
    }
}

/* NOTE:
 * Name: display_rules
 * Purpose: Displays the rules of sudoku.
 * Parameters:
 *      EDGE -> Line and column to start the display at.
 */
void InGameMenu::display_rules (const cell EDGE) {
    const string TITLE = "RULES FOR PLAYING SUDOKU",
                 RULES_INTRO = string("Sudoku is a puzzle game using the numbers 1-9. The ") +
                               "puzzle board is a 9x9 grid that can be broken up evenly in 3 " +
                               "different ways: rows, columns, and 3x3 boxes. A completed sudoku " +
                               "puzzle is one where each cell contains a number, but with the " +
                               "following restrictions:",
                 RULES_ROWS = "1. Each row can only contain one each of the numbers 1-9",
                 RULES_COLUMNS = "2. Each column can only contain one each of the numbers 1-9",
                 RULES_SUBMATRIX = "3. Each 3x3 box can only contain one each of the numbers 1-9";
                 
    const uint8_t NUM_RULES = 4;
    string rules_text[NUM_RULES] = { RULES_INTRO, RULES_ROWS, RULES_COLUMNS, RULES_SUBMATRIX };
    uint8_t display_offset = NUM_OPTS + 2;  //NOTE: Offset to allow the display to start below the
                                            //      list of menu options
    
    mvprintw(EDGE.first + IN_GAME_MENU_TITLE_SPACING + display_offset++, EDGE.second,
             "%s", TITLE.c_str());
    for (uint8_t i = 0; i < NUM_RULES; i++) {
        display_offset++;
        screen_reader(EDGE, rules_text[i], display_offset);
    }
}

/* NOTE:
 * Name: display_manual
 * Purpose: Displays the tsudoku game manual.
 * Parameters:
 *      EDGE -> Line and column to start the display at.
 */
void InGameMenu::display_manual (const cell EDGE) {
    const string TITLE = "TSUDOKU GAME MANUAL",
                 MANUAL_INTRO = string("Red numbers are givens provided for you when the puzzle ") +
                                "has been generated. The number of givens present corresponds to " +
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
                                "analyzing what numbers (candidates) should go in that " +
                                "particular cell; numbers entered in these cells will appear " +
                                "yellow in color. Numbers in these cells can also be removed by " +
                                "pressing either the Backspace or Delete keys while the cursor " +
                                "is over the cell. You cannot enter anything in the note-taking " +
                                "cells surrounding puzzle cells with red numbers. BEWARE: " +
                                "Entering a number in a '?' occupied cell will also erase your " +
                                "notes in the eight surrounding cells. This action cannot be " +
                                "undone.",
                 MANUAL_M = "m/M -> Enter/Exit the in-game manual",
                 MANUAL_Q = "q/Q -> Quit the game without saving",
                 MANUAL_DIR_KEYS = "Up, Down, Left, Right -> Navigate the sudoku board",
                 MANUAL_NUM = "1-9 -> Places number in cell highlighted by cursor",
                 MANUAL_ENTER = "Enter -> Evaluate the puzzle. Analysis will appear below puzzle.";
                 
    const uint8_t NUM_MANUAL = 6;
    string manual_text[NUM_MANUAL] = { MANUAL_INTRO, MANUAL_M, MANUAL_Q, MANUAL_DIR_KEYS,
                                       MANUAL_NUM, MANUAL_ENTER };
    uint8_t display_offset = NUM_OPTS + 2;  //NOTE: Offset to allow the display to start below the
                                            //      list of menu options
    
    mvprintw(EDGE.first + IN_GAME_MENU_TITLE_SPACING + display_offset++, EDGE.second,
             "%s", TITLE.c_str());
    for (uint8_t i = 0; i < NUM_MANUAL; i++) {
        display_offset++;
        screen_reader(EDGE, manual_text[i], display_offset);
    }
}

/* NOTE:
 * Name: screen_reader
 * Purpose: Prints string of text to the screen in a nicely formatted way that makes it easy to
 *          read. This function parses the input string by spaces and determines when to start
 *          printing on a new line (i.e. if adding another word would overlap with the window border
 *          and ruin the sudoku board display).
 * Parameters:
 *      EDGE -> Line and column to start the display at.
 *      str -> Input string to be printed in the in-game menu's display area.
 *      display_offset -> Line offset to allow displaying correctly below the in-game menu title and
 *                        options.
 */
void InGameMenu::screen_reader (const cell EDGE, string str, uint8_t& display_offset) {
    string display_str;
    while (not str.empty()) {
        size_t space_pos = str.find_first_of(' ');  //NOTE: Index of the next space character in str
        
        //NOTE: If the end of the string has been reached, print what remains
        if (space_pos == string::npos) {
            display_str += str;
            str.clear();
            mvprintw(EDGE.first + IN_GAME_MENU_TITLE_SPACING + display_offset++, EDGE.second,
                     "%s", display_str.c_str());
        }
        
        //NOTE: If there is enough space to display the next word, add it to the display string
        else if (display_str.size() + space_pos + 1 < IN_GAME_MENU_DISPLAY_SPACING) {
            display_str += str.substr(0, space_pos + 1);
            str = str.substr(space_pos + 1);
        }
        
        /* NOTE: Otherwise, print the display string on the current line, clear the string, and 
         *       increment the display offset.
         */
        else {  
            mvprintw(EDGE.first + IN_GAME_MENU_TITLE_SPACING + display_offset++, EDGE.second,
                     "%s", display_str.c_str());
            display_str.clear();
        }
    }
    display_str.clear();
}

/* NOTE:
 * Name: save_game
 * Purpose: Prompts the user for the name to save the game under before saving the game. Displays a
 *          success message after having saved.
 * Parameters:
 *      EDGE -> Line and column to start the display at. The prompt for the save file name will
 *              start here.
 */
void InGameMenu::save_game (const cell EDGE) {
    uint8_t display_offset = NUM_OPTS + 2;
    mvprintw(EDGE.first + IN_GAME_MENU_TITLE_SPACING + display_offset++, EDGE.second,
             "Enter save file name: ");
    
    curs_set(true);
    mvprintw(EDGE.first + IN_GAME_MENU_TITLE_SPACING + ++display_offset, EDGE.second,
             "%s saved!", save_game(display_matrix).c_str());
    curs_set(false);
}

/* NOTE:
 * Name: save_game (static overload)
 * Purpose: Performs the actual saving of the current game to a CSV file in $HOME/.tsudoku. Each
 *          cell is saved as its integer representation follow by a letter representing the
 *          foreground color of the character.
 * Parameters:
 *      display_matrix -> The display matrix as seen in the terminal window.
 */
string InGameMenu::save_game (uint8_t* display_matrix[DISPLAY_MATRIX_COLUMNS]) {
    const uint8_t NAME_SIZE = 18;   //NOTE: NAME_SIZE limited by window width requirements of no
    char name[NAME_SIZE];           //      in-game menu mode
    nodelay(stdscr, false);
    echo();
    getnstr(name, NAME_SIZE - 1);   //TODO: When resizing, this will save the game as ".csv"
    noecho();
    nodelay(stdscr, true);
    
    const string FILENAME = DIR + "/" + name + ".csv";
    ofstream outfile;
    outfile.open(FILENAME.c_str());
    for (uint8_t i = 0; i < DISPLAY_MATRIX_ROWS; i++) {
        for (uint8_t j = 0; j < DISPLAY_MATRIX_COLUMNS; j++) {
            outfile << static_cast<uint16_t>(display_matrix[i][j]);
            chtype ch = mvinch(i + ORIGINy + i / CONTAINER_SIZE, j + ORIGINx + j / CONTAINER_SIZE);
            switch (ch & A_COLOR) {
                case COLOR_PAIR(UNKNOWN):
                    outfile << color_code[UNKNOWN];
                    break;
                                          
                case COLOR_PAIR(GIVEN):
                    outfile << color_code[GIVEN];
                    break;
                                        
                case COLOR_PAIR(CANDIDATES_Y):
                    outfile << color_code[CANDIDATES_Y];
                    break;
                                               
                case COLOR_PAIR(CANDIDATES_B):
                    outfile << color_code[CANDIDATES_B];
                    break;
                                             
                case COLOR_PAIR(GUESS):
                    outfile << color_code[GUESS];
                    break;
                                        
                default: outfile << color_code[0];
            }
            if (j < DISPLAY_MATRIX_COLUMNS - 1) outfile << ",";
        }
        outfile << endl;
    }
    outfile.close();
    
    return string(name);
}

void InGameMenu::set_window_resized (const bool WINDOW_RESIZED) {
    window_resized = WINDOW_RESIZED;
}

/* NOTE:
 * Name: menu (overload)
 * Purpose: Controls the menu display based on the option chosen by the user.
 * Parameters: None
 */
options InGameMenu::menu () {
    curs_set(false);    //NOTE: Turn off the cursor while in the in-game menu
    set_window_resized(false);
    options opt = options::RULES;
    int16_t input;  //NOTE: Signed needed because getch can return ERR=-1 on timeout
    //NOTE: No timeout needed because that is set by the in-game menu
    do {
        refresh();
        display_menu(cell {TOP_PADDING, IN_GAME_MENU_LEFT_EDGE}, opt);
        input = getch();
        switch (input) {
            case KEY_DOWN:
                opt++;
                break;
                           
            case KEY_UP:
                opt--;
                break;
                            
            case KEY_ENTER: 
                switch (opt) {
                    case options::RULES:
                        clear(cell {TOP_PADDING, IN_GAME_MENU_LEFT_EDGE});
                        display_rules(cell {TOP_PADDING, IN_GAME_MENU_LEFT_EDGE});
                        break;
                                             
                    case options::MANUAL:
                        clear(cell {TOP_PADDING, IN_GAME_MENU_LEFT_EDGE});
                        display_manual(cell{TOP_PADDING, IN_GAME_MENU_LEFT_EDGE});
                        break;
                                              
                    case options::SAVE_GAME:
                        clear(cell {TOP_PADDING, IN_GAME_MENU_LEFT_EDGE});
                        //NOTE: Turn off highlighted option while entering in save name
                        display_menu(cell {TOP_PADDING, IN_GAME_MENU_LEFT_EDGE}, options::NONE);
                        save_game(cell {TOP_PADDING, IN_GAME_MENU_LEFT_EDGE});
                        break;
                    
                    default:;   //NOTE: opt will never be NONE based on this logic
                }
            default:
                if (invalid_window_size_handler()) {
                    set_window_resized(true);
                    mvprintw(TOP_PADDING, LEFT_PADDING, "Press 'm' to restore the game");
                }
        }
    } while (tolower(input) != 'm');
    opt = options::NONE;
    display_menu(cell {TOP_PADDING, IN_GAME_MENU_LEFT_EDGE}, opt);
    curs_set(true); //NOTE: Turn the cursor back on before returning to the game
    
    return opt;
}

bool InGameMenu::get_window_resized () const {
    return window_resized;
}
