use signal_hook::{
    consts,
    low_level,
};
use strum::{
    EnumIter,
    IntoEnumIterator,
    EnumCount,
    VariantArray,
};
use crate::{
    terminal::{
        Cell,
        display::{
            self,
            ColorPair,
            CursorVisibility,
        },
    },
    common::{
        self,
        csv,
    },
    sudoku::SavedPuzzle,
};
use std::{
    fs,
    cell::RefCell,
    iter,
};

/// A wrapper enum to enforce a certain type of MenuOption be used
pub enum MenuOption {
    /// Wrapper to enforce using a MainMenuOption variant
    MainMenu(MainMenuOption),
    /// Wrapper to enforce using a SavedGameMenuOption variant
    SavedGameMenu(SavedGameMenuOption),
    /// Wrapper to enforce using a DifficultyMenuOption variant
    DifficultyMenu(DifficultyMenuOption),
    /// Wrapper to enforce using an InGameMenuOption variant
    InGameMenu(InGameMenuOption),
}

/// Options displayed on the main menu.
#[derive(PartialEq, EnumIter, EnumCount, VariantArray, Copy, Clone)]
pub enum MainMenuOption {
    /// Start a new game
    NewGame,
    /// Resume a previously saved game
    ResumeGame,
    /// Show the number of completed games
    ShowStats,
    /// Exit the program
    Exit,
}

impl MainMenuOption {
    /**
     * Returns a zipped iterator associating an 8-bit integer with each MainMenuOption variant.
     * This helps in determining the offset for displaying the options in the terminal window
     * where each 8-bit integer is the offset from the center of the screen.
     */
    fn enumerate () -> std::iter::Zip<std::ops::Range<u8>, MainMenuOptionIter> {
        (0..Self::COUNT as u8).zip(Self::iter()).into()
    }
}

///Options displayed on the saved game menu.
pub enum SavedGameMenuOption {
    /// Indicates that a saved game is available and ready to be loaded
    SaveReady,
    /// Indicates that no saved games are available to be resumed
    NoSaves,
    /// Used when a SavedGameMenuOption isn't applicable in the current context
    None,
}

/* According to https://www.101computing.net/sudoku-generator-algorithm/, the minimum
 * amount of tiles that need to be filled in in order to create a uniquely solvable
 * puzzle is 17.
 */
/// Different difficulty levels used to determing how many clues the puzzle starts with
#[derive(PartialEq, EnumIter, EnumCount, VariantArray, Copy, Clone)]
pub enum DifficultyMenuOption {
    /// The puzzle will begin with 60 clues
    Easy,
    /// The puzzle will begin with 45 clues
    Medium,
    /// The puzzle will begin with 30 clues
    Hard,
    /// The puzzle will begin with 17 clues, the minimum number for a unique solution.
    Expert,
}

impl DifficultyMenuOption {
    /**
     * Returns a zipped iterator associating an 8-bit integer with each DifficultyMenuOption
     * variant. This helps in determining the offset for displaying the options in the terminal
     * window where each 8-bit integer is the offset from the center of the screen.
     */
    fn enumerate () -> std::iter::Zip<std::ops::Range<u8>, DifficultyMenuOptionIter> {
        (0..Self::COUNT as u8).zip(Self::iter()).into()
    }
}

#[derive(PartialEq, EnumIter, EnumCount, VariantArray, Copy, Clone)]
pub enum InGameMenuOption {
    /// Display the rules of sudoku
    Rules,
    /// Display the tsudoku manual
    Manual,
    /// Save the state of the current sudoku game
    SaveGame,
    /// Don't highlight any option
    None,
}

impl InGameMenuOption {
    /**
     * Returns a zipped iterator associating an 8-bit integer with each InGameMenuOption
     * variant. This helps in determining the offset for displaying the options in the terminal
     * window where each 8-bit integer is the offset from the center of the screen.
     */
    fn enumerate () -> std::iter::Zip<std::ops::Range<u8>, InGameMenuOptionIter> {
        (0..Self::COUNT as u8).zip(Self::iter()).into()
    }
}

pub trait Menu {
    fn display_menu (&self, _: &Cell, _: &MenuOption);
    fn menu (&self) -> MenuOption;

    /**
     * Resets the terminal settings to their previous state from before the NCurses environment
     * was initialized.
     */
    fn sigint_handler () {
        display::sigint_handler();
    }
}

