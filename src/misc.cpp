#include <ncurses.h>
#include "misc.hpp"
#include <sstream>

using namespace std;

cell WINDOW_REQ;    //NOTE: The size requirements for the terminal window.

/* NOTE:
 * Name: invalid_window_size_handler
 * Purpose: Forces the user to maintain a specific size of the terminal window.
 * Parameters: None
 */
void invalid_window_size_handler () {
    uint8_t x_max,
            y_max;
    getmaxyx(stdscr, y_max, x_max);
    
    while (y_max < WINDOW_REQ.first or x_max < WINDOW_REQ.second) {
        ::clear();
        string msg1 = "The current window is too small",
               msg4 = "Resize the terminal window and press Enter twice to continue";
        stringstream msg2,
                     msg3;
        msg2 << "Required dimensions: " << WINDOW_REQ.second+0 << " x " << WINDOW_REQ.first+0;
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
}
