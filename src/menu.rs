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
            self,
            COLOR_PAIR,
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
            CURSOR_VISIBILITY,
        },
    },
    common::{
        DIR,
        csv,
    },
    sudoku::SavedPuzzle,
};
use std::{
    fs,
    cell::RefCell,
};

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

#[derive(PartialEq, EnumIter, EnumCount, VariantArray, Copy, Clone)]
enum InGameMenuOption {
    /// Display the rules of sudoku
    RULES,
    /// Display the tsudoku manual
    MANUAL,
    /// Save the state of the current sudoku game
    SAVE_GAME,
    /// Don't highlight any option
    NONE,
}

impl InGameMenuOption {
    /**
     * Returns a zipped iterator associating an 8-bit integer with each InGameMenuOption variant.
     * This helps in determining the offset for displaying the options in the terminal window
     * where each 8-bit integer is the offset from the center of the screen.
     */
    fn enumerate () -> std::iter::Zip<std::ops::Range<u8>, InGameMenuOptionIter> {
        (0..Self::COUNT as u8).zip(Self::iter()).into()
    }
}

pub trait Menu {
    fn display_menu (&self, _: &Cell, _: &MenuOption);
    fn menu (&self) -> MenuOption;

    /**
     * Resets the terminal settings to their previous state from before the NCurses environment was
     * initialized.
     */
    fn SIGINT_handler () {
        display::tui_end();
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
    in_game_menu_enabled: bool,
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

        display::mvprintw(Y_CENTER as i32 - 2, X_CENTER as i32, TITLE);
        for (i, variant) in MainMenuOption::enumerate() {
            if *opt == variant {
                display::color_set(COLOR_PAIR::MAIN_MENU_SELECTION);
                //mvprintw(2, 2, format!("status: {}", COLOR_PAIR(MAIN_MENU_SELECTION)).as_str());
            }
            display::mvprintw((Y_CENTER + i) as i32, X_CENTER as i32, match variant {
                MainMenuOption::NEW_GAME => "New Game",
                MainMenuOption::RESUME_GAME => "Resume Game",
                MainMenuOption::SHOW_STATS => "Show # Finished Games",
                MainMenuOption::EXIT => "Exit",
            });
            if *opt == variant {
                display::color_set(COLOR_PAIR::DEFAULT);
            }
        }
        display::refresh();
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

        let (y_max, x_max): (i32, i32) = display::get_max_yx();
        display::curs_set(CURSOR_VISIBILITY::NONE);

        //invalid_window_size_handler();
        //clear();

        //TODO: For some reason, the menu selection highlighting is getting turned off
        //      for the main menu completely after returning from the difficulty menu.
        //      Highlighting in the difficulty menu is not affected.
        let max: Cell = Cell::new(y_max as u8, x_max as u8);
        let mut opt: MainMenuOption = MainMenuOption::NEW_GAME;
        //self.display_menu(&max, &opt);
        let mut input: Option<display::Input> = None;
        display::timeout(250);
        while input != Some(display::Input::KeyEnter) {
            invalid_window_size_handler();
            display::clear();
            self.display_menu(&max, &MenuOption::MAIN_MENU(opt));
            input = display::getch();
            opt =
                match input {
                    Some(display::Input::KeyUp) | Some(display::Input::Character('w')) => match opt {
                        MainMenuOption::EXIT => MainMenuOption::SHOW_STATS,
                        MainMenuOption::SHOW_STATS => MainMenuOption::RESUME_GAME,
                        _ => MainMenuOption::NEW_GAME,
                    },
                    Some(display::Input::KeyDown) | Some(display::Input::Character('s')) => match opt {
                        MainMenuOption::NEW_GAME => MainMenuOption::RESUME_GAME,
                        MainMenuOption::RESUME_GAME => MainMenuOption::SHOW_STATS,
                        _ => MainMenuOption::EXIT,
                    },
                    Some(display::Input::KeyEnter) => {
                        opt
                    },
                    _ => opt
                };
        }

        display::clear();
        display::nodelay(false);
        display::curs_set(CURSOR_VISIBILITY::BLOCK);
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

        display::tui_init();
        display::init_color_pairs();

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
        display::tui_end();
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
        let mut input: Option<display::Input> = None;
        display::curs_set(CURSOR_VISIBILITY::NONE);  //Turn off cursor while in menu
        display::timeout(250);
        if self.saved_games.is_empty() {
            loop {
                match input {
                    Some(display::Input::KeyEnter) => break,
                    _ => {
                        display::mvprintw(TOP_PADDING as i32, LEFT_PADDING as i32, "You have no saved games.");
                        display::mvprintw(
                            TOP_PADDING as i32 + self.saved_games.len() as i32 + 3,
                            LEFT_PADDING as i32,
                            "Press ENTER to continue..."
                        );
                        display::refresh();
                        input = display::getch();    //This needs to be here for the display to work correctly
                        display::invalid_window_size_handler();
                    }
                }
            }
        }
        else {
            loop {
                match input {
                    Some(display::Input::KeyEnter) => break,
                    _ => {
                        self.display_menu(
                            &Cell::new(TOP_PADDING, LEFT_PADDING), &MenuOption::SAVED_GAME_MENU(SavedGameMenuOption::NONE)
                        );

                        input = display::getch();
                        let selection: String = self.selection.borrow().to_string();
                        let i: usize = self.saved_games.binary_search(&selection.to_string()).unwrap();
                        match input {
                            Some(display::Input::KeyUp) | Some(display::Input::Character('w'))
                                => if selection.as_str() != self.saved_games.first().unwrap() {
                                    *self.selection.borrow_mut() = self.saved_games.get(i - 1).unwrap().to_string();
                            },
                            Some(display::Input::KeyDown) | Some(display::Input::Character('s'))
                                => if selection.as_str() != self.saved_games.last().unwrap() {
                                    *self.selection.borrow_mut() = self.saved_games.get(i + 1).unwrap().to_string();
                                },
                            _ => {
                                display::invalid_window_size_handler();
                            },
                        }
                    }
                }
            }
        }

        display::refresh();
        display::nodelay(false);
        display::curs_set(CURSOR_VISIBILITY::BLOCK);

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

        display::dbgprint(self.selection.borrow().as_str());
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
        display::clear();
        display::mvprintw(display_line as i32, EDGE.x() as i32, "Saved Games:");
        display_line += 2;
        for game in &self.saved_games {
            if self.selection.borrow().to_string() == *game {
                //attron(COLOR_PAIR(MENU_SELECTION));
            }
            display::mvprintw(
                display_line as i32,
                EDGE.x() as i32,
                format!("{}", game.strip_suffix(".csv").unwrap())
                    .as_str()
            );
            if self.selection.borrow().to_string() == *game {
                //attroff(COLOR_PAIR(MENU_SELECTION));
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
            println!("Error: Did not receive a DifficultyMenuOption. Exiting...");
            std::process::exit(1);
        };

        display::clear();
        display::mvprintw(EDGE.y() as i32, EDGE.x() as i32, "CHOOSE DIFFICULTY SETTING");
        for (i, variant) in DifficultyMenuOption::enumerate() {
            if *opt == variant {
                display::color_set(COLOR_PAIR::DIFFICULTY_MENU_SELECTION);
            }
            display::mvprintw((EDGE.y() + i + 2) as i32, EDGE.x() as i32, match variant {
                DifficultyMenuOption::EASY => "Easy",
                DifficultyMenuOption::MEDIUM => "Medium",
                DifficultyMenuOption::HARD => "Hard",
                DifficultyMenuOption::EXPERT => "Expert",
            });
            if *opt == variant {
                display::color_set(COLOR_PAIR::DEFAULT);
            }
        }
        display::refresh();
    }
    
    /**
     * Controls the menu display and difficulty level recording.
     */
    fn menu (&self) -> MenuOption {
        display::curs_set(CURSOR_VISIBILITY::NONE);
        let mut diff: DifficultyMenuOption = DifficultyMenuOption::EASY;
        let mut input: Option<display::Input> = None;
        display::timeout(250);
        while input != Some(display::Input::KeyEnter) {
            display::refresh();
            self.display_menu(&Cell::new(TOP_PADDING, LEFT_PADDING), &MenuOption::DIFFICULTY_MENU(diff));
            input = display::getch();
            diff = 
                match input {
                    Some(display::Input::KeyUp) | Some(display::Input::Character('w')) => match diff {
                        DifficultyMenuOption::EXPERT => DifficultyMenuOption::HARD,
                        DifficultyMenuOption::HARD => DifficultyMenuOption::MEDIUM,
                        _ => DifficultyMenuOption::EASY,
                    },
                    Some(display::Input::KeyDown) | Some(display::Input::Character('s')) => match diff {
                        DifficultyMenuOption::EASY => DifficultyMenuOption::MEDIUM,
                        DifficultyMenuOption::MEDIUM => DifficultyMenuOption::HARD,
                        _ => DifficultyMenuOption::EXPERT,
                    },
                    Some(display::Input::KeyEnter) => {
                        diff
                    },
                    _ => diff,
                };
        }
        display::nodelay(false);
        display::curs_set(CURSOR_VISIBILITY::BLOCK);

        MenuOption::DIFFICULTY_MENU(diff)
    }
}

pub struct InGameMenu {
    display_matrix: [[u8; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS],
    window_resized: RefCell<bool>,
    IN_GAME_MENU_LEFT_EDGE: u8,
    IN_GAME_MENU_TITLE_SPACING: u8,
    save_file_name: RefCell<String>,
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
            //TODO: Might need to manually copy this over like in the C++ version if weird stuff happens
            display_matrix: *display_matrix,
            window_resized: RefCell::new(false),
            IN_GAME_MENU_LEFT_EDGE: LEFT_PADDING + PUZZLE_SPACE + unsafe { VERTICAL_DIVIDER },
            IN_GAME_MENU_TITLE_SPACING: 1,
            save_file_name: RefCell::new(String::new()),
        }
    }

