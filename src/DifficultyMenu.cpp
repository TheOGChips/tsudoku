#include <ncurses.h>
#include "DifficultyMenu.hpp"
#include <map>

using namespace std;

/* NOTE:
 * Name: Operator ++ (post-increment, overloaded)
 * Purpose: Shorthand convenience for changing difficulty options.
 * Parameters:
 *      opt -> The previously highlighted difficulty option to update.
 */
difficulty_level operator ++ (difficulty_level& diff, int) {
    switch (diff) {
        case difficulty_level::EASY:
            diff = difficulty_level::MEDIUM;
            break;
                                     
        case difficulty_level::MEDIUM:
            diff = difficulty_level::HARD;
            break;
                                       
        case difficulty_level::HARD:
            diff = difficulty_level::EXPERT;
            break;
                                     
        default: diff = difficulty_level::EXPERT;
    }
    return diff;
}

/* NOTE:
 * Name: Operator -- (post-decrement, overloaded)
 * Purpose: Shorthand convenience for changing difficulty options.
 * Parameters:
 *      opt -> The previously highlighted difficulty option to update.
 */
difficulty_level operator -- (difficulty_level& diff, int) {
    switch (diff) {
        case difficulty_level::EXPERT:
            diff = difficulty_level::HARD;
            break;
                                     
        case difficulty_level::HARD:
            diff = difficulty_level::MEDIUM;
            break;
                                       
        case difficulty_level::MEDIUM:
            diff = difficulty_level::EASY;
            break;
                                     
        default: diff = difficulty_level::EASY;
    }
    return diff;
}

/* NOTE:
 * Name: display_menu (pure virtual override)
 * Purpose: Displays the difficulty menu. The currently selected option is always highlighted. The
 *          difficulty menu is re-rendered each time the user uses the Up/Down keys to highlight a
 *          different option.
 * Parameters:
 *      EDGE -> Starting cell the difficulty menu will display at. The menu title should display at
 *              the origin.
 *      (unused options enum) -> Unused variable with a named reference. Required because of the
 *                               function prototype inherited from Menu.
 */
void DifficultyMenu::display_menu (const cell EDGE, const options) {
    const uint8_t NUM_DIFFS = 4;
    const string TITLE = "CHOOSE DIFFICULTY SETTING",
                 DIFFS[NUM_DIFFS] = { "Easy", "Medium", "Hard", "Expert" };
    
    map<uint8_t, difficulty_level> diff_map;
    diff_map[0] = difficulty_level::EASY;
    diff_map[1] = difficulty_level::MEDIUM;
    diff_map[2] = difficulty_level::HARD;
    diff_map[3] = difficulty_level::EXPERT;
           
    mvprintw(EDGE.first, EDGE.second, "%s", TITLE.c_str());
    for (uint8_t i = 0; i < NUM_DIFFS; i++) {
        if (diff == diff_map[i]) attron(COLOR_PAIR(MENU_SELECTION));
        mvprintw(EDGE.first + i + 2, EDGE.second, "%s", DIFFS[i].c_str());
        if (diff == diff_map[i]) attroff(COLOR_PAIR(MENU_SELECTION));
    }
    refresh();
}

/* NOTE:
 * Name: set_difficulty_level
 * Purpose: Records the difficulty level the user has chosen to start a new game.
 * Parameters:
 *      DIFF -> The chosen difficulty level.
 */
void DifficultyMenu::set_difficulty_level (const difficulty_level DIFF) {
    diff = DIFF;
}

/* NOTE:
 * Name: menu (overload)
 * Purpose: Controls the menu display and difficulty level recording.
 * Parameters: None
 */
options DifficultyMenu::menu () {
    curs_set(false);
    diff = difficulty_level::EASY;
    uint16_t input;
    do {
        refresh();
        display_menu(cell {TOP_PADDING, LEFT_PADDING}, options::NONE);
        input = getch();
        switch (input) {
            case KEY_DOWN:
                diff++;
                break;
                           
            case KEY_UP:
                diff--;
                break;
                         
            default:;
        }
    } while (input != KEY_ENTER);
    curs_set(true);
    
    return options::NONE;
}

/* NOTE:
 * Name: get_difficulty_level
 * Purpose: Returns the difficulty level the user has chosen to start the new game.
 * Parameters: None
 */
difficulty_level DifficultyMenu::get_difficulty_level () {
    return diff;
}
