#include <ncurses.h>
#include "Menu.hpp"
#include <csignal>

/* NOTE:
 * Name: Class Constructor
 * Purpose: Sets a signal handler for signal SIGINT. All derived classes will be aware of this.
 * Parameters: None
 */
Menu::Menu () {
    signal(SIGINT, SIGINT_handler);
}

/* NOTE:
 * Name: SIGINT_handler
 * Purpose: Resets the terminal settings to their previous state from before the NCurses environment
 *          was initialized. This function is inherited by all its derived classes.
 * Parameters:
 *      (unused 32-bit integer) -> The signal being caught by this handler, in this case SIGINT.
 *                                 Since it's value is never used, it does not need a name for
 *                                 reference.
 */
void Menu::SIGINT_handler (int32_t) {
    curs_set(true);
    echo();
    nocbreak();
    endwin();
    exit(EXIT_SUCCESS);
}