    /**
     * Sets whether the window has been resized during gameplay. True indicates the window was
     * resized.
     * 
     *      WINDOW_RESIZED -> Boolean indicating whether the window was resized. Sets the member
     *                        window_resized to its value.
     */
    fn set_window_resized (&self, WINDOW_RESIZED: bool) {
        &self.window_resized.replace(WINDOW_RESIZED);
    }

    /**
     * Lets the caller know whether the window was resized while in the in-game
     * menu. True indicates the window was resized.
     */
    pub fn get_window_resized (&self) -> bool {
        *self.window_resized.borrow()
    }

    /**
     * Clears the in-game menu display area.
     * 
     *      EDGE -> Line and column to start clearing from.
     */
    fn clear (&self, EDGE: Cell) {
        let in_game_menu_top_left: i32 = EDGE.y() as i32 +
            unsafe { IN_GAME_MENU_DISPLAY_SPACING as i32 } +
            InGameMenuOption::COUNT as i32 +
            2;
        for y in in_game_menu_top_left..display::get_max_y() {
            display::mv(y, EDGE.x().into());
            display::clrtoeol();
        }
    }

    /**
     * Displays the rules of sudoku.
     * 
     *      EDGE -> Line and column to start the display at.
     */
    fn display_rules (&self, EDGE: Cell) {
        let TITLE: &str = "RULES FOR PLAYING SUDOKU";
        let RULES_INTRO: String = format!("{} {} {} {} {}",
            "Sudoku is a puzzle game using the numbers 1-9. The",
            "puzzle board is a 9x9 grid that can be broken up evenly in 3",
            "different ways: rows, columns, and 3x3 boxes. A completed sudoku",
            "puzzle is one where each cell contains a number, but with the",
            "following restrictions:"
        );
        let RULES_ROWS: String = String::from("1. Each row can only contain one each of the numbers 1-9");
        let RULES_COLUMNS: String = String::from("2. Each column can only contain one each of the numbers 1-9");
        let RULES_BOXES: String = String::from("3. Each 3x3 box can only contain one each of the numbers 1-9");
        let mut display_offset: i32 = InGameMenuOption::COUNT as i32 + 2;

        display::mvprintw((EDGE.y() + unsafe { IN_GAME_MENU_DISPLAY_SPACING }) as i32 + display_offset,
            EDGE.x() as i32,
            TITLE);
        display_offset += 1;
        for string in [RULES_INTRO, RULES_ROWS, RULES_COLUMNS, RULES_BOXES] {
            display_offset += 1;
            self.screen_reader(EDGE, &string, &mut display_offset);
        }
    }

