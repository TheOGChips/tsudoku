use ncurses::{
    initscr, endwin,
    nocbreak, cbreak,
    noecho, echo,
    stdscr,
    keypad,
    start_color, init_pair, COLOR_PAIR, COLOR_WHITE, COLOR_BLACK,
    curs_set, CURSOR_VISIBILITY,
    mvprintw, addstr,
    attron, attroff,
    refresh,
    getmaxyx,
    clear,
    timeout,
    getch,
    KEY_DOWN, KEY_UP,
    nodelay,
};
use signal_hook::{
    consts::SIGINT,
    low_level::register,
};
use strum::{
    EnumIter,
    IntoEnumIterator,
    EnumCount,
    VariantArray,
};
use crate::{
    terminal::{
        KEY_ENTER,
        Cell,
        display::{
            TOP_PADDING,
            LEFT_PADDING,
            PUZZLE_SPACE,
            WINDOW_REQ,
            VERTICAL_DIVIDER,
            IN_GAME_MENU_DISPLAY_SPACING,
            set_VERTICAL_DIVIDER,
            set_IN_GAME_MENU_DISPLAY_SPACING,
            invalid_window_size_handler,
            DISPLAY_MATRIX_ROWS,
            DISPLAY_MATRIX_COLUMNS,
        },
    },
    common::{
        DIR,
        csv,
        dbgprint,
    },
    sudoku::SavedPuzzle,
};
use std::{
    fs,
    cell::RefCell,
};

//NOTE: Don't use 0 with COLOR_PAIRs. This seems to have the effect of having no attribute on.
/// The COLOR_PAIR associated with the current highlighted selection in the menu.
pub const MENU_SELECTION: i16 = 1;

/// A wrapper enum to enforce a certain type of MenuOption be used
pub enum MenuOption {
    /// Wrapper to enforce using a MainMenuOption variant
    MAIN_MENU(MainMenuOption),
    /// Wrapper to enforce using a SavedGameMenuOption variant
    SAVED_GAME_MENU(SavedGameMenuOption),
    /// Wrapper to enforce using a DifficultyMenuOption variant
    DIFFICULTY_MENU(DifficultyMenuOption),
    /// Wrapper to enforce using an InGameMenuOption variant
    IN_GAME_MENU(InGameMenuOption),
}

/// Options displayed on the main menu.
#[derive(PartialEq, EnumIter, EnumCount, VariantArray, Copy, Clone)]
pub enum MainMenuOption {
    /// Start a new game
    NEW_GAME,
    /// Resume a previously saved game
    RESUME_GAME,
    /// Show the number of completed games
    SHOW_STATS,
    /// Exit the program
    EXIT,
}

impl MainMenuOption {
    /**
     * Returns a zipped iterator associating an 8-bit integer with each MainMenuOption variant. This
     * helps in determining the offset for displaying the options in the terminal window where each
     * 8-bit integer is the offset from the center of the screen.
     */
    fn enumerate () -> std::iter::Zip<std::ops::Range<u8>, MainMenuOptionIter> {
        (0..Self::COUNT as u8).zip(Self::iter()).into()
    }
}

///Options displayed on the saved game menu.
pub enum SavedGameMenuOption {
    /// Indicates that a saved game is available and ready to be loaded
    SAVE_READY,
    /// Indicates that no saved games are available to be resumed
    NO_SAVES,
    /// Used when a SavedGameMenuOption isn't applicable in the current context
    NONE,
}

/* According to https://www.101computing.net/sudoku-generator-algorithm/, the minimum
 * amount of tiles that need to be filled in in order to create a uniquely solvable
 * puzzle is 17.
 */
/// Different difficulty levels used to determing how many clues the puzzle starts with
#[derive(PartialEq, EnumIter, EnumCount, VariantArray, Copy, Clone)]
pub enum DifficultyMenuOption {
    /// The puzzle will begin with 60 clues
    EASY,
    /// The puzzle will begin with 45 clues
    MEDIUM,
    /// The puzzle will begin with 30 clues
    HARD,
    /// The puzzle will begin with 17 clues, the minimum number for a unique solution.
    EXPERT,
}

impl DifficultyMenuOption {
    /**
     * Returns a zipped iterator associating an 8-bit integer with each
     * DifficultyMenuOption variant. This helps in determining the offset for displaying
     * the options in the terminal window where each 8-bit integer is the offset from the
     * center of the screen.
     */
    fn enumerate () -> std::iter::Zip<std::ops::Range<u8>, DifficultyMenuOptionIter> {
        (0..Self::COUNT as u8).zip(Self::iter()).into()
    }
}

