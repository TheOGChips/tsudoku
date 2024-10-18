/// A cell in the terminal display.
#[derive(Eq, Hash, PartialEq, Copy, Clone, Debug)]
pub struct Cell (u8, u8);

impl Cell {
    /**
     * Returns a new Cell representing a cell in the terminal display. The terminal origin is at
     * (0, 0).
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
        consts,
        low_level,
    };
    use pancurses as pc;
    use once_cell::unsync::Lazy;

    /// Sudoku puzzle display origin coordinates (top left cell)
    pub const ORIGIN: Cell = Cell::new(3, 6);

    /// Empty space between the top of the terminal window and the in-terminal display.
    pub const TOP_PADDING: u8 = ORIGIN.row_no();

    /// Empty space between the left side of the terminal window and the in-terminal display.
    pub const LEFT_PADDING: u8 = ORIGIN.col_no();

    /**
     * Minimum number of lines and columns required to properly display the terminal (27 for all
     * cells of the display matrix, 2 for border lines/columns between boxes).
     */
    pub const PUZZLE_SPACE: u8 = 29;

    /// The size requirements for the terminal window.
    pub static mut WINDOW_REQ: Cell = Cell(0, 0);

    /**
     * Empty space between puzzle and the in-game menu. Disabling the in-game menu changes the
     * default value.
     */
    pub static mut VERTICAL_DIVIDER: u8 = 4;

    /**
     * Number of columns to be used for the in-game menu display area. Disabling the in-game menu
     * changes the default value.
     */
    pub static mut IN_GAME_MENU_DISPLAY_SPACING: u8 = 80;

    /**
     * Number of rows to use for the display matrix. This doesn't include the rows containing
     * Box border characters.
     */
    pub const DISPLAY_MATRIX_ROWS: usize = 27;

    /**
     * Number of columns to use for the display matrix. This doesn't include the columns
     * containing Box border characters.
     */
    pub const DISPLAY_MATRIX_COLUMNS: usize = DISPLAY_MATRIX_ROWS;

    /**
     * Global pancurses::Window object that controls the curses display. All wrapper functions in
     * the `display` module use this object.
     */ 
    const WINDOW: Lazy<pc::Window> = Lazy::new(|| pc::initscr());
    
    /**
     * Sets the value of the vertical divider between the display puzzle and the in-game menu.
     *
     *      n -> The number of columns to be used as the vertical divider. If the in-game menu is
     *           disabled, the vertical divider is set to 0.
     */
    pub unsafe fn set_vertical_divider (n: u8) {
        VERTICAL_DIVIDER = n;
    }

    /**
     * Sets the number of columns used to display the in-game menu.
     *
     *      n -> The number of columns to be used to display the in-game menu. If the in-game
     *           menu is disabled, the number of columns is set to 0.
     */
    pub unsafe fn set_in_game_menu_display_spacing (n: u8) {
        IN_GAME_MENU_DISPLAY_SPACING = n;
    }

    /**
     * Enforces window size if terminal window is not compliant and displays what the appropriate
     * size should be.
     */
    pub fn invalid_window_size_handler () -> bool {
        let _ = unsafe {
            low_level::register(consts::SIGINT, || sigint_handler())
        }.expect("Error: Signal not found");

        let (mut y_max, mut x_max): (i32, i32) = WINDOW.get_max_yx();
        
        unsafe {
            if y_max >= WINDOW_REQ.y() as i32 && x_max >= WINDOW_REQ.x() as i32 {
                return false
            }
            while y_max < WINDOW_REQ.y() as i32 || x_max < WINDOW_REQ.x() as i32 {
                WINDOW.clear();
                let msg1: &str = "The current window size is incorrect.";
                let msg2: String = format!("Required dimensions: {} x {}",
                                            WINDOW_REQ.x(), WINDOW_REQ.y());
                let msg3: String = format!("Current dimensions:  {} x {}", x_max, y_max);
                let msg4: &str =
                    "Resize the terminal window to the required dimensions to continue.";
                    WINDOW.mvprintw(
                    (y_max/2).into(),     x_max/2 - msg1.len() as i32/2, msg1
                );
                WINDOW.mvprintw(
                    (y_max/2 + 2).into(), x_max/2 - msg2.len() as i32/2, msg2.as_str()
                );
                WINDOW.mvprintw(
                    (y_max/2 + 3).into(), x_max/2 - msg3.len() as i32/2, msg3.as_str()
                );

                if msg4.len() as i32 > x_max {
                    let partition: usize = 30;
                    WINDOW.mvprintw(
                        (y_max/2 + 5).into(),
                        x_max/2 - msg4.len() as i32/2,
                        msg4.get(..partition).unwrap()
                    );
                    WINDOW.mvprintw(
                        (y_max/2 + 6).into(),
                        x_max/2 - msg4.len() as i32/2,
                        msg4.get(partition..).unwrap()
                    );
                }
                else {
                    WINDOW.mvprintw((y_max/2 + 5).into(), x_max/2 - msg4.len() as i32 / 2, msg4);
                }

                WINDOW.refresh();
                napms(100);
                (y_max, x_max) = WINDOW.get_max_yx();
            }
            WINDOW.clear();
            true
        }
    }