    /**
     * Prints string of text to the screen in a nicely formatted way that makes it easy to read.
     * This function parses the input string by spaces and determines when to start printing on a
     * new line (i.e. if adding another word would overlap with the window border and ruin the
     * sudoku board display).
     * 
     *      EDGE -> Line and column to start the display at.
     *      str -> Input string to be printed in the in-game menu's display area.
     *      display_offset -> Line offset to allow displaying correctly below the in-game menu
     *                        title and options.
     */
    fn screen_reader (&self, EDGE: Cell, string: &str, display_offset: &mut i32) {
        let mut display_str: String = String::new();
        for word in string.split_whitespace() {
            if display_str.len() + 1 + word.len() < unsafe { IN_GAME_MENU_DISPLAY_SPACING.into() } {
                display_str.push_str(word);
                display_str.push(' ');
            }
            else {
                display_str.pop();  // NOTE: Pop the unnecessary extra space
                display::mvprintw((EDGE.y() + self.IN_GAME_MENU_TITLE_SPACING) as i32 + *display_offset,
                    EDGE.x().into(),
                    &display_str);
                *display_offset += 1;
                display_str = String::new();
            }
        }
    }

    /**
     * Displays the tsudoku game manual.
     * 
     *      EDGE -> Line and column to start the display at.
     */
    fn display_manual (&self, EDGE: Cell) {
        let TITLE: &str = "TSUDOKU GAME MANUAL";
        let mut MANUAL_INTRO: String = String::new();
        MANUAL_INTRO.push_str("Red numbers are givens provided for you when the puzzle ");
        MANUAL_INTRO.push_str("has been generated. The number of givens present corresponds to ");
        MANUAL_INTRO.push_str("the difficulty level you have chosen. Cells with white '?' ");
        MANUAL_INTRO.push_str("symbols are empty cells which you must solve for to complete ");
        MANUAL_INTRO.push_str("the puzzle. To enter a number 1-9 into an empty cell, simply ");
        MANUAL_INTRO.push_str("move the cursor over to an empty cell and type the number. The ");
        MANUAL_INTRO.push_str("'?' symbol will be replaced with the number you entered, which ");
        MANUAL_INTRO.push_str("will be green in color. To remove a number from one of these ");
        MANUAL_INTRO.push_str("cells, move the cursor over the cell and press either the ");
        MANUAL_INTRO.push_str("Backspace or Delete keys; the green number will be replaced ");
        MANUAL_INTRO.push_str("with a '?' symbol again. The eight blank cells surrounding each ");
        MANUAL_INTRO.push_str("sudoku puzzle cell are available as a note-taking area when ");
        MANUAL_INTRO.push_str("analyzing what numbers (candidates) should go in that ");
        MANUAL_INTRO.push_str("particular cell; numbers entered in these cells will appear ");
        MANUAL_INTRO.push_str("yellow in color. Numbers in these cells can also be removed by ");
        MANUAL_INTRO.push_str("pressing either the Backspace or Delete keys while the cursor ");
        MANUAL_INTRO.push_str("is over the cell. You cannot enter anything in the note-taking ");
        MANUAL_INTRO.push_str("cells surrounding puzzle cells with red numbers. BEWARE: ");
        MANUAL_INTRO.push_str("Entering a number in a '?' occupied cell will also erase your ");
        MANUAL_INTRO.push_str("notes in the eight surrounding cells. This action cannot be ");
        MANUAL_INTRO.push_str("undone.");
        let MANUAL_M: String = String::from("m/M -> Enter/Exit the in-game manual");
        let MANUAL_Q: String = String::from("q/Q -> Quit the game without saving");
        let MANUAL_DIR_KEYS: String = String::from("Up/w/W, Down/s/S, Left/a/A, Right/d/D -> Navigate the sudoku board");
        let MANUAL_NUM: String = String::from("1-9 -> Places number in cell highlighted by cursor");
        let MANUAL_ENTER: String = String::from("Enter -> Evaluate the puzzle. Analysis will appear below puzzle.");
        let mut display_offset: i32 = InGameMenuOption::COUNT as i32 + 2;

        display::mvprintw((EDGE.y() + unsafe { IN_GAME_MENU_DISPLAY_SPACING }) as i32 + display_offset,
            EDGE.x() as i32,
            TITLE);
        display_offset += 1;
        for string in [MANUAL_INTRO, MANUAL_M, MANUAL_Q, MANUAL_DIR_KEYS, MANUAL_NUM, MANUAL_ENTER] {
            display_offset += 1;
            self.screen_reader(EDGE, &string, &mut display_offset);
        }
    }

