#include <ncurses.h>
#include "MainMenu.hpp"
#include "colors.hpp"
#include <sstream>
#include <map>

using namespace std;

/* NOTE:
 * Name: Operator ++ (pre-increment, overloaded)
 * Purpose: Shorthand convenience for changing main menu options.
 * Parameters:
 *      opt -> The previously selected main menu option to update.
 */
options operator ++ (options& opt) {
    switch (opt) {
        case options::NEW_GAME:
            opt = options::RESUME_GAME;
            break;
                                     
        case options::RESUME_GAME:
            opt = options::SHOW_STATS;
            break;
                                       
        case options::SHOW_STATS:
            opt = options::EXIT;
            break;
                                     
        default: opt = options::EXIT;
    }
    return opt;
}

/* NOTE:
 * Name: Operator -- (pre-decrement, overloaded)
 * Purpose: Shorthand convenience for changing main menu options.
 * Parameters:
 *      opt -> The previously selected main menu option to update.
 */
options operator -- (options& opt) {
    switch (opt) {
        case options::EXIT:
            opt = options::SHOW_STATS;
            break;
                                     
        case options::SHOW_STATS:
            opt = options::RESUME_GAME;
            break;
                                       
        case options::RESUME_GAME:
            opt = options::NEW_GAME;
            break;
                                     
        default: opt = options::NEW_GAME;
    }
    return opt;
}

/* NOTE:
 * Name: Class Constructor
 * Purpose: Initializes the NCurses environment and global NCurses settings.
 * Parameters: None
 */
MainMenu::MainMenu () {
    initscr();
    cbreak();   //TODO: Will need to account for signal handling
    noecho();
    keypad(stdscr, true);
    
    start_color();
    init_pair(MENU_SELECTION, COLOR_BLACK, COLOR_WHITE);
}

/* NOTE:
 * Name: Class Destructor
 * Purpose: Unsets the NCurses environment once the user chooses to exit the program.
 * Parameters: None
 */
MainMenu::~MainMenu () {
    echo();
    nocbreak();
    endwin();
}

/* NOTE:
 * Name: display_menu (pure virtual override)
 * Purpose: Displays the main menu. The currently selected option is always highlighted. The main
 *          menu is re-rendered each time the user uses the Up/Down keys to highlight a different
 *          option.
 * Parameters:
 *      MAX -> Bottom right corner cell of the terminal window. Signifies the max number of lines
 *             and columns in the window.
 *      OPT -> The currently selected main menu option.
 */
void MainMenu::display_menu (const cell MAX, const options OPT) {
    const string TITLE = "MAIN MENU";
    const uint8_t Y_CENTER = MAX.first/2,
                  X_CENTER = MAX.second/2 - TITLE.size()/2,
                  NUM_OPTS = 4;
    const string OPTS[NUM_OPTS] = { "New Game", "Resume Game", "Show # Finished Games", "Exit" };
    map<uint8_t, options> opt_map;
    opt_map[0] = options::NEW_GAME;
    opt_map[1] = options::RESUME_GAME;
    opt_map[2] = options::SHOW_STATS;
    opt_map[3] = options::EXIT;
           
    mvprintw(Y_CENTER - 2, X_CENTER, "%s", TITLE.c_str());
    for (uint8_t i = 0; i < NUM_OPTS; i++) {
        if (OPT == opt_map[i]) attron(COLOR_PAIR(MENU_SELECTION));
        mvprintw(Y_CENTER + i, X_CENTER, "%s", OPTS[i].c_str());
        if (OPT == opt_map[i]) attroff(COLOR_PAIR(MENU_SELECTION));
    }
    refresh();
}

/* NOTE:
 * Name: set_VERTICAL_DIVIDER
 * Purpose: Sets the value of the vertical divider between the display puzzle and the in-game menu.
 * Parameters:
 *      VAL -> The number of columns to be used as the vertical divider. If the in-game menu is
 *             disabled, the vertical divider is set to 0.
 */
void MainMenu::set_VERTICAL_DIVIDER (const uint8_t VAL) {
    VERTICAL_DIVIDER = VAL;
}

/* NOTE:
 * Name: set_IN_GAME_MENU_DISPLAY_SPACING
 * Purpose: Sets the number of columns used to display the in-game menu.
 * Parameters:
 *      VAL -> The number of columns to be used to display the in-game menu. If the in-game menu is
 *             disabled, the number of columns is set to 0.
 */