enum InGameMenuOption {
    /// Display the rules of sudoku
    RULES,
    /// Display the tsudoku manual
    MANUAL,
    /// Save the state of the current sudoku game
    SAVE_GAME,
}

pub trait Menu {
    fn display_menu (&self, _: &Cell, _: &MenuOption);
    fn menu (&self) -> MenuOption;

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
        /* TODO: Look into a cleaner way to do this. This might not be needed, but no destructors
         *       will get called for any objects still on the stack. This works for now, though.
         */
    }
}

/// Displays and controls the main menu the user sees before and after every game.
pub struct MainMenu {
    /// Empty space between the bottom of the terminal window and the in-terminal display.
    BOTTOM_PADDING: u8,

    /// Empty space between the right side of the terminal window and the in terminal display.
    RIGHT_PADDING: u8,

    /// Minimum number of lines required to properly display the puzzle's evaluation result.
    RESULT_MSG_SPACE: u8,

    /// Whether the in-game menu is able to be displayed.
    in_game_menu_enabled: bool
}

impl Menu for MainMenu {
    /**
     * Displays the main menu. The currently selected option is always highlighted. The main menu is
     * re-rendered each time the user uses the Up/Down keys to highlight a different option.
     *
     *      MAX -> Bottom right corner cell of the terminal window. Signifies the max number of lines
     *             and columns in the window.
     *      OPT -> The currently selected main menu option.
     */
    fn display_menu (&self, MAX: &Cell, OPT: &MenuOption) {
        let TITLE: &str = "MAIN MENU";
        let Y_CENTER: u8 = MAX.y()/2;
        let X_CENTER: u8 = MAX.x()/2 - TITLE.len() as u8/2;
        let opt: &MainMenuOption = if let MenuOption::MAIN_MENU(option) = OPT {
                option
            }
            else {
                println!("Error: Did not receive a MainMenuOption. Exiting...");
                std::process::exit(1);
            };

        mvprintw(Y_CENTER as i32 - 2, X_CENTER as i32, TITLE);
        for (i, variant) in MainMenuOption::enumerate() {
            if *opt == variant {
                attron(COLOR_PAIR(MENU_SELECTION));
            }
            mvprintw((Y_CENTER + i) as i32, X_CENTER as i32, match variant {
                MainMenuOption::NEW_GAME => "New Game",
                MainMenuOption::RESUME_GAME => "Resume Game",
                MainMenuOption::SHOW_STATS => "Show # Finished Games",
                MainMenuOption::EXIT => "Exit",
            });
            if *opt == variant {
                attroff(COLOR_PAIR(MENU_SELECTION));
            }
        }
        refresh();
    }

    /**
     * Implements the logic behind the main menu. Controls which option will be highlighted when
     * displaying all options indicating the current selection.
     */
    fn menu (&self) -> MenuOption {
        unsafe {
            if !self.in_game_menu_enabled {
                set_VERTICAL_DIVIDER(0);
                set_IN_GAME_MENU_DISPLAY_SPACING(0);
            }
            self.set_WINDOW_REQ();
        }

        let mut y_max: i32 = 0;
        let mut x_max: i32 = 0;
        getmaxyx(stdscr(), &mut y_max, &mut x_max);
        curs_set(CURSOR_VISIBILITY::CURSOR_INVISIBLE);

        //invalid_window_size_handler();
        //clear();

        let max: Cell = Cell::new(y_max as u8, x_max as u8);
        let mut opt: MainMenuOption = MainMenuOption::NEW_GAME;
        //self.display_menu(&max, &opt);
        let mut input: i32 = -1;
        timeout(250);
        while input != KEY_ENTER {
            invalid_window_size_handler();
            clear();
            self.display_menu(&max, &MenuOption::MAIN_MENU(opt));
            input = getch();
            opt =
                if input == KEY_UP || input == 'w' as i32 {
                    match opt {
                        MainMenuOption::EXIT => MainMenuOption::SHOW_STATS,
                        MainMenuOption::SHOW_STATS => MainMenuOption::RESUME_GAME,
                        _ => MainMenuOption::NEW_GAME,
                    }
                }
                else if input == KEY_DOWN || input == 's' as i32 {
                     match opt {
                        MainMenuOption::NEW_GAME => MainMenuOption::RESUME_GAME,
                        MainMenuOption::RESUME_GAME => MainMenuOption::SHOW_STATS,
                        _ => MainMenuOption::EXIT,
                    }
                }
                else {
                    opt
                };
        }

        clear();
        nodelay(stdscr(), false);
        curs_set(CURSOR_VISIBILITY::CURSOR_VISIBLE);
        MenuOption::MAIN_MENU(opt)
    }
}

