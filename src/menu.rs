//TODO: Copy over notes for Menu and MainMenu
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
        },
    },
    common::{
        DIR,
        csv,
    },
};
use std::{
    fs,
    cell::RefCell,
};

//NOTE: Don't use 0 with COLOR_PAIRs. This seems to have the effect of having no attribute on.
/// The COLOR_PAIR associated with the current highlighted selection in the menu.
const MENU_SELECTION: i16 = 1;

pub enum MenuOption {
    MAIN_MENU(MainMenuOption),
    SAVED_GAME_MENU(SavedGameMenuOption),
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
    //RULES,
    //MANUAL,
    //SAVE_GAME,
    //NO_SAVES,
    //NONE
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
    SAVE_READY,
    NO_SAVES,
    NONE,
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

/// displays and controls the main menu the user sees before and after every game.
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

pub struct SavedGameMenu {
    saved_games: Vec<String>,
    saved_game: [[u8; 9]; 9],
    saved_color_codes: [[char; 9]; 9],
    selection: RefCell<String>,
}

impl SavedGameMenu {
    pub fn new () -> Self {
        let saved_games: Vec<String> = Self::generate_saved_games_list();
        let selection: String = String::clone(&saved_games[0]);
        Self {
            saved_games: saved_games,
            saved_game: [[0; 9]; 9],
            saved_color_codes: [[' '; 9]; 9],
            selection: RefCell::new(selection),
        }
    }

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

    fn select_saved_game (&self) -> bool {
        let mut input: i32 = -1;
        curs_set(CURSOR_VISIBILITY::CURSOR_INVISIBLE);
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
                input = getch();    //NOTE: This needs to be here for the display to work correctly
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

    fn read_saved_game (&self) {
        let game_data: Vec<u8> = csv::read_csv(
            DIR().join(self.selection.borrow().to_string() + (".csv")).to_str().unwrap()
            )
            .unwrap();
        //TODO: Load saved game into 2D array (increment row counter when newline byte is read)
    }
}

impl Menu for SavedGameMenu {
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

    fn menu (&self) -> MenuOption {
        if self.select_saved_game() {
            //TODO
            self.read_saved_game();
        }
        //TODO: Finish this function
        MenuOption::SAVED_GAME_MENU(SavedGameMenuOption::NO_SAVES)
    }
}
