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
    use pancurses as pc;
    use std::{
        thread,
        time,
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
    pub fn invalid_window_size_handler (window: &pc::Window) -> bool {
        let _ = unsafe {
            register(SIGINT, || SIGINT_handler())
        }.expect("Error: Signal not found");

        let (mut y_max, mut x_max): (i32, i32) = window.get_max_yx();
        
        unsafe {
            if (y_max == WINDOW_REQ.y() as i32 && x_max == WINDOW_REQ.x() as i32) {
                return false
            }
            while y_max != WINDOW_REQ.y() as i32 || x_max != WINDOW_REQ.x() as i32 {
                window.clear();
                let msg1: &str = "The current window size is incorrect.";
                let msg2: String = format!("Required dimensions: {} x {}",
                                            WINDOW_REQ.x(), WINDOW_REQ.y());
                let msg3: String = format!("Current dimensions:  {} x {}", x_max, y_max);
                let msg4: &str = "Resize the terminal window to the required dimensions to continue.";
                window.mvprintw((y_max/2).into(),     x_max/2 - msg1.len() as i32/2, msg1);
                window.mvprintw((y_max/2 + 2).into(), x_max/2 - msg2.len() as i32/2, msg2.as_str());
                window.mvprintw((y_max/2 + 3).into(), x_max/2 - msg3.len() as i32/2, msg3.as_str());

                if msg4.len() as i32 > x_max {
                    let PARTITION: usize = 30;
                    window.mvprintw((y_max/2 + 5).into(), x_max/2 - msg4.len() as i32/2,
                                msg4.get(..PARTITION).unwrap());
                    window.mvprintw((y_max/2 + 6).into(), x_max/2 - msg4.len() as i32/2,
                                msg4.get(PARTITION..).unwrap());
                }
                else {
                    window.mvprintw((y_max/2 + 5).into(), x_max/2 - msg4.len() as i32 / 2, msg4);
                }

                window.refresh();
                thread::sleep(time::Duration::from_millis(100));
                (y_max, x_max) = window.get_max_yx();
            }
            window.clear();
            true
        }
    }

    /**
     * Resets the terminal settings to their previous state from before the NCurses environment was
     * initialized.
     */
    fn SIGINT_handler () {
        pc::curs_set(CURSOR_VISIBILITY::BLOCK);
        pc::echo();
        pc::nocbreak();
        pc::endwin();
        std::process::exit(0);
    }

    /**
     * 
     */
    pub fn tui_init () -> pc::Window {
        let window: pc::Window = pc::initscr();
        pc::cbreak();
        pc::noecho();
        window.keypad(true);
        window
    }

    /**
     * 
     */
    pub mod CURSOR_VISIBILITY {
        pub const NONE: i32 = 0;
        pub const BLOCK: i32 = 2;
    }

    /**
     * 
     */
    pub fn getnstr (window: &pc::Window, target: &mut String, max_len: usize) {
        let mut string: String = String::new();
        let mut count: usize = 0;
        loop {
            match window.getch() {
                Some(pc::Input::KeyEnter) | Some(pc::Input::Character('\n')) => break,
                Some(pc::Input::KeyBackspace) => {
                    string.pop();
                    count -= if count > 0 {
                        1
                    } else {
                        0
                    };
                    window.addstr(" ");
                    let (y, x): (i32, i32) = window.get_cur_yx();
                    window.mv(y, x - 1);
                },
                Some(pc::Input::Character(c)) => {
                    if count < max_len {
                        string.push(c);
                        count += 1;
                    }
                },
                _ => (),
            }
        }
        *target = string;
    }

    pub mod pair {
        //NOTE: Don't use 0 with COLOR_PAIRs. This seems to have the effect of having no attribute on.
        /// The COLOR_PAIR associated with the current highlighted selection in the menu.
        pub const DEFAULT: i16 = 1;
        pub const MAIN_MENU_SELECTION: i16 = 2;
        pub const DIFFICULTY_MENU_SELECTION: i16 = 3;
    }

    /**
     * 
     */
    pub fn init_color_pairs () {
        pc::start_color();
        pc::init_pair(pair::DEFAULT, pc::COLOR_WHITE, pc::COLOR_BLACK);
        pc::init_pair(pair::MAIN_MENU_SELECTION, pc::COLOR_BLACK, pc::COLOR_WHITE);
        pc::init_pair(pair::DIFFICULTY_MENU_SELECTION, pc::COLOR_BLACK, pc::COLOR_WHITE);
    }

    /**
     * 
     */
    pub enum COLOR_ATTR {
        DEFAULT,
        MAIN_MENU_SELECTION,
        DIFFICULTY_MENU_SELECTION,
    }

    /**
     * 
     */
    pub fn attron (window: &pc::Window, attr: COLOR_ATTR) {
        window.color_set(
            match attr {
                COLOR_ATTR::DEFAULT => pair::DEFAULT,
                COLOR_ATTR::MAIN_MENU_SELECTION => pair::MAIN_MENU_SELECTION,
                COLOR_ATTR::DIFFICULTY_MENU_SELECTION => pair::DIFFICULTY_MENU_SELECTION,
            }
        );
    }

    /**
     * 
     */
    pub fn attroff (window: &pc::Window, attr: COLOR_ATTR) {
        window.color_set(pair::DEFAULT);
    }
}