    /**
     * Prompts the user for the name to save the game under before saving the game. Displays a
     * success message after having saved.
     * 
     *      EDGE -> Line and column to start the display at. The prompt for the save file name
     *              will start here.
     */
    fn save_game_prompt (&self, EDGE: Cell) {
        let mut display_offset: i32 = InGameMenuOption::COUNT as i32 + 2;

        display::mvprintw(
            (EDGE.y() + self.IN_GAME_MENU_TITLE_SPACING) as i32 + display_offset,
            EDGE.x().into(),
            "Enter save file name: ");
        display_offset += 2;

        display::curs_set(CURSOR_VISIBILITY::BLOCK);
        /* NOTE: This message will not be seen if there is a window resize, so no error handling
         *       required.
         */
        display::mvprintw(
            (EDGE.y() + self.IN_GAME_MENU_TITLE_SPACING) as i32 + display_offset,
            EDGE.x().into(),
            format!("{} saved!", self.save_game()).as_str()
        );
        display::curs_set(CURSOR_VISIBILITY::NONE);
    }

    /**
     * 
     */
    fn save_game (&self) -> String {
        let NAME_SIZE: usize = 16;              //NOTE: NAME_SIZE limited by window width
        let mut name: String = String::new();   //      requirements of no in-game menu mode
        display::nodelay(false);
        display::echo();
        display::getnstr(&mut name, NAME_SIZE - 1);
        display::noecho();
        display::nodelay(true);

        /* NOTE: Only save the file if the lplayer was able to enter any text first. The success
         *       message will be handled by the calling function.
         */
        if !name.is_empty() {
            self.save_file_name.replace(name.clone());
            //TODO
            name
        }
        else {
            String::from("Game not")
        }
    }
}