impl MainMenu {
    /**
     * Initializes the NCurses environment and global NCurses settings. Returns a MainMenu object with
     * the remainder of the padding space set for the terminal window display.
     *
     *      use_in_game_menu -> Indicating whether the in-game menu is disabled or not. This affects
     *                          the enforced size of the terminal window.
     */
    pub fn new (use_in_game_menu: bool) -> MainMenu {
        let _ = unsafe {
            register(SIGINT, || Self::SIGINT_handler())
        }.expect("Error: Signal not found");

        initscr();
        cbreak();
        noecho();
        keypad(stdscr(), true);

        start_color();
        init_pair(MENU_SELECTION, COLOR_BLACK, COLOR_WHITE);

        Self {
            BOTTOM_PADDING: TOP_PADDING,
            RIGHT_PADDING: LEFT_PADDING,
            RESULT_MSG_SPACE: 3,
            in_game_menu_enabled: use_in_game_menu,
        }
    }

    /**
     * Sets the number of lines and columns the terminal window must be in order to play. This size is
     * dependent on whether the in-game menu is enabled.
     */
    unsafe fn set_WINDOW_REQ (&self) {
        WINDOW_REQ = Cell::new(
            TOP_PADDING + PUZZLE_SPACE + self.RESULT_MSG_SPACE + self.BOTTOM_PADDING,
            LEFT_PADDING + PUZZLE_SPACE + VERTICAL_DIVIDER + IN_GAME_MENU_DISPLAY_SPACING + self.RIGHT_PADDING,
        )
    }
}

impl Drop for MainMenu {
    /// Unsets the NCurses environment once the user chooses to exit the program.
    fn drop (&mut self) {
        echo();
        nocbreak();
        endwin();
        //TODO: Probably unnecessary, but consider unregistering the SIGINT handler function.
    }
}

/* The use of RefCell was a workaround during the port from the pure C++ version.
 * There's probably a better way to handle this that doesn't involve use of RefCells,
 * but that would require a much deeper refactoring than I'm willing to give it at this 
 * point.
 */
/// Provides the functionality to allow a player to load and resume a saved game.
pub struct SavedGameMenu {
    /**
     * The list of saved games to be displayed to the screen. Each will appear as an
     * option the user can choose from.
     */
    saved_games: Vec<String>,
    /// The matrix used to read a saved game into. This later becomes the display matrix.
    saved_game: RefCell<SavedPuzzle>,
    /**
     * The currently highlighted game from the displayed list. If the user presses Enter,
     * this becomes the game loaded.
     */
    selection: RefCell<String>,
}

impl SavedGameMenu {
    /**
     * Returns an instance of a SavedGameMenu containing a list of the current saved
     * games. The saved game to be used will be initially be set to a "blank" 
     * SavedPuzzle object. The current selection from the `saved_games` list will be
     * preset to the first file in the list.
     */
    pub fn new () -> Self {
        let saved_games: Vec<String> = Self::generate_saved_games_list();
        let selection: String = String::clone(&saved_games[0]);
        Self {
            saved_games: saved_games,
            saved_game: RefCell::new(SavedPuzzle::new()),
            selection: RefCell::new(selection), //NOTE: This contains the CSV extension
        }
    }

    /**
     * Creates the list of saved games from the names of available CSV files in the 
     * ~/.tsudoku directory. This entries in this list are what will be displayed to the 
     * player. The entries are stored without the ".csv" file extension. Text files with 
     * extension ".txt" are ignored so as to avoid adding the completed games file to 
     * the list.
     */
    fn generate_saved_games_list () -> Vec<String> {
        let mut saved_games: Vec<String> = match fs::read_dir(DIR()) {
            Ok(list) => list.filter(
                |file| file.as_ref().unwrap().path().display().to_string().contains(".csv")
            )
            .map(|file| file.unwrap().file_name().to_str().unwrap().to_string())
            .collect(),
            Err(msg) => {
                eprintln!("{}", msg.to_string());
                std::process::exit(1);
            },
        };
        saved_games.sort();
        saved_games
    }