/// Displays and controls the main menu the user sees before and after every game.
pub struct MainMenu {
    /// Empty space between the bottom of the terminal window and the in-terminal display.
    bottom_padding: u8,

    /// Empty space between the right side of the terminal window and the in terminal display.
    right_padding: u8,

    /// Minimum number of lines required to properly display the puzzle's evaluation result.
    result_msg_space: u8,

    /// Whether the in-game menu is able to be displayed.
    in_game_menu_enabled: bool,

    /// ID of the sigint_handler function when registered via `signal_hook::register`.
    sig_id: signal_hook::SigId,
}

impl Menu for MainMenu {
    /**
     * Displays the main menu. The currently selected option is always highlighted. The main
     * menu is re-rendered each time the user uses the Up/Down keys to highlight a different
     * option.
     *
     *      MAX -> Bottom right corner cell of the terminal window. Signifies the max number of
     *             lines and columns in the window.
     *      OPT -> The currently selected main menu option.
     */
    fn display_menu (&self, max: &Cell, opt: &MenuOption) {
        let title: &str = "MAIN MENU";
        let y_center: u8 = max.y()/2;
        let x_center: u8 = max.x()/2 - title.len() as u8/2;
        let opt: &MainMenuOption = if let MenuOption::MainMenu(option) = opt {
                option
            }
            else {
                println!("Error: Did not receive a MainMenuOption. Exiting...");
                std::process::exit(1);
            };

        display::mvprintw(y_center as i32 - 2, x_center as i32, title);
        for (i, variant) in MainMenuOption::enumerate() {
            if *opt == variant {
                display::color_set(&ColorPair::MenuSelection);
            }
            display::mvprintw((y_center + i) as i32, x_center as i32, match variant {
                MainMenuOption::NewGame => "New Game",
                MainMenuOption::ResumeGame => "Resume Game",
                MainMenuOption::ShowStats => "Show # Finished Games",
                MainMenuOption::Exit => "Exit",
            });
            if *opt == variant {
                display::color_set(&ColorPair::Default);
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
                display::set_vertical_divider(0);
                display::set_in_game_menu_display_spacing(0);
            }
            self.set_window_req();
        }

        display::curs_set(CursorVisibility::None);

        let mut opt: MainMenuOption = MainMenuOption::NewGame;
        let mut input: Option<display::Input> = None;
        display::timeout(250);
        while input != Some(display::Input::KeyEnter) {
            display::invalid_window_size_handler();
            let (y_max, x_max): (i32, i32) = display::get_max_yx();
            let max: Cell = Cell::new(y_max as u8, x_max as u8);
            display::clear();
            self.display_menu(&max, &MenuOption::MainMenu(opt));
            input = display::getch();
            opt =
                match input {
                    Some(display::Input::KeyUp) | Some(display::Input::Character('w'))
                        => match opt {
                            MainMenuOption::Exit => MainMenuOption::ShowStats,
                            MainMenuOption::ShowStats => MainMenuOption::ResumeGame,
                            _ => MainMenuOption::NewGame,
                        },
                    Some(display::Input::KeyDown) | Some(display::Input::Character('s'))
                        => match opt {
                            MainMenuOption::NewGame => MainMenuOption::ResumeGame,
                            MainMenuOption::ResumeGame => MainMenuOption::ShowStats,
                            _ => MainMenuOption::Exit,
                        },
                    Some(display::Input::KeyEnter) => {
                        opt
                    },
                    _ => opt
                };
        }

        display::clear();
        display::nodelay(false);
        display::curs_set(CursorVisibility::Block);
        MenuOption::MainMenu(opt)
    }
}

impl MainMenu {
    /**
     * Initializes the NCurses environment and global NCurses settings. Returns a MainMenu
     * object with the remainder of the padding space set for the terminal window display.
     *
     *      use_in_game_menu -> Indicating whether the in-game menu is disabled or not. This
     *                          affects the enforced size of the terminal window.
     */
    pub fn new (use_in_game_menu: bool) -> MainMenu {
        let sig_id: signal_hook::SigId = unsafe {
            low_level::register(consts::SIGINT, || Self::sigint_handler())
        }.expect("Error: Signal not found");

        display::tui_init();
        display::init_color_pairs();

        Self {
            bottom_padding: display::TOP_PADDING,
            right_padding: display::LEFT_PADDING,
            result_msg_space: 3,
            in_game_menu_enabled: use_in_game_menu,
            sig_id: sig_id,
        }
    }