impl Menu for InGameMenu {
    /**
     * Displays the in-game menu. The currently selected option is always highlighted. The
     * in-game menu is re-rendered each time the user uses the Up/Down keys to highlight a 
     * ifferent option.
     * 
     *      EDGE -> Starting cell the in-game menu will display at. The menu title should display
     *              on the line below the top padding and the column after the vertical divider.
     *      OPT -> The currently highlighted main menu option.
     */
    fn display_menu (&self, EDGE: &Cell, OPT: &MenuOption) {
        let opt: &InGameMenuOption = if let MenuOption::IN_GAME_MENU(option) = OPT {
            option
        }
        else {
            println!("Error: Did not receive an InGameMenuOption. Exiting...");
            std::process::exit(1);
        };

        display::mvprintw(EDGE.y() as i32, EDGE.x() as i32, "IN-GAME MENU");
        for (i, variant) in InGameMenuOption::enumerate() {
            if *opt == variant {
                //attron(COLOR_PAIR(MENU_SELECTION));
            }
            display::mvprintw((EDGE.y() + i) as i32, EDGE.x() as i32, match variant {
                InGameMenuOption::RULES => "View the rules of sudoku",
                InGameMenuOption::MANUAL => "See game manual",
                InGameMenuOption::SAVE_GAME => "Save current game",
                InGameMenuOption::NONE => "",
            });
            if *opt == variant {
                //attroff(COLOR_PAIR(MENU_SELECTION));
            }
        }
        display::refresh();
    }

    /**
     * Controls the menu display based on the option chosen by the user.
     */
    fn menu (&self) -> MenuOption {
        display::curs_set(CURSOR_VISIBILITY::NONE);
        self.set_window_resized(false);
        let in_game_menu_left_edge: Cell = Cell::new(TOP_PADDING, self.IN_GAME_MENU_LEFT_EDGE);
        let mut opt: InGameMenuOption = InGameMenuOption::RULES;

        loop {
            display::refresh();
            self.display_menu(&in_game_menu_left_edge, &MenuOption::IN_GAME_MENU(opt));
            let input: Option<display::Input> = display::getch();
            match input {
                Some(display::Input::Character('m')) | Some(display::Input::Character('M')) => break,
                Some(display::Input::Character('w')) | Some(display::Input::Character('W')) |
                Some(display::Input::KeyUp) => opt = match opt {
                    InGameMenuOption::SAVE_GAME => InGameMenuOption::MANUAL,
                    _ => InGameMenuOption::RULES,
                },
                Some(display::Input::Character('s')) | Some(display::Input::Character('S')) |
                Some(display::Input::KeyDown) => opt = match opt {
                    InGameMenuOption::RULES => InGameMenuOption::MANUAL,
                    _ => InGameMenuOption::SAVE_GAME,
                },
                Some(display::Input::KeyEnter) => {
                    self.clear(in_game_menu_left_edge);
                    match opt {
                        InGameMenuOption::RULES => self.display_rules(in_game_menu_left_edge),
                        InGameMenuOption::MANUAL => self.display_manual(in_game_menu_left_edge),
                        InGameMenuOption::SAVE_GAME => {
                            self.display_menu(&in_game_menu_left_edge, &MenuOption::IN_GAME_MENU(InGameMenuOption::NONE));
                            self.save_game_prompt(in_game_menu_left_edge);
                        },
                        InGameMenuOption::NONE => (),
                    }
                },
                _ => if (display::invalid_window_size_handler()) {
                    self.set_window_resized(true);
                    display::mvprintw(TOP_PADDING as i32, LEFT_PADDING as i32,
                             "Press 'm' to restore the game");
                },
            }
        }

        opt = InGameMenuOption::NONE;
        self.display_menu(&in_game_menu_left_edge, &MenuOption::IN_GAME_MENU(opt));
        display::curs_set(CURSOR_VISIBILITY::BLOCK);

        MenuOption::IN_GAME_MENU(opt)
    }
}
