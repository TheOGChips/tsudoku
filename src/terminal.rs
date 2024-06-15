/// Redefines the value of the Enter key because the pre-defined NCurses value doesn't seem to work.
pub const KEY_ENTER: i32 = '\n' as i32;

/// A cell in the terminal display.
#[derive(Eq, Hash, PartialEq, Copy, Clone)]
pub struct Cell (u8, u8);
impl Cell {
    /**
     * Returns a new Cell representing a cell in the terminal display. The terminal origin is at (0, 0).
     *
     *      row -> the row number of the Cell
     *      column -> the column number of the Cell
     */
    pub const fn new (row: u8, column: u8) -> Self {
        Self(row, column)
    }

    /// Returns the y-coordinate of the Cell
    pub fn y (&self) -> u8 {
        self.row_no()
    }

    /// Returns the x-coordinate of the Cell
    pub fn x (&self) -> u8 {
        self.col_no()
    }

    /// Returns the row number of the Cell
    pub const fn row_no (&self) -> u8 {
        self.0
    }

    /// Returns the column number of the Cell
    pub const fn col_no (&self) -> u8 {
        self.1
    }

    /**
     * Changes the coordinates of the Cell.
     *
     *      row -> the row number of the Cell
     *      column -> the column number of the Cell
     */
    pub fn set (&mut self, row: u8, column: u8) {
        self.0 = row;
        self.1 = column;
    }
}

/**
 * Handles the parameters involving how everything is formatted in the terminal display and error
 * message that is displayed when the terminal window isn't the proper size.
 */
pub mod display {
    use super::Cell;
    use signal_hook::{
        consts::SIGINT,
        low_level::register,
    };
    use ncurses::{
        curs_set,
        CURSOR_VISIBILITY,
        echo,
        nocbreak,
        endwin,
        getmaxyx,
        stdscr,
        clear,
        mvprintw,
        refresh,
    };
    use std::{
        thread::sleep,
        time::Duration,
    };

    /// Sudoku puzzle display origin coordinates (top left cell)
    pub const ORIGIN: Cell = Cell::new(3, 6);

    /// Empty space between the top of the terminal window and the in-terminal display.
    pub const TOP_PADDING: u8 = ORIGIN.row_no();

    /// Empty space between the left side of the terminal window and the in-terminal display.
    pub const LEFT_PADDING: u8 = ORIGIN.col_no();

    /**
     * Minimum number of lines and columns required to properly display the terminal (27 for all cells
     * of the display matrix, 2 for border lines/columns between boxes).
     */
    pub const PUZZLE_SPACE: u8 = 29;

    /// The size requirements for the terminal window.
    pub static mut WINDOW_REQ: Cell = Cell(0, 0);

    /**
     * Empty space between puzzle and the in-game menu. Disabling the in-game menu changes the default
     * value.
     */
    pub static mut VERTICAL_DIVIDER: u8 = 4;

    /**
     * Number of columns to be used for the in-game menu display area. Disabling the in-game menu
     * changes the default value.
     */
    pub static mut IN_GAME_MENU_DISPLAY_SPACING: u8 = 80;

    pub const DISPLAY_MATRIX_ROWS: usize = 27;
    pub const DISPLAY_MATRIX_COLUMNS: usize = DISPLAY_MATRIX_ROWS;

    /**
     * Sets the value of the vertical divider between the display puzzle and the in-game menu.
     *
     *      n -> The number of columns to be used as the vertical divider. If the in-game menu is
     *           disabled, the vertical divider is set to 0.
     */
    pub unsafe fn set_VERTICAL_DIVIDER (n: u8) {
        VERTICAL_DIVIDER = n;
    }

    /**
     * Sets the number of columns used to display the in-game menu.
     *
     *      n -> The number of columns to be used to display the in-game menu. If the in-game menu is
     *           disabled, the number of columns is set to 0.
     */
    pub unsafe fn set_IN_GAME_MENU_DISPLAY_SPACING (n: u8) {
        IN_GAME_MENU_DISPLAY_SPACING = n;
    }

    /**
     * Enforce window size on initial startup if terminal window is not already compliant. The user will
     * be updated as to whether the window is the correct size or not after pressing the Enter key
     * twice. The reason the Enter key must be hit twice is actually a bug I decided to make a feature.
     * For some reason, it's required to hit twice only in this section. Since it doesn't affect
     * anything else, I just left it alone. I now suspect it has something to do with how NCurses
     * handles window resizing, and might not be fixable anyway.
     */
    pub fn invalid_window_size_handler () -> bool {
        let _ = unsafe {
            register(SIGINT, || SIGINT_handler())
        }.expect("Error: Signal not found");

        let mut y_max: i32 = 0;
        let mut x_max: i32 = 0;
        getmaxyx(stdscr(), &mut y_max, &mut x_max);
        
        unsafe {
            if (y_max == WINDOW_REQ.y() as i32 && x_max == WINDOW_REQ.x() as i32) {
                return false
            }
            while y_max != WINDOW_REQ.y() as i32 || x_max != WINDOW_REQ.x() as i32 {
                clear();
                let msg1: &str = "The current window size is incorrect.";
                let msg2: String = format!("Required dimensions: {} x {}",
                                            WINDOW_REQ.x(), WINDOW_REQ.y());
                let msg3: String = format!("Current dimensions:  {} x {}", x_max, y_max);
                let msg4: &str = "Resize the terminal window to the required dimensions to continue.";
                mvprintw((y_max/2).into(),     x_max/2 - msg1.len() as i32/2, msg1);
                mvprintw((y_max/2 + 2).into(), x_max/2 - msg2.len() as i32/2, msg2.as_str());
                mvprintw((y_max/2 + 3).into(), x_max/2 - msg3.len() as i32/2, msg3.as_str());

                if msg4.len() as i32 > x_max {
                    let PARTITION: usize = 30;
                    mvprintw((y_max/2 + 5).into(), x_max/2 - msg4.len() as i32/2,
                                msg4.get(..PARTITION).unwrap());
                    mvprintw((y_max/2 + 6).into(), x_max/2 - msg4.len() as i32/2,
                                msg4.get(PARTITION..).unwrap());
                }
                else {
                    mvprintw((y_max/2 + 5).into(), x_max/2 - msg4.len() as i32 / 2, msg4);
                }

                refresh();
                sleep(Duration::from_millis(100));
                getmaxyx(stdscr(), &mut y_max, &mut x_max);
            }
            clear();
            true
        }
    }

    /**
     * Resets the terminal settings to their previous state from before the NCurses environment was
     * initialized.
     */
    fn SIGINT_handler () {
        curs_set(CURSOR_VISIBILITY::CURSOR_VISIBLE);
        echo();
        nocbreak();
        endwin();
        std::process::exit(0);
    }
}