    /**
     * Controls iterating through the list from player input and highlighting the name 
     * of the current game that will be loaded once the player presses Enter. If there 
     * are no saved games, the player will instead be notified as much and then prompted
     * to continue.
     */
    fn select_saved_game (&self) -> bool {
        let mut input: i32 = -1;
        curs_set(CURSOR_VISIBILITY::CURSOR_INVISIBLE);  //Turn off cursor while in menu
        timeout(250);
        if self.saved_games.is_empty() {
            while input != KEY_ENTER {
                mvprintw(TOP_PADDING as i32, LEFT_PADDING as i32, "You have no saved games.");
                mvprintw(
                    TOP_PADDING as i32 + self.saved_games.len() as i32 + 3,
                    LEFT_PADDING as i32,
                    "Press ENTER to continue..."
                );
                refresh();
                input = getch();    //This needs to be here for the display to work correctly
                invalid_window_size_handler();
            }
        }
        else {
            while input != KEY_ENTER {
                self.display_menu(
                    &Cell::new(TOP_PADDING, LEFT_PADDING), &MenuOption::SAVED_GAME_MENU(SavedGameMenuOption::NONE)
                );

                input = getch();
                let selection: String = self.selection.borrow().to_string();
                let i: usize = self.saved_games.binary_search(&selection.to_string()).unwrap();
                if (input == KEY_DOWN || input as u8 as char == 's') &&
                    selection.as_str() != self.saved_games.last().unwrap() {
                        *self.selection.borrow_mut() = self.saved_games.get(i + 1).unwrap().to_string();
                }
                else if (input == KEY_UP || input as u8 as char == 'w') &&
                    selection.as_str() != self.saved_games.first().unwrap() {
                        *self.selection.borrow_mut() = self.saved_games.get(i - 1).unwrap().to_string();
                }
                else {
                    invalid_window_size_handler();
                }
            }
        }

        refresh();
        nodelay(stdscr(), false);
        curs_set(CURSOR_VISIBILITY::CURSOR_VISIBLE);

        !self.saved_games.is_empty()
    }