    /**
     * Resets the terminal settings to their previous state from before the pancurses environment
     * was initialized, then terminates the program.
     */
    pub fn sigint_handler () {
        tui_end();
        std::process::exit(0);
    }

    /// Initializes the pancurses environment and the global pancurses::Window object.
    pub fn tui_init () {
        pc::cbreak();
        noecho();
        WINDOW.keypad(true);
    }

    /// Resets the terminal settings to the default before destroying pancurses environment.
    pub fn tui_end () {
        curs_set(CursorVisibility::Block);
        echo();
        pc::nocbreak();
        pc::endwin();
    }

    /**
     * Returns the highest (y, x) coordinates of the terminal window. These should correspond to
     * the cell in the bottom right corner. This function is a wrapper around
     * `pancurses::get_max_yx`.
     */
    pub fn get_max_yx () -> (i32, i32) {
        WINDOW.get_max_yx()
    }

    /**
     * Returns the highest y coordinate of the terminal window. This should correspond to the
     * bottom row of the terminal. This function is a wrapper around `pancurses::get_max_y`.
     */
    pub fn get_max_y () -> i32 {
        WINDOW.get_max_y()
    }

    /**
     * Returns the current position of the cursor in (y, x) coordinate format. This is a wrapper
     * around `pancurses::get_cur_yx`.
     */
    pub fn get_cur_yx () -> (i32, i32) {
        WINDOW.get_cur_yx()
    }

    /**
     * Returns the current y-coordinate of the terminal window (i.e. the current line number).
     * This is a wrapper around `pancurses::get_cur_y`.
     */
    pub fn get_cur_y () -> i32 {
        WINDOW.get_cur_y()
    }

    /**
     * Clears the terminal window of all content. This is a wrapper around `pancurses::clear`.
     */
    pub fn clear () {
        WINDOW.clear();
    }

    /**
     * Clears all content from the cursor's current position to the end of the line the cursor
     * is currently on. This is a wrapper around `pancurses::clrtoeol`.
     */
    pub fn clrtoeol () {
        WINDOW.clrtoeol();
    }

    /**
     * Variants of cursor visibility that can be toggled between. This partially emulates the
     * `ncurses::CURSOR_VISIBILITY` enum.
     */
    pub enum CursorVisibility {
        /** 
         * Do not display the cursor at all. This is similar to
         * `ncurses::CURSOR_VISIBILITY::CURSOR_INVISIBLE`.
         */
        None,
        /**
         * Displays the cursor as a block (usually). This is similar to 
         * `ncurses::CURSORY_VISIBILITY::VERY_VISIBLE`.
         */
        Block,
    }

    /**
     * Toggles the visibility of the cursor. This is a wrapper around `pancurses::curs_set`,
     * although the function signature intentionally looks exactly like `ncurses::curs_set`.
     * 
     *      visibility -> Level of visibility of the cursor. See `display::CursorVisibility`.
     */
    pub fn curs_set (visibility: CursorVisibility) {
        pc::curs_set(match visibility {
            CursorVisibility::None => 0,
            CursorVisibility::Block => 2,
        });
    }

    /**
     * Prints a string starting from the cursor's current position. This is a wrapper around
     * `pancurses::addstr`.
     * 
     *      string -> The string to be printed. A string reference.
     */
    pub fn addstr (string: &str) {
        WINDOW.addstr(string);
    }