    /**
     * Sets the number of lines and columns the terminal window must be in order to play. This
     * size is dependent on whether the in-game menu is enabled.
     */
    unsafe fn set_window_req (&self) {
        display::WINDOW_REQ = Cell::new(
            display::TOP_PADDING + display::PUZZLE_SPACE + self.result_msg_space +
                self.bottom_padding,
            display::LEFT_PADDING + display::PUZZLE_SPACE + display::VERTICAL_DIVIDER +
                display::IN_GAME_MENU_DISPLAY_SPACING + self.right_padding,
        )
    }
}

impl Drop for MainMenu {
    /// Unsets the NCurses environment once the user chooses to exit the program.
    fn drop (&mut self) {
        display::tui_end();
        low_level::unregister(self.sig_id);
    }
}

/* The use of RefCell was a workaround during the port from the pure C++ version. There's
 * probably a better way to handle this that doesn't involve use of RefCells, but that would
 * require a much deeper refactoring than I'm willing to give it at this point.
 */
/// Provides the functionality to allow a player to load and resume a saved game.
pub struct SavedGameMenu {
    /**
     * The list of saved games to be displayed to the screen. Each will appear as an option the
     * user can choose from.
     */
    saved_games: Vec<String>,
    /// The matrix used to read a saved game into. This later becomes the display matrix.
    saved_game: RefCell<SavedPuzzle>,
    /**
     * The currently highlighted game from the displayed list. If the user presses Enter, this
     *  becomes the game loaded.
     */
    selection: RefCell<String>,
}

impl SavedGameMenu {
    /**
     * Returns an instance of a SavedGameMenu containing a list of the current saved games. The
     * saved game to be used will be initially be set to a "blank" SavedPuzzle object. The
     * current selection from the `saved_games` list will be preset to the first file in the
     * list.
     */
    pub fn new () -> Self {
        let saved_games: Vec<String> = Self::generate_saved_games_list();
        let selection: String = saved_games.first().cloned().unwrap_or(String::new());
        Self {
            saved_games: saved_games,
            saved_game: RefCell::new(SavedPuzzle::new()),
            selection: RefCell::new(selection), // NOTE: This contains the CSV extension
        }
    }