void MainMenu::set_IN_GAME_MENU_DISPLAY_SPACING (const uint8_t VAL) {
    IN_GAME_MENU_DISPLAY_SPACING = VAL;
}

/* NOTE:
 * Name: set_WINDOW_XMIN
 * Purpose: Sets the minimum number of columns the terminal window must be in order to play. This
 *          size is dependent on whether the in-game menu is enabled.
 * Parameters: None
 */
void MainMenu::set_WINDOW_XMIN () {
    WINDOW_XMIN = LEFT_PADDING + PUZZLE_SPACE + VERTICAL_DIVIDER + IN_GAME_MENU_DISPLAY_SPACING +
                  RIGHT_PADDING;
}

/* NOTE:
 * Name: menu (overload)
 * Purpose: Sets class-level global variables before running the actual main menu.
 * Parameters:
 *      USE_IN_GAME_MENU -> Boolean indicating whether the in-game menu is disabled or not. This
 *                          affects the enforced size of the terminal window.
 */
options MainMenu::menu (const bool USE_IN_GAME_MENU) {
    if (not USE_IN_GAME_MENU) {
        set_VERTICAL_DIVIDER(0);
        set_IN_GAME_MENU_DISPLAY_SPACING(0);
    }
    set_WINDOW_XMIN();
    
    return menu();
}

/* NOTE:
 * Name: menu (pure virtual override)
 * Purpose: On the first call, determines whether the terminal window meets the size requirments. If
 *          not, the program does not proceed until the user resizes the terminal window
 *          appropriately. Afterwards and on subsequent calls, displays the main menu.
 * Parameters: None
 */
options MainMenu::menu () {
    uint8_t x_max,
            y_max;
    static bool first_pass = true;
    getmaxyx(stdscr, y_max, x_max);
    curs_set(false);    //NOTE: Turn the cursor off while in the main menu.

    /* NOTE: Enforce window size on initial startup if terminal window is not already compliant. The
     *       user will be updated as to whether the window is the correct size or not after pressing
     *       the Enter key twice. The reason the Enter key must be hit twice is actually a bug I
     *       decided to make a feature. For some reason, it's required to hit twice only in this
     *       section. Since it doesn't affect anything else, I just left it alone.
     */
    if (first_pass) {
        while (y_max < WINDOW_YMIN or x_max < WINDOW_XMIN) {
            ::clear();
            string msg1 = "The current window is too small",
                   msg4 = "Resize the terminal window and press Enter twice to continue";
            stringstream msg2,
                         msg3;
            msg2 << "Required dimensions: " << WINDOW_XMIN+0 << " x " << WINDOW_YMIN+0;
            msg3 << "Current dimensions: " << x_max+0 << " x " << y_max+0;
            mvprintw(y_max/2,     x_max/2 - msg1.size()/2,       "%s", msg1.c_str());
            mvprintw(y_max/2 + 2, x_max/2 - msg2.str().size()/2, "%s", msg2.str().c_str());
            mvprintw(y_max/2 + 3, x_max/2 - msg3.str().size()/2, "%s", msg3.str().c_str());
            mvprintw(y_max/2 + 5, x_max/2 - msg4.size()/2,       "%s", msg4.c_str());
            refresh();
            getmaxyx(stdscr, y_max, x_max);
            while (getch() != KEY_ENTER);   //NOTE: For some reason, the Enter key needs to be
        }                                   //      pressed twice here
        ::clear();
        
        string msg1 = "The window is now an appropriate size",
            msg2 = "Press Enter to continue";
        mvprintw(y_max/2,     x_max/2 - msg1.size()/2, "%s", msg1.c_str());
        mvprintw(y_max/2 + 1, x_max/2 - msg2.size()/2, "%s", msg2.c_str());
        refresh();
        while (getch() != KEY_ENTER);
        first_pass = false;
    }
    ::clear();
    
    //NOTE: Display the main menu starting with the "New Game" option highlighted
    options opt = options::NEW_GAME;
    display_menu(cell {y_max, x_max}, opt);
    
    //NOTE: Cycle through the menu until the user selects an option
    uint16_t input;
    do {
        input = getch();
        switch (input) {
            case KEY_DOWN:
                ++opt;
                display_menu(cell {y_max, x_max}, opt);
                break;
                            
            case KEY_UP:
                --opt;
                display_menu(cell {y_max, x_max}, opt);
                break;
                
            default:;
        }
    } while (input != KEY_ENTER);
    
    ::clear();
    curs_set(true); //NOTE: Turn the cursor back on before leaving the main menu
    return opt;
}