    /**
     * Moves the cursor to a (y, x)-coordinate position and starts printing at that location.
     * 
     *      y -> The row to start printing on.
     *      x -> The column to start printing at.
     *      string -> The string to be printed. A string reference.
     */
    pub fn mvprintw (y: i32, x: i32, string: &str) {
        WINDOW.mvprintw(y, x, string);
    }

    /**
     * Clears the screen, moves to the (10, 10)-coordinates, prints a string, and blocks for
     * user input before continuing on. This is used only for debugging purposes during
     * development.
     * 
     *      msg -> The string to be printed.
     */
    pub fn dbgprint (msg: &str) {
        clear();
        mvprintw(10, 10, msg);
        refresh();
        getch();
    }

    /**
     * Updates the terminal display with any changes. This is a wrapper around
     * `pancurses::Window::refresh`.
     */
    pub fn refresh () {
        WINDOW.refresh();
    }

    /**
     * Possible inputs that can be received via `display::getch`. This partially imitates
     * `pancurses::Input` and is not a full re-implementation. Only relevant values to this
     * program are re-implemented.
     */
    #[derive(PartialEq)]
    pub enum Input {
        /// A letter (e.g. 'a') or control (e.g. '\n') character
        Character(char),
        /// The Enter key, although on Linux systems Character('\n') is returned instead.
        KeyEnter,
        /// The four arrow keys
        KeyUp, KeyDown, KeyLeft, KeyRight,
        /// The Backspace key
        KeyBackspace,
        /// The Delete key
        KeyDC,
    }

    /**
     * Returns the next character entered by the user. This is a wrapper around 
     * `pancurses::getch`. Returns an `Option<display::Input>` that is converted from an
     * `Option<pancurses::Input>` with 2 special cases:
     * 
     *      1. Any key not represented in `display::Input` returns `None`.
     *      2. Any character that could by identified by the Enter key returns
     *         `Some(Input::KeyEnter)`.
     */
    pub fn getch () -> Option<Input> {
        match WINDOW.getch() {
            Some(pc::Input::Character('\n')) | Some(pc::Input::Character('\r')) |
                Some(pc::Input::KeyEnter) => Some(Input::KeyEnter),
            Some(pc::Input::Character(ch)) => Some(Input::Character(ch)),
            Some(pc::Input::KeyUp) => Some(Input::KeyUp),
            Some(pc::Input::KeyDown) => Some(Input::KeyDown),
            Some(pc::Input::KeyLeft) => Some(Input::KeyLeft),
            Some(pc::Input::KeyRight) => Some(Input::KeyRight),
            Some(pc::Input::KeyBackspace) => Some(Input::KeyBackspace),
            Some(pc::Input::KeyDC) => Some(Input::KeyDC),
            _ => None,
        }
    }

