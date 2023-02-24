#include <ncurses.h>
#include "misc.hpp"
#include <sstream>
#include <thread>
#include <chrono>

using namespace std;

cell WINDOW_REQ;    //NOTE: The size requirements for the terminal window.

/* NOTE:
 * Name: invalid_window_size_handler
 * Purpose: Enforce window size on initial startup if terminal window is not already compliant. The
 *          user will be updated as to whether the window is the correct size or not after pressing
 *          the Enter key twice. The reason the Enter key must be hit twice is actually a bug I
 *          decided to make a feature. For some reason, it's required to hit twice only in this
 *          section. Since it doesn't affect anything else, I just left it alone. I now suspect it
 *          has something to do with how NCurses handles window resizing, and might not be fixable
 *          anyway.
 * Parameters: None
 */
bool invalid_window_size_handler () {
    uint8_t y_max,
            x_max;
    getmaxyx(stdscr, y_max, x_max);
    
    if (y_max == WINDOW_REQ.first and x_max == WINDOW_REQ.second) {
        return false;
    }
    else {
        do {
            clear();
            string msg1 = "The current window is too small",
                   msg4 = "Resize the terminal window and press Enter twice to continue";
            stringstream msg2,
                         msg3;
            msg2 << "Required dimensions: " << WINDOW_REQ.second+0 << " x "
                                            << WINDOW_REQ.first+0;
            msg3 << "Current dimensions: " << x_max+0 << " x " << y_max+0;
            mvprintw(y_max/2,     x_max/2 - msg1.size()/2,       "%s", msg1.c_str());
            mvprintw(y_max/2 + 2, x_max/2 - msg2.str().size()/2, "%s", msg2.str().c_str());
            mvprintw(y_max/2 + 3, x_max/2 - msg3.str().size()/2, "%s", msg3.str().c_str());
            
            if (msg4.size() > x_max) {
                const uint8_t PARTITION = 30;
                mvprintw(y_max/2 + 5, x_max/2 - msg4.substr(0, PARTITION).size()/2,
                         "%s", msg4.substr(0, PARTITION).c_str());
                mvprintw(y_max/2 + 6, x_max/2 - msg4.substr(PARTITION).size()/2,
                         "%s", msg4.substr(PARTITION).c_str());
            }
            else mvprintw(y_max/2 + 5, x_max/2 - msg4.size()/2,  "%s", msg4.c_str());
            
            refresh();
            this_thread::sleep_for(chrono::milliseconds(100));
            getmaxyx(stdscr, y_max, x_max);
        } while (y_max != WINDOW_REQ.first or x_max != WINDOW_REQ.second);
        clear();
        
        return true;
    }
}
