#include <ncurses.h>
#include "SavedGameMenu.hpp"
#include "colors.hpp"
#include <fstream>

using namespace std;

/* NOTE:
 * Name: display_menu (pure virtual override)
 * Purpose: Displays the saved games menu. The options listed are saved games in CSV files from the
 *          ~/.tsudoku directory. The currently selected option is always highlighted. The saved
 *          games menu is re-rendered each time the player uses the Up/Down keys to highlight a
 *          different option.
 * Parameters:
 *      EDGE -> Starting cell the saved games menu will display at. The menu title should display on
 *              the line below the top padding and the column after the vertical divider.
 *      (unused options enum) -> Unused variable with a named reference. Required because of the
 *                               function prototype inherited from Menu.
 */
void SavedGameMenu::display_menu (const cell EDGE, const options) {
    uint8_t display_line = EDGE.first;
    clear();
    mvprintw(display_line++, EDGE.second, "Saved Games:");
    for (list_iter iter = saved_games.begin(); iter != saved_games.end(); iter++) {
        if (*selection == *iter) attron(COLOR_PAIR(MENU_SELECTION));
        mvprintw(++display_line, EDGE.second, "%s", iter->c_str());
        if (*selection == *iter) attroff(COLOR_PAIR(MENU_SELECTION));
    }
    refresh();
}

/* NOTE:
 * Name: generate_saved_games_list
 * Purpose: Creates the list of saved games from the names of available CSV files in the ~/.tsudoku
 *          directory. This entries in this list are what will be displayed to the player. The
 *          entries are stored without the ".csv" file extension. Text files with extension ".txt"
 *          are ignored so as to avoid adding the completed games file to the list.
 * Parameters: None
 */
void SavedGameMenu::generate_saved_games_list () {
    using namespace filesystem; //NOTE: Use C++17's filesystem library
    typedef directory_iterator dir_iter;
    
    for (dir_iter iter(DIR); iter != end(dir_iter()); iter++) {
        if (iter->path().extension() == ".csv") saved_games.push_back(iter->path().stem().string());
    }
    saved_games.sort();
}

/* NOTE:
 * Name: select_saved_game
 * Purpose: Controls iterating through the list from player input and highlighting the name of the
 *          current game that will be loaded once the player presses Enter. If there are no saved
 *          games, the player will instead be notified as much and then prompted to continue.
 * Parameters: None
 */
bool SavedGameMenu::select_saved_game () {
    uint16_t input,
             size_offset;
    selection = saved_games.begin();
    
    curs_set(false);    //NOTE: Turn off cursor while in the menu.
    if (saved_games.empty()) {
        mvprintw(TOP_PADDING, LEFT_PADDING, "You have no saved games.");
        mvprintw(TOP_PADDING + saved_games.size() + 3, LEFT_PADDING, "Press ENTER to continue...");
        while (getch() != KEY_ENTER);
    }
    else {
        timeout(250);
        do {
            display_menu(cell {TOP_PADDING, LEFT_PADDING}, options::NONE);
            
            input = getch();
            if (input == KEY_DOWN and *selection != saved_games.back()) selection++;
            else if (input == KEY_UP and *selection != saved_games.front()) selection--;
            else invalid_window_size_handler();
        } while (input != KEY_ENTER);
        nodelay(stdscr, false);
    }
    refresh();
    curs_set(true);     //NOTE: Turn cursor back on before leaving the menu.
        
    return not saved_games.empty();
}

/* NOTE:
 * Name: read_saved_game
 * Purpose: Reads a saved game from it's CSV file to the saved game and color code matrices.
 * Parameters: None
 */
void SavedGameMenu::read_saved_game () {
    ifstream infile (DIR + "/" + *selection + ".csv");
    for (uint8_t i = 0; i < DISPLAY_MATRIX_ROWS; i++) {
        string row;
        getline(infile, row);
        for (uint8_t j = 0; j < DISPLAY_MATRIX_COLUMNS; j++) {
            size_t index;
            saved_game[i][j] = stoi(row, &index);   //NOTE: Read in number
            saved_color_codes[i][j] = row[index];   //NOTE: Read in color code character
            
            //NOTE: Drop over to next entry in string. Index will cause a thrown out_of_range exception on the last number in the string.
            try { row = row.substr(index + 2); }
            catch (const out_of_range) { row = row.substr(index + 1); }
        }
    }
    infile.close();
}

#if DEBUG
/* NOTE:
 * Name: print_saved_game
 * Purpose: Prints the matrices of saved game values and color codes side by side. This is only
 *          available when the program has been compiled with debug mode enabled.
 * Parameters: None
 */
void SavedGameMenu::print_saved_game () {
    clear();
    for (uint8_t i = 0; i < DISPLAY_MATRIX_ROWS; i++) {
        for (uint8_t j = 0; j < DISPLAY_MATRIX_COLUMNS; j++) {
            mvprintw(TOP_PADDING + i, LEFT_PADDING + j, "%c", saved_game[i][j]);
            mvprintw(TOP_PADDING + i, LEFT_PADDING + j + 30, "%c", saved_color_codes[i][j]);
        }
    }
    refresh();
    getch();
}
#endif

/* NOTE:
 * Name: get_saved_game
 * Purpose: Wraps the selected saved game value and color code matrices into a SavedPuzzle object
 *          which is returned to the calling function. This makes passing around the saved game
 *          information easier.
 * Parameters: None
 */
SavedPuzzle SavedGameMenu::get_saved_game () {
    SavedPuzzle saved_puzzle;
    for (uint8_t i = 0; i < DISPLAY_MATRIX_ROWS; i++) {
        for (uint8_t j = 0; j < DISPLAY_MATRIX_COLUMNS; j++) {
            saved_puzzle.puzzle[i][j] = saved_game[i][j];
            saved_puzzle.color_codes[i][j] = saved_color_codes[i][j];
        }
    }
    saved_puzzle.filename = DIR + "/" + *selection + ".csv";
    return saved_puzzle;
}

/* NOTE:
 * Name: menu (overload)
 * Purpose: Coordinates generating the saved games list, displaying the list to the player in menu
 *          form, and reading in the saved game chosen by the player.
 * Parameters: None
 */
options SavedGameMenu::menu () {
    generate_saved_games_list();
    if (select_saved_game()) {
        read_saved_game();
        #if DEBUG
            print_saved_game();
        #endif
        return options::SAVE_READY;
    }
    else return options::NO_SAVES;
}