    /**
     * Returns a string entered by the user up to a certain number of characters. Since
     * `pancurses` does not currently implement this function, this is actually a partial (i.e.
     * not perfect) reimplementation of `ncurses::getnstr`. This should account for the user's
     * input properly, to include the use of the Backspace key, although the display might not
     * properly represent it. Notably, only bare minimum functionality has been implemented, so
     * conveniences like the use of the arrow keys and the Delete key are not handled and thus
     * do nothing.
     * 
     *      target -> The string that the user's input will be copied to. A `&mut String`.
     *      max_len -> The maximum number of characters to copy from the user's input. After the
     *                 user hits Enter, their input will be truncated down to this many
     *                 characters.
     */
    pub fn getnstr (target: &mut String, max_len: usize) {
        let mut string: String = target.clone();
        addstr(format!("{}", string).as_str());
        let mut count: usize = string.len();
        let x_start: i32 = WINDOW.get_cur_x() - count as i32;
        loop {
            match WINDOW.getch() {
                Some(pc::Input::KeyEnter) | Some(pc::Input::Character('\n')) => break,
                Some(pc::Input::KeyBackspace) => {
                    if count > 0 {
                        string.pop();
                        count -= 1;
                        WINDOW.addstr(" ");
                        let (y, x): (i32, i32) = WINDOW.get_cur_yx();
                        WINDOW.mv(y, x - 1);
                    }
                    else {
                        WINDOW.mv(WINDOW.get_cur_y(), x_start);
                    }
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

    /**
     * Numeric codes associated with `display::COLOR_PAIR`s. These are the codes used internally
     * for initializing and toggling `display::COLOR_PAIR`s on the screen. Color pairs of
     * candidate cells alternate depending on which guess or given cell they are adjacent to.
     */
    pub mod pair_code {
        /* NOTE: Don't use 0 with COLOR_PAIRs. This seems to have the effect of having no
         *       attribute on.
         */
        /// The uninteresting default of white text on black background.
        pub const DEFAULT: i16 = 1;
        /// The currently highlighted option in the current menu.
        pub const MENU_SELECTION: i16 = 2;
        /// The default coloring of a `Sudoku` terminal `Cell`.
        pub const UNKNOWN: i16 = 11;
        /// The color of a given clue (aka "hint") in a `Sudoku` puzzle.
        pub const GIVEN: i16 = 12;
        /// One possible color option of a candidate cell.
        pub const CANDIDATES_Y: i16 = 13;
        /// The other possible color option of a candidate cell.
        pub const CANDIDATES_B: i16 = 14;
        /// The color of a guess cell.
        pub const GUESS: i16 = 15;
    }

    /**
     * Establishes the color pairs used while printing anywhere in the display matrix. In the
     * case coloring is not available (in the event this somehow finds its way onto some old
     * machine), a monochrome mode is also provided where everything but guesses are the same
     * color.
     */
    pub fn init_color_pairs () {
        pc::start_color();
        pc::init_pair(pair_code::DEFAULT, pc::COLOR_WHITE, pc::COLOR_BLACK);
        pc::init_pair(pair_code::MENU_SELECTION, pc::COLOR_BLACK, pc::COLOR_WHITE);
        if pc::has_colors() {
            pc::init_pair(pair_code::UNKNOWN, pc::COLOR_WHITE, pc::COLOR_BLACK);
            pc::init_pair(pair_code::GIVEN, pc::COLOR_RED, pc::COLOR_BLACK);
            pc::init_pair(pair_code::CANDIDATES_Y, pc::COLOR_YELLOW, pc::COLOR_BLACK);
            pc::init_pair(pair_code::CANDIDATES_B, pc::COLOR_BLUE, pc::COLOR_BLACK);
            pc::init_pair(pair_code::GUESS, pc::COLOR_GREEN, pc::COLOR_BLACK);
        }
        else {  //Monochrome mode
            // NOTE: Given and guess cells have reversed color scheme to better stand out
            pc::init_pair(pair_code::UNKNOWN, pc::COLOR_WHITE, pc::COLOR_BLACK);
            pc::init_pair(pair_code::GIVEN, pc::COLOR_BLACK, pc::COLOR_WHITE);
            pc::init_pair(pair_code::CANDIDATES_Y, pc::COLOR_WHITE, pc::COLOR_BLACK);
            pc::init_pair(pair_code::CANDIDATES_B, pc::COLOR_WHITE, pc::COLOR_BLACK);
            pc::init_pair(pair_code::GUESS, pc::COLOR_BLACK, pc::COLOR_WHITE);
        }
    }

    /**
     * Moves the cursor the given (y, x)-coordinates. This is wrapper around `pancurses::mv`.
     * 
     *      y -> The row to move to.
     *      x -> The column to move to.
     */
    pub fn mv (y: i32, x: i32) {
        WINDOW.mv(y, x);
    }

    /**
     * Sets a time to wait for user input before returning a default value from getter functions
     * (e.g. `display::getch`). This is a wrapper around `pancurses::timeout`.
     * 
     *      wait_time -> Time to wait for input in milliseconds.
     */
    pub fn timeout (wait_time: i32) {
        WINDOW.timeout(wait_time);
    }

    /**
     * Controls whether to block on calls to `display::getch`. This is a wrapper around
     * `pancurses::nodelay`.
     * 
     *      no_delay -> Whether to delay or not (false or true, respectively).
     */
    pub fn nodelay (no_delay: bool) {
        WINDOW.nodelay(no_delay);
    }

    /**
     * Amount of time to temporarily halt the program. This is a wrapper around
     * `pancurses::napms`.
     * 
     *      nap_time -> Amount of time to halt in milliseconds.
     */
    pub fn napms (nap_time: i32) {
        pc::napms(nap_time);
    }

    /**
     * Toggles bold text on or off.
     * 
     *      bold_on -> Whether to toggle bold text on or off (true or false, respectively).
     */
    pub fn bold_set (bold_on: bool) {
        if bold_on {
            WINDOW.attron(pc::A_BOLD);
        }
        else {
            WINDOW.attroff(pc::A_BOLD);
        }
    }

    /**
     * Options used by outside functions to request a change in color output in the terminal
     * display.
     */
    #[derive(PartialEq, Copy, Clone, Debug)]
    pub enum ColorPair {
        /// The uninteresting default of white text on black background.
        Default,
        /// The currently highlighted option in the current menu.
        MenuSelection,
        /// The default coloring of a `Sudoku` terminal `Cell`.
        Unknown,
        /// The color of a given clue (aka "hint") in a `Sudoku` puzzle.
        Given,
        /// One possible color option of a candidate cell.
        CandidatesY,
        /// The other possible color option of a candidate cell.
        CandidatesB,
        /// The color of a guess cell.
        Guess,
    }

    /**
     * Toggles the foreground and background color pair used when printing in the terminal
     * display. The `display::ColorPair` variant requested is converted to a numeric code
     * internally for use in actually setting the color pair. This is a wrapper around
     * `pancurses::color_set`.
     * 
     *      pair -> Foreground/background `display::ColorPair` variant used for requesting what
     *              color pair to toggle on.
     */
    pub fn color_set (pair: &ColorPair) {
        WINDOW.color_set(
            match pair {
                ColorPair::Default => pair_code::DEFAULT,
                ColorPair::MenuSelection => pair_code::MENU_SELECTION,
                ColorPair::Unknown => pair_code::UNKNOWN,
                ColorPair::Given => pair_code::GIVEN,
                ColorPair::CandidatesY => pair_code::CANDIDATES_Y,
                ColorPair::CandidatesB => pair_code::CANDIDATES_B,
                ColorPair::Guess => pair_code::GUESS,
            }
        );
    }

    /**
     * Toggles on the display of the characters the user types. This is a wrapper around
     * `pancurses::echo`.
     */
    pub fn echo () {
        pc::echo();
    }

    /**
     * Toggles off the display of the characters the user types. This is a wrapper around
     * `pancurses::noecho`.
     */
    pub fn noecho () {
        pc::noecho();
    }

    /**
     * Retrieves the character and attribute from the specified window position, in the form of
     * a chtype. This is a wrapper around `pancurses::Window::mvinch`.
     */
    pub fn mvinch (y: i32, x: i32) -> pc::chtype {
        WINDOW.mvinch(y, x)
    }

    /**
     * Extracts the character out of a `pancurses::chtype`. Internally, this is obtained by
     * bitwise anding against `pancurses::A_CHARTEXT`; this information will be left in the
     * least significant bit. The result is returned as a `pancurses::chtype`.
     * 
     *      ch -> Pancurses terminal cell information. This should be the result of a call to
     *            `pancurses::mvinch`.
     * 
     * Comparison can be done against the return value by casting the character(s) checked to a
     * `pancurses::chtype` like so:
     * 
     *      `ch == 'x' as pancurses::chtype`
     *      `['x' as pancurses::chtype, 'y' as pancurses::chtype].contains(&ch)`
     */
    pub fn decode_char (ch: pc::chtype) -> pc::chtype {
        ch & pc::A_CHARTEXT
    }

    /**
     * Extracts the `ColorPair` information out of a `pancurses::chtype` (obtained via
     * `pancurses::Window::mvinch`). Internally, this is obtained by bitwise anding against
     * `pancurses::A_COLOR`. `ColorPair::DEFAULT` is returned if any of the other `ColorPair`s
     * are not found.
     * 
     *      ch -> Pancurses terminal cell information. This should be the result of a call to
     *            `pancurses::mvinch`.
     */
    pub fn decode_color_pair (ch: pc::chtype) -> ColorPair {
        let code: i16 = ((ch & pc::A_COLOR) >> 8) as i16;
        if code == pair_code::MENU_SELECTION {
            ColorPair::MenuSelection
        }
        else if code == pair_code::UNKNOWN {
            ColorPair::Unknown
        }
        else if code == pair_code::GIVEN {
            ColorPair::Given
        }
        else if code == pair_code::CANDIDATES_Y {
            ColorPair::CandidatesY
        }
        else if code == pair_code::CANDIDATES_B {
            ColorPair::CandidatesB
        }
        else if code == pair_code::GUESS {
            ColorPair::Guess
        }
        else /* code == pair_code::DEFAULT */ {
            ColorPair::Default
        }
    }
}