    /// Reads a saved game from it's CSV file to the saved game and color code matrices.
    fn read_saved_game (&self) {
        let game_data: Vec<u8> = csv::read(
            DIR().join(self.selection.borrow().to_string()).to_str().unwrap()
            )
            .unwrap();

        let mut i: usize = 0;
        let mut j: usize = 0;
        let mut saved_game: [[u8; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS] =
            [[0; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS];
        let mut saved_color_codes: [[char; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS] =
            [[' '; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS];
        for byte in game_data {
            if (byte as char).is_numeric() {
                saved_game[i][j] = byte;
            }
            else if (byte as char).is_alphabetic() {
                saved_color_codes[i][j] = byte as char;
                i += 1;
            }
            else if byte == '\n' as u8 {
                i = 0;
                j += 1;
            }
        }

        dbgprint(self.selection.borrow().as_str());
        let mut puzzle: SavedPuzzle = SavedPuzzle::new();
        puzzle.set_puzzle(saved_game);
        puzzle.set_color_codes(saved_color_codes);
        puzzle.set_filename(DIR().join(self.selection.borrow().as_str())
            .to_str()
            .unwrap()
        );
        *self.saved_game.borrow_mut() = puzzle;
    }

    /**
     * Wraps the selected saved game value and color code matrices into a SavedPuzzle 
     * object which is returned to the calling function. This makes passing around the 
     * saved game information easier.
     */
    pub fn get_saved_game (&self) -> SavedPuzzle {
        self.saved_game.borrow().clone()
    }
}

impl Menu for SavedGameMenu {
    /**
     * Displays the saved games menu. The options listed are saved games in CSV files 
     * from the ~/.tsudoku directory. The currently selected option is always 
     * highlighted. The saved games menu is re-rendered each time the player uses the 
     * Up/Down keys to highlight a different option.
     * 
     *      EDGE -> Starting cell the saved games menu will display at. The menu title 
     *              should display on the line below the top padding and the column 
     *              after the vertical divider.
     *      unused MenuOption enum variant -> Required because of the function prototype 
     *                                        inherited from Menu.
     */
    fn display_menu (&self, EDGE: &Cell, _: &MenuOption) {
        let mut display_line: u8 = EDGE.y();
        clear();
        mvprintw(display_line as i32, EDGE.x() as i32, "Saved Games:");
        display_line += 2;
        for game in &self.saved_games {
            if self.selection.borrow().to_string() == *game {
                attron(COLOR_PAIR(MENU_SELECTION));
            }
            mvprintw(
                display_line as i32,
                EDGE.x() as i32,
                format!("{}", game.strip_suffix(".csv").unwrap())
                    .as_str()
            );
            if self.selection.borrow().to_string() == *game {
                attroff(COLOR_PAIR(MENU_SELECTION));
            }
            display_line += 1;
        }
    }

    /**
     * Coordinates generating the saved games list, displaying the list to the player in 
     * menu form, and reading in the saved game chosen by the player.
     */
    fn menu (&self) -> MenuOption {
        if self.select_saved_game() {
            self.read_saved_game();
            MenuOption::SAVED_GAME_MENU(SavedGameMenuOption::SAVE_READY)
        }
        else {
            MenuOption::SAVED_GAME_MENU(SavedGameMenuOption::NO_SAVES)
        }
    }
}

/// Allows the user to choose a difficulty level before starting a new game.
pub struct DifficultyMenu {
    difficulty_level: DifficultyMenuOption,
}

impl DifficultyMenu {
    /**
     * Returns the difficulty level the user has chosen to start the new game.
     */
    pub fn new () -> Self {
        Self {
            difficulty_level: DifficultyMenuOption::EASY,
        }
    }
    
    /**
     * Records the difficulty level the user has chosen to start a new game.
     * 
     *      DIFF -> The chosen difficulty level.
     */
    pub fn set_difficulty_level (&mut self, diff: DifficultyMenuOption) {
        self.difficulty_level = diff;
    }
    
    /**
     * Returns the difficulty level the user has chosen to start the new game.
     */
    pub fn get_difficulty_level (&self) -> DifficultyMenuOption {
        self.difficulty_level
    }
}

impl Menu for DifficultyMenu {
    /**
     * Displays the difficulty menu. The currently selected option is always highlighted. The
     * difficulty menu is re-rendered each time the user uses the Up/Down keys to highlight a
     * different option.
     * 
     *      EDGE -> Starting cell the difficulty menu will display at. The menu title should
     *              display at the origin.
     *      OPT -> The currently selected difficulty menu option.
     */
    fn display_menu (&self, EDGE: &Cell, OPT: &MenuOption) {
        let opt: &DifficultyMenuOption = if let MenuOption::DIFFICULTY_MENU(option) = OPT {
            option
        }
        else {
            println!("Error: Did not receive a MainMenuOption. Exiting...");
            std::process::exit(1);
        };

        clear();
        mvprintw(EDGE.y() as i32, EDGE.x() as i32, "CHOOSE DIFFICULTY SETTING");
        for (i, variant) in DifficultyMenuOption::enumerate() {
            if *opt == variant {
                attron(COLOR_PAIR(MENU_SELECTION));
            }
            mvprintw((EDGE.y() + i) as i32, EDGE.x() as i32, match variant {
                DifficultyMenuOption::EASY => "Easy",
                DifficultyMenuOption::MEDIUM => "Medium",
                DifficultyMenuOption::HARD => "Hard",
                DifficultyMenuOption::EXPERT => "Expert",
            });
            if *opt == variant {
                attroff(COLOR_PAIR(MENU_SELECTION));
            }
        }
        refresh();
    }
    
    /**
     * Controls the menu display and difficulty level recording.
     */
    fn menu (&self) -> MenuOption {
        curs_set(CURSOR_VISIBILITY::CURSOR_INVISIBLE);
        let mut diff: DifficultyMenuOption = DifficultyMenuOption::EASY;
        let mut input: i32 = 0;
        timeout(250);
        while input != KEY_ENTER {
            refresh();
            self.display_menu(&Cell::new(TOP_PADDING, LEFT_PADDING), &MenuOption::DIFFICULTY_MENU(diff));
            input = getch();
            diff = 
                if input == KEY_DOWN || input == 's' as i32 {
                    match diff {
                        DifficultyMenuOption::EXPERT => DifficultyMenuOption::HARD,
                        DifficultyMenuOption::HARD => DifficultyMenuOption::MEDIUM,
                        _ => DifficultyMenuOption::EASY,
                    }
                }
                else if input == KEY_UP || input == 'w' as i32 {
                    match diff {
                        DifficultyMenuOption::EASY => DifficultyMenuOption::MEDIUM,
                        DifficultyMenuOption::MEDIUM => DifficultyMenuOption::HARD,
                        _ => DifficultyMenuOption::EXPERT,
                    }
                }
                else { diff }
        }
        nodelay(stdscr(), false);
        curs_set(CURSOR_VISIBILITY::CURSOR_VISIBLE);

        MenuOption::DIFFICULTY_MENU(diff)
    }
}

pub struct InGameMenu {
    display_matrix: [[u8; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS],
}

impl InGameMenu {
    /**
     * Initializes the display matrix, so the in-game menu can track any changes made during
     * gameplay.
     * 
     *      display_matrix -> The display matrix from the main gameplay loop in Sudoku.
     */
    pub fn new (display_matrix: &[[u8; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS]) -> Self {
        Self {
            display_matrix: *display_matrix,
        }
    }
}