    /**
     * Creates the list of saved games from the names of available CSV files in the ~/.tsudoku
     * directory. This entries in this list are what will be displayed to the player. The
     * entries are stored without the ".csv" file extension. Text files with extension ".txt"
     * are ignored so as to avoid adding the completed games file to the list.
     */
    fn generate_saved_games_list () -> Vec<String> {
        let mut saved_games: Vec<String> = match fs::read_dir(common::game_dir()) {
            Ok(list) => list.filter(
                |item| {
                    fs::metadata(item.as_ref().unwrap().path().display().to_string())
                        .unwrap()
                        .is_dir()
                }
            )
            .map(|item| item.unwrap().file_name().to_str().unwrap().to_string())
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
     * Controls iterating through the list from player input and highlighting the name of the
     * current game that will be loaded once the player presses Enter. If there are no saved
     * games, the player will instead be notified as much and then prompted to continue.
     */
    fn select_saved_game (&self) -> bool {
        let mut input: Option<display::Input> = None;
        display::curs_set(CursorVisibility::None);  // NOTE: Turn off cursor while in menu
        display::timeout(250);
        if self.saved_games.is_empty() {
            loop {
                match input {
                    Some(display::Input::KeyEnter) => break,
                    _ => {
                        display::mvprintw(
                            display::TOP_PADDING as i32,
                            display::LEFT_PADDING as i32,
                            "You have no saved games."
                        );
                        display::mvprintw(
                            display::TOP_PADDING as i32 + self.saved_games.len() as i32 + 3,
                            display::LEFT_PADDING as i32,
                            "Press ENTER to continue..."
                        );
                        display::refresh();
                        /* NOTE: This next line needs to be here for the display to work
                         * correctly
                         */
                        input = display::getch();
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
                            &Cell::new(display::TOP_PADDING, display::LEFT_PADDING),
                            &MenuOption::SavedGameMenu(SavedGameMenuOption::None)
                        );

                        input = display::getch();
                        let selection: String = self.selection.borrow().to_string();
                        let i: usize = self.saved_games.binary_search(&selection.to_string())
                            .unwrap();
                        match input {
                            Some(display::Input::KeyUp) | Some(display::Input::Character('w'))
                                => if selection.as_str() != self.saved_games.first().unwrap() {
                                    *self.selection.borrow_mut() = self.saved_games.get(i - 1)
                                        .unwrap()
                                        .to_string();
                            },
                            Some(display::Input::KeyDown) | Some(display::Input::Character('s'))
                                => if selection.as_str() != self.saved_games.last().unwrap() {
                                    *self.selection.borrow_mut() = self.saved_games.get(i + 1)
                                        .unwrap()
                                        .to_string();
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
        display::curs_set(CursorVisibility::Block);

        !self.saved_games.is_empty()
    }

    /// Reads a saved game from its CSV files to the saved game and color code matrices.
    fn read_saved_game (&self) {
        let save_data_numeric: Vec<Vec<u8>> = csv::read(
                common::game_dir().join(self.selection.borrow().to_string())
                    .join(common::NUMERIC_DATA_FILENAME)
                    .to_str()
                    .unwrap()
            )
            .unwrap();
        let save_data_color_codes: Vec<Vec<char>> = csv::read(
                common::game_dir().join(self.selection.borrow().to_string())
                    .join(common::COLOR_DATA_FILENAME)
                    .to_str()
                    .unwrap()
            ).unwrap()
            .iter()
            .map(|row| row.iter().map(|byte| *byte as char).collect())
            .collect();

        let mut i: usize = 0;
        let mut j: usize = 0;
        let mut game_data_numeric: [
            [u8; display::DISPLAY_MATRIX_COLUMNS]; display::DISPLAY_MATRIX_ROWS
        ] = [[0; display::DISPLAY_MATRIX_COLUMNS]; display::DISPLAY_MATRIX_ROWS];
        let mut game_data_color_codes: [
            [ColorPair; display::DISPLAY_MATRIX_COLUMNS]; display::DISPLAY_MATRIX_ROWS
        ] = [
            [ColorPair::Default; display::DISPLAY_MATRIX_COLUMNS];
            display::DISPLAY_MATRIX_ROWS];
        for (row_numeric, row_color_code) in iter::zip(
            save_data_numeric,
            save_data_color_codes
        ) {
            for (number, color_code) in iter::zip(row_numeric, row_color_code) {
                game_data_numeric[i][j] = number;
                game_data_color_codes[i][j] = match color_code {
                    'u' => ColorPair::Unknown,
                    'r' => ColorPair::Given,
                    'y' => ColorPair::CandidatesY,
                    'b' => ColorPair::CandidatesB,
                    'g' => ColorPair::Guess,
                    _ => ColorPair::Default,
                };
                j += 1;
            }
            j = 0;
            i += 1;
        }

        let mut puzzle: SavedPuzzle = SavedPuzzle::new();
        puzzle.set_puzzle(game_data_numeric);
        puzzle.set_color_codes(game_data_color_codes);
        puzzle.set_filename(self.selection.borrow().as_str());
        *self.saved_game.borrow_mut() = puzzle;
    }

    /**
     * Wraps the selected saved game value and color code matrices into a SavedPuzzle object
     * which is returned to the calling function. This makes passing around the saved game
     * information easier.
     */
    pub fn get_saved_game (&self) -> SavedPuzzle {
        self.saved_game.borrow().clone()
    }
}

impl Menu for SavedGameMenu {
    /**
     * Displays the saved games menu. The options listed are saved games in CSV files from the
     * ~/.tsudoku directory. The currently selected option is always highlighted. The saved
     * games menu is re-rendered each time the player uses the Up/Down keys to highlight a
     * different option.
     * 
     *      edge -> Starting cell the saved games menu will display at. The menu title should
     *              display on the line below the top padding and the column after the vertical
     *              divider.
     *      unused MenuOption enum variant -> Required because of the function prototype 
     *                                        inherited from Menu.
     */
    fn display_menu (&self, edge: &Cell, _: &MenuOption) {
        let mut display_line: u8 = edge.y();
        display::clear();
        display::mvprintw(display_line as i32, edge.x() as i32, "Saved Games:");
        display_line += 2;
        for game in &self.saved_games {
            if self.selection.borrow().to_string() == *game {
                display::color_set(&ColorPair::MenuSelection);
            }
            display::mvprintw(
                display_line as i32,
                edge.x() as i32,
                format!("{}", game).as_str()
            );
            if self.selection.borrow().to_string() == *game {
                display::color_set(&ColorPair::Default);
            }
            display_line += 1;
        }
    }

    /**
     * Coordinates generating the saved games list, displaying the list to the player in menu
     * form, and reading in the saved game chosen by the player.
     */
    fn menu (&self) -> MenuOption {
        if self.select_saved_game() {
            self.read_saved_game();
            MenuOption::SavedGameMenu(SavedGameMenuOption::SaveReady)
        }
        else {
            MenuOption::SavedGameMenu(SavedGameMenuOption::NoSaves)
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
            difficulty_level: DifficultyMenuOption::Easy,
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
     *      edge -> Starting cell the difficulty menu will display at. The menu title should
     *              display at the origin.
     *      OPT -> The currently selected difficulty menu option.
     */
    fn display_menu (&self, edge: &Cell, opt: &MenuOption) {
        let opt: &DifficultyMenuOption = if let MenuOption::DifficultyMenu(option) = opt {
            option
        }
        else {
            println!("Error: Did not receive a DifficultyMenuOption. Exiting...");
            std::process::exit(1);
        };

        display::clear();
        display::mvprintw(edge.y() as i32, edge.x() as i32, "CHOOSE DIFFICULTY SETTING");
        for (i, variant) in DifficultyMenuOption::enumerate() {
            if *opt == variant {
                display::color_set(&ColorPair::MenuSelection);
            }
            display::mvprintw((edge.y() + i + 2) as i32, edge.x() as i32, match variant {
                DifficultyMenuOption::Easy => "Easy",
                DifficultyMenuOption::Medium => "Medium",
                DifficultyMenuOption::Hard => "Hard",
                DifficultyMenuOption::Expert => "Expert",
            });
            if *opt == variant {
                display::color_set(&ColorPair::Default);
            }
        }
        display::refresh();
    }
    
    /**
     * Controls the menu display and difficulty level recording.
     */
    fn menu (&self) -> MenuOption {
        display::curs_set(CursorVisibility::None);
        let mut diff: DifficultyMenuOption = DifficultyMenuOption::Easy;
        let mut input: Option<display::Input> = None;
        display::timeout(250);
        while input != Some(display::Input::KeyEnter) {
            display::invalid_window_size_handler();
            display::refresh();
            self.display_menu(
                &Cell::new(display::TOP_PADDING, display::LEFT_PADDING),
                &MenuOption::DifficultyMenu(diff)
            );
            input = display::getch();
            diff = 
                match input {
                    Some(display::Input::KeyUp) | Some(display::Input::Character('w'))
                        => match diff {
                            DifficultyMenuOption::Expert => DifficultyMenuOption::Hard,
                            DifficultyMenuOption::Hard => DifficultyMenuOption::Medium,
                            _ => DifficultyMenuOption::Easy,
                        },
                        Some(display::Input::KeyDown) | Some(display::Input::Character('s'))
                        => match diff {
                            DifficultyMenuOption::Easy => DifficultyMenuOption::Medium,
                            DifficultyMenuOption::Medium => DifficultyMenuOption::Hard,
                            _ => DifficultyMenuOption::Expert,
                        },
                    Some(display::Input::KeyEnter) => {
                        diff
                    },
                    _ => diff,
                };
        }
        display::nodelay(false);
        display::curs_set(CursorVisibility::Block);

        MenuOption::DifficultyMenu(diff)
    }
}

pub struct InGameMenu {
    display_matrix: [[u8; display::DISPLAY_MATRIX_COLUMNS]; display::DISPLAY_MATRIX_ROWS],
    color_codes: [[ColorPair; display::DISPLAY_MATRIX_COLUMNS]; display::DISPLAY_MATRIX_ROWS],
    window_resized: RefCell<bool>,
    in_game_menu_left_edge: u8,
    in_game_menu_title_spacing: u8,
    save_file_name: RefCell<String>,
}

impl InGameMenu {
    /**
     * Initializes the display matrix, so the in-game menu can track any changes made during
     * gameplay.
     * 
     *      display_matrix -> The display matrix from the main gameplay loop in Sudoku.
     */
    pub fn new (
        display_matrix: &[[u8; display::DISPLAY_MATRIX_COLUMNS]; display::DISPLAY_MATRIX_ROWS],
        color_codes: &[
            [ColorPair; display::DISPLAY_MATRIX_COLUMNS]; display::DISPLAY_MATRIX_ROWS
        ],
        save_file_name: &str,
    ) -> Self {
        Self {
            /* TODO: Might need to manually copy this over like in the C++ version if weird
             *       stuff happens
             */
            display_matrix: *display_matrix,
            color_codes: *color_codes,
            window_resized: RefCell::new(false),
            in_game_menu_left_edge: display::LEFT_PADDING +
                display::PUZZLE_SPACE +
                unsafe { display::VERTICAL_DIVIDER },
            in_game_menu_title_spacing: 1,
            save_file_name: RefCell::new(String::from(save_file_name)),
        }
    }

    /**
     * Sets whether the window has been resized during gameplay. True indicates the window was
     * resized.
     * 
     *      window_resized -> Boolean indicating whether the window was resized. Sets the member
     *                        window_resized to its value.
     */
    fn set_window_resized (&self, window_resized: bool) {
        let _ = &self.window_resized.replace(window_resized);
    }

    /**
     * Lets the caller know whether the window was resized while in the in-game menu. True
     * indicates the window was resized.
     */
    pub fn get_window_resized (&self) -> bool {
        *self.window_resized.borrow()
    }

    /**
     * Clears the in-game menu display area.
     * 
     *      edge -> Line and column to start clearing from.
     */
    fn clear (&self, edge: Cell) {
        let in_game_menu_top_left: i32 = edge.y() as i32 + InGameMenuOption::COUNT as i32 + 1;
        for y in in_game_menu_top_left..display::get_max_y() {
            display::mv(y, edge.x().into());
            display::clrtoeol();
        }
    }

    /**
     * Displays the rules of sudoku.
     * 
     *      edge -> Line and column to start the display at.
     */
    fn display_rules (&self, edge: Cell) {
        let title: &str = "RULES FOR PLAYING SUDOKU";
        let rules_intro: String = String::from("
Sudoku is a puzzle game using the numbers 1-9. The puzzle board is a 9x9 grid that can be broken
up evenly in 3 different ways: rows, columns, and 3x3 boxes. A completed sudoku puzzle is one
where each cell contains a number, but with the following restrictions:");
        let rules_rows: String = String::from(
            "1. Each row can only contain one each of the numbers 1-9"
        );
        let rules_columns: String = String::from(
            "2. Each column can only contain one each of the numbers 1-9"
        );
        let rules_boxes: String = String::from(
            "3. Each 3x3 box can only contain one each of the numbers 1-9"
        );
        let mut display_offset: i32 = InGameMenuOption::COUNT as i32 + 1;

        display::mvprintw(edge.y() as i32 + display_offset, edge.x() as i32, title);
        display_offset += 1;
        for string in [rules_intro, rules_rows, rules_columns, rules_boxes] {
            display_offset += 1;
            self.screen_reader(edge, &string, &mut display_offset);
        }
    }

    /**
     * Prints string of text to the screen in a nicely formatted way that makes it easy to read.
     * This function parses the input string by spaces and determines when to start printing on
     * a new line (i.e. if adding another word would overlap with the window border and ruin the
     * sudoku board display).
     * 
     *      edge -> Line and column to start the display at.
     *      string -> Input string to be printed in the in-game menu's display area.
     *      display_offset -> Line offset to allow displaying correctly below the in-game menu
     *                        title and options.
     */
    fn screen_reader (&self, edge: Cell, string: &str, display_offset: &mut i32) {
        let mut display_str: String = String::new();
        for word in string.split_whitespace() {
            if display_str.len() + 1 + word.len() < unsafe {
                display::IN_GAME_MENU_DISPLAY_SPACING.into()
            } {
                display_str.push_str(word);
                display_str.push(' ');
            }
            else {
                display_str.pop();  // NOTE: Pop the unnecessary extra space
                display::mvprintw(
                    (edge.y() + self.in_game_menu_title_spacing) as i32 + *display_offset,
                    edge.x().into(),
                    &display_str
                );
                *display_offset += 1;
                display_str = String::from(word);
                display_str.push(' ');
            }
        }
        if !display_str.is_empty() {
            display::mvprintw(
                (edge.y() + self.in_game_menu_title_spacing) as i32 + *display_offset,
                edge.x().into(),
                &display_str
            );
            *display_offset += 1;
        }
    }

    /**
     * Displays the tsudoku game manual.
     * 
     *      edge -> Line and column to start the display at.
     */
    fn display_manual (&self, edge: Cell) {
        let title: &str = "TSUDOKU GAME MANUAL";
        let manual_intro: String = String::from("Red numbers are givens provided for you when
the puzzle has been generated. The number of givens present corresponds to the difficulty level
you have chosen. Cells with white '?' symbols are empty cells which you must solve for to
complete the puzzle. To enter a number 1-9 into an empty cell, simply move the cursor over to an
empty cell and type the number. The '?' symbol will be replaced with the number you entered,
which will be green in color. To remove a number from one of these cells, move the cursor over
the cell and press either the Backspace or Delete keys; the green number will be replaced with a
'?' symbol again. The eight blank cells surrounding each sudoku puzzle cell are available as a
note-taking area when analyzing what numbers (candidates) should go in that particular cell;
numbers entered in these cells will appear yellow in color. Numbers in these cells can also be
removed by pressing either the Backspace or Delete keys while the cursor is over the cell. You
cannot enter anything in the note-taking cells surrounding puzzle cells with red numbers. BEWARE:
Entering a number in a '?' occupied cell will also erase your notes in the eight surrounding
cells. This action cannot be undone.");
        let manual_m: String = String::from("m/M -> Enter/Exit the in-game manual");
        let manual_q: String = String::from("q/Q -> Quit the game without saving");
        let manual_dir_keys: String = String::from(
            "Up/w/W, Down/s/S, Left/a/A, Right/d/D -> Navigate the sudoku board"
        );
        let manual_num: String = String::from(
            "1-9 -> Places number in cell highlighted by cursor"
        );
        let manual_enter: String = String::from(
            "Enter -> Evaluate the puzzle. Analysis will appear below puzzle."
        );
        let mut display_offset: i32 = InGameMenuOption::COUNT as i32 + 1;

        display::mvprintw(edge.y() as i32 + display_offset, edge.x() as i32, title);
        display_offset += 1;
        for string in [
            manual_intro, manual_m, manual_q, manual_dir_keys, manual_num, manual_enter
        ] {
            display_offset += 1;
            self.screen_reader(edge, &string, &mut display_offset);
        }
    }

    /**
     * Prompts the user for the name to save the game under before saving the game. Displays a
     * success message after having saved.
     * 
     *      edge -> Line and column to start the display at. The prompt for the save file name
     *              will start here.
     */
    fn save_game_prompt (&self, edge: Cell) {
        let mut display_offset: i32 = InGameMenuOption::COUNT as i32 + 1;

        display::mvprintw(
            edge.y() as i32 + display_offset,
            edge.x().into(),
            "Enter save file name: ");
        display_offset += 2;

        display::curs_set(CursorVisibility::Block);
        /* NOTE: Saving will not be interrupted until the user hits Enter, but this message will
         *       not be seen if there is a window resize, so no error handling required.
         */
        let new_name: String = self.save_game();
        display::mvprintw(
            (edge.y() + self.in_game_menu_title_spacing) as i32 + display_offset,
            edge.x().into(),
            format!(
                "{} saved!",
                if !new_name.is_empty() {
                    new_name
                }
                else {
                    String::from("Game not")
                }
            ).as_str()
        );
        display::curs_set(CursorVisibility::None);
    }

    /**
     * Saves the current game, first waiting for the user to input the name they wish to save
     * the game under. If the user enters nothing, saving is aborted and a message will be
     * displayed saying the game wasn't saved. If the user enters a name, the game data is
     * saved inside a directory of the same name, with numbers and color codes saved to
     * different files. The name the user entered is returned to the calling function.
     */
    pub fn save_game (&self) -> String {
        // NOTE: name_size limited by window width requirements of no in-game menu mode
        let name_size: usize = 16;
        let mut name: String = self.save_file_name();
        display::nodelay(false);
        display::echo();
        display::getnstr(&mut name, name_size - 1);
        display::noecho();
        display::nodelay(true);

        /* NOTE: Only save the file if the player was able to enter any text first. The success
         *       message will be handled by the calling function.
         */
        // TODO: Does COLOR_PAIR::DEFAULT need to be handled here?
        if !name.is_empty() {
            let mut color_codes: Vec<[char; display::DISPLAY_MATRIX_ROWS]> = Vec::new();
            for row in self.color_codes {
                let colors: [char; display::DISPLAY_MATRIX_ROWS] = row.map(|color| match color {
                    ColorPair::Unknown => 'u',
                    ColorPair::Given => 'r',
                    ColorPair::CandidatesY => 'y',
                    ColorPair::CandidatesB => 'b',
                    ColorPair::Guess => 'g',
                    _ => 'n',
                });
                color_codes.push(colors);
            }
            csv::write(&name, common::NUMERIC_DATA_FILENAME, &self.display_matrix.to_vec());
            csv::write(&name, common::COLOR_DATA_FILENAME, &color_codes);
            self.save_file_name.replace(name.clone());
        }
        name
    }

    /**
     * Returns the name the game is currently saved under as a `String`.
     */
    pub fn save_file_name (&self) -> String {
        self.save_file_name.borrow().to_string()
    }
}

impl Menu for InGameMenu {
    /**
     * Displays the in-game menu. The currently selected option is always highlighted. The
     * in-game menu is re-rendered each time the user uses the Up/Down keys to highlight a 
     * different option.
     * 
     *      edge -> Starting cell the in-game menu will display at. The menu title should
     *              display on the line below the top padding and the column after the vertical
     *              divider.
     *      opt -> The currently highlighted main menu option.
     */
    fn display_menu (&self, edge: &Cell, opt: &MenuOption) {
        let opt: &InGameMenuOption = if let MenuOption::InGameMenu(option) = opt {
            option
        }
        else {
            println!("Error: Did not receive an InGameMenuOption. Exiting...");
            display::tui_end();
            std::process::exit(1);
        };

        display::mvprintw(edge.y() as i32, edge.x() as i32, "IN-GAME MENU");
        for (i, variant) in InGameMenuOption::enumerate() {
            if *opt == variant {
                display::color_set(&ColorPair::MenuSelection);
            }
            display::mvprintw((edge.y() + i) as i32, edge.x() as i32, match variant {
                InGameMenuOption::Rules => "View the rules of sudoku",
                InGameMenuOption::Manual => "See game manual",
                InGameMenuOption::SaveGame => "Save current game",
                InGameMenuOption::None => "",
            });
            if *opt == variant {
                display::color_set(&ColorPair::Default);
            }
        }
        display::refresh();
    }

    /**
     * Controls the menu display based on the option chosen by the user.
     */
    fn menu (&self) -> MenuOption {
        display::curs_set(CursorVisibility::None);
        self.set_window_resized(false);
        let in_game_menu_left_edge: Cell = Cell::new(
            display::TOP_PADDING,
            self.in_game_menu_left_edge
        );
        let mut opt: InGameMenuOption = InGameMenuOption::Rules;

        loop {
            display::refresh();
            self.display_menu(&in_game_menu_left_edge, &MenuOption::InGameMenu(opt));
            let input: Option<display::Input> = display::getch();
            match input {
                Some(display::Input::Character('m')) | Some(display::Input::Character('M'))
                    => break,
                Some(display::Input::Character('w')) | Some(display::Input::Character('W')) |
                Some(display::Input::KeyUp) => opt = match opt {
                    InGameMenuOption::SaveGame => InGameMenuOption::Manual,
                    _ => InGameMenuOption::Rules,
                },
                Some(display::Input::Character('s')) | Some(display::Input::Character('S')) |
                Some(display::Input::KeyDown) => opt = match opt {
                    InGameMenuOption::Rules => InGameMenuOption::Manual,
                    _ => InGameMenuOption::SaveGame,
                },
                Some(display::Input::KeyEnter) => {
                    self.clear(in_game_menu_left_edge);
                    match opt {
                        InGameMenuOption::Rules => self.display_rules(in_game_menu_left_edge),
                        InGameMenuOption::Manual => self.display_manual(in_game_menu_left_edge),
                        InGameMenuOption::SaveGame => {
                            self.display_menu(
                                &in_game_menu_left_edge,
                                &MenuOption::InGameMenu(InGameMenuOption::None)
                            );
                            self.save_game_prompt(in_game_menu_left_edge);
                        },
                        InGameMenuOption::None => (),
                    }
                },
                _ => if display::invalid_window_size_handler() {
                    self.set_window_resized(true);
                    display::mvprintw(
                        display::TOP_PADDING as i32,
                        display::LEFT_PADDING as i32,
                        "Press 'm' to restore the game"
                    );
                },
            }
        }

        opt = InGameMenuOption::None;
        self.display_menu(&in_game_menu_left_edge, &MenuOption::InGameMenu(opt));
        display::curs_set(CursorVisibility::Block);

        MenuOption::InGameMenu(opt)
    }
}
