use crate::{
    terminal::{
        display::{
            self,
            CURSOR_VISIBILITY,
            COLOR_PAIR,
        },
        Cell,
    },
    menu::{
        DifficultyMenu,
        Menu,
        DifficultyMenuOption,
        MenuOption,
        //MENU_SELECTION,
        InGameMenu,
    },
};
use pancurses as pc;
use std::{
    collections::HashMap,
    array,
    cell::RefCell,
};
use rand::{
    thread_rng,
    distributions::Uniform,
    seq::SliceRandom,
};
use queues::{
    Queue,
    IsQueue,
};

extern "C" {
    fn time (_: i32) -> i32;
}

const GRID_SIZE: u8 = 81;
const NUM_CONTAINERS: u8 = 9;
const CONTAINER_SIZE: u8 = 9;

enum _neighbor_cells {
    TL,
    T,
    TR,
    L,
    R,
    BL,
    B,
    BR,
}

impl _neighbor_cells {
    const fn _NUM_BORDER_POSITIONS () -> u8 {
        8
    }
}

/// Holds data for a saved game that is selected to be resumed
#[derive(Clone)]
pub struct SavedPuzzle {
    /// A 9x9 matrix containing the values in the puzzle cells
    puzzle: [[u8; display::DISPLAY_MATRIX_COLUMNS]; display::DISPLAY_MATRIX_ROWS],
    /// A 9x9 matrix containing the current color codes of the puzzle cells
    color_codes: [[char; display::DISPLAY_MATRIX_COLUMNS]; display::DISPLAY_MATRIX_ROWS],
    /// The name of the file the puzzle is saved under
    filename: String,
}

impl SavedPuzzle {
    /// Returns an "empty" SavedPuzzle (zeroed/spaced arrays and empty filename).
    pub fn new () -> Self {
        Self {
            puzzle: [[0; display::DISPLAY_MATRIX_COLUMNS]; display::DISPLAY_MATRIX_ROWS],
            color_codes: [[' '; display::DISPLAY_MATRIX_COLUMNS]; display::DISPLAY_MATRIX_ROWS],
            filename: String::new(),
        }
    }

    /// Stores the values of the saved puzzle into an array.
    pub fn set_puzzle (&mut self,
        puzzle: [[u8; display::DISPLAY_MATRIX_COLUMNS]; display::DISPLAY_MATRIX_ROWS]) {
        self.puzzle = puzzle;
    }

    /// Stores the color codes of the saved puzzle into an array.
    pub fn set_color_codes (&mut self,
        color_codes: [[char; display::DISPLAY_MATRIX_COLUMNS]; display::DISPLAY_MATRIX_ROWS]) {
        self.color_codes = color_codes;
    }

    /// Sets the filename the saved puzzle has been stored in.
    pub fn set_filename (&mut self, filename: &str) {
        self.filename = String::from(filename);
    }
}

/**
 * Represents an interactive live game of sudoku.
 * 
 *      display_matrix -> 27x27 matrix of 8-bit characters that is displayed to the screen during
 *                        play. This is the data structure the user directly interacts with.
 *      grid -> Grid object representing just the 81 cells of a sudoku board.
 *      grid2display_map -> Mapping of 81 positions of a grid to their (y, x) coordinates in the
 *                          display matrix.
 *      display2grid_map -> Reverse mapping of _map_.
 *      ORIGIN -> Starting cell of the display matrix's (0, 0) position on the actual terminal
 *                window. This is effectively the row and column offset from the top left cell of the
 *                terminal and also controls the size of the buffers from the edges of the terminal
 *                window.
 *                NOTE: Terminal coordinates are in (y,x) format. Origin coordinates can be found in
 *                      misc.hpp
 *      cursor_pos -> The current position of the cursor on the display matrix. At the start of the
 *                    game, this is equivalent to the ORIGIN, and his is constantly tracked and
 *                    updated every time the cursor moves afterwards. This is primarily beneficial
 *                    for resetting (with the appearance of maintaining) the cursor on the current
 *                    position after reacting to user input (i.e. this object's primary purpose is to
 *                    make the cursor appear as if it never moved after the user inputs a character
 *                    on the display matrix).
 *      display_matrix_offset -> Mapping of the display matrix's index pairs to their (y, x)
 *                               positions displayed in the terminal window. This makes the math
 *                               behind making position-related changes easier. display_matrix[0][0]
 *                               is mapped to the ORIGIN and so forth.
 *      neighbor_cells -> Enumeration of shorthand constants used when retrieving a cell's 8
 *                          surrounding (i.e. border) cells along with a constant for the number of
 *                          border positions.
 */
pub struct Sudoku {
    display_matrix: [[u8; display::DISPLAY_MATRIX_COLUMNS]; display::DISPLAY_MATRIX_ROWS],
    color_codes: [[COLOR_PAIR; display::DISPLAY_MATRIX_COLUMNS]; display::DISPLAY_MATRIX_ROWS],
    grid: Grid,
    grid2display_map: HashMap<u8, Cell>,
    display2grip_map: HashMap<Cell, u8>,
    ORIGIN: Cell,
    cursor_pos: Cell,
    offset2actual: HashMap<Cell, Cell>,
    save_file_name: RefCell<String>,
}

impl Sudoku {
    /**
     * Returns a Sudoku instance, a live interactive game of sudoku. Also coordinates 
     * setup of color mappings and display matrix initialization.
     */
    pub fn new (saved_puzzle: Option<&SavedPuzzle>) -> Self {
        display::init_color_pairs();
        let (grid2display, display2grid) = Self::create_maps();
        //TODO: Continue testing from here
        /*let color_codes: [
            [display::COLOR_PAIR; display::DISPLAY_MATRIX_COLUMNS];
            display::DISPLAY_MATRIX_ROWS
        ] = Self::create_color_codes(&grid2display);*/
        /*
        println!("The maps {} the same size...",
            if grid2display.len() == display2grid.len() {
                "are"
            }
            else {
                "are not"
            }
        );
        for i in 0..grid2display.len() as u8 {
            let (_, val) = grid2display.get_key_value(&i).unwrap();
            println!("{:?} -> {:?}",
                grid2display.get_key_value(&i).unwrap(),
                display2grid.get_key_value(&val).unwrap()
            );
        }
        println!("hello there...");
        std::process::exit(0);*/
        //TODO: The display matrix isn't looking right
        let (display_matrix, grid) = Self::init_display_matrix(saved_puzzle, &grid2display);
        let offset2actual: HashMap<Cell, Cell> = Self::map_display_matrix_offset();
        /*display::tui_end();
        println!("display_matrix:\n");
        for row in display_matrix {
            println!("{:?}", row);
        }
        std::process::exit(0);*/
        /*for i in 0..display::DISPLAY_MATRIX_ROWS as u8 {
            for j in 0..display::DISPLAY_MATRIX_COLUMNS as u8 {
                let (actual, offset): (&Cell, &Cell) = display_matrix_offset
                    .get_key_value(&Cell::new(i, j))
                    .unwrap();
                println!("{:?}: {:?}", actual, offset);
            }
        }*/
        Self {
            display_matrix: display_matrix,
            color_codes: [
                [COLOR_PAIR::UNKNOWN; display::DISPLAY_MATRIX_COLUMNS];
                display::DISPLAY_MATRIX_ROWS],
            grid: grid,
            grid2display_map: grid2display,
            display2grip_map: display2grid,
            ORIGIN: display::ORIGIN,
            cursor_pos: display::ORIGIN,
            offset2actual: offset2actual,
            save_file_name: RefCell::new(match saved_puzzle {
                Some(puzzle) => puzzle.filename.clone(),
                None => String::new(),
            }),
        }
    }

    /**
     * Creates a mapping between the 81 cells in a grid to their positions in the display
     * matrix. A reverse mapping is also created simultaneously. This mapping assumes a
     * display matrix origin of (0, 0), and a offset is applied later during actual
     * display.
     */
    fn create_maps () -> (HashMap<u8, Cell>, HashMap<Cell, u8>) {
        let mut row: u8 = 1;
        let mut col: u8 = 1;

        let mut grid2display: HashMap<u8, Cell> = HashMap::new();
        for i in 0..GRID_SIZE {
            grid2display.insert(i, Cell::new(row, col));
            col += 3;
            if col / display::DISPLAY_MATRIX_COLUMNS as u8 != 0 {
                col %= display::DISPLAY_MATRIX_COLUMNS as u8;
                row += 3;
            }
        }

        let mut display2grid: HashMap<Cell, u8> = HashMap::new();
        for i in 0..GRID_SIZE {
            let (key, value) = grid2display.get_key_value(&i)
                .expect(format!(
                    "Key '{}' not found while creating display2grid map...", i).as_str()
                );
            display2grid.insert(*value, *key);
        }

        (grid2display, display2grid)
    }

    /**
     * 
     */
    /*fn create_color_codes (grid2display: &HashMap<u8, Cell>) -> [
        [COLOR_PAIR; display::DISPLAY_MATRIX_COLUMNS];
        display::DISPLAY_MATRIX_ROWS
    ] {
        let color_codes = [[COLOR_PAIR::UNKNOWN; display::DISPLAY_MATRIX_COLUMNS]; display::DISPLAY_MATRIX_ROWS];
        let mut cells = grid2display.values();
        for i in 0..display::DISPLAY_MATRIX_ROWS as u8 {
            for j in 0..display::DISPLAY_MATRIX_COLUMNS as u8 {
                match cells.find(|&&cell| cell == Cell::new(i, j)) {
                    Some(_) => color_codes[i][j] = COLOR_PAIR::GIVEN,
                    None => todo!(),
                }
            }
        }
        color_codes
    }*/

    /**
     * Initialiizes the display matrix with either a newly generated puzzle or a saved
     * game.
     * 
     *      saved_puzzle -> Pointer to a SavedPuzzle object that represents a previously
     *                      saved game. If the user has selected to start a new game,
     *                      this will be a nullptr. If the user has selected to resume a
     *                      saved game, this object will be read in beforehand.
     */
    fn init_display_matrix (saved_puzzle: Option<&SavedPuzzle>, grid2display: &HashMap<u8, Cell>)
        -> ([[u8; display::DISPLAY_MATRIX_COLUMNS]; display::DISPLAY_MATRIX_ROWS], Grid) {
        /* This is a display matrix indeces "cheat sheet", with Grid cells mapped out.
         * This will display as intended if looking at it full screen with 1920x1080
         * screen dimensions.
         * 
         *   0,0  0,1  0,2  0,3  0,4  0,5  0,6  0,7  0,8 |  0,9  0,10  0,11  0,12  0,13  0,14  0,15  0,16  0,17 |  0,18  0,19  0,20  0,21  0,22  0,23  0,24  0,25  0,26
         *   1,0  1,1            1,4            1,7      |       1,10              1,13              1,16       |        1,19              1,22              1,25
         *   2,0                                         |                                                      |
         *   3,0                                         |                                                      |
         *   4,0  4,1            4,4            4,7      |       4,10              4,13              4,16       |        4,19              4,22              4,25
         *   5,0                                         |                                                      |
         *   6,0                                         |                                                      |
         *   7,0  7,1            7,4            7,7      |       7,10              7,13              7,16       |        7,19              7,22              7,25
         *   8,0                                         |                                                      |
         *  ---------------------------------------------|------------------------------------------------------|------------------------------------------------------
         *   9,0                                         |                                                      |
         *  10,0 10,1           10,4           10,7      |      10,10             10,13             10,16       |       10,19             10,22             10,25
         *  11,0                                         |                                                      |
         *  12,0                                         |                                                      |
         *  13,0 13,1           13,4           13,7      |      13,10             13,13             13,16       |       13,19             13,22             13,25
         *  14,0                                         |                                                      |
         *  15,0                                         |                                                      |
         *  16,0 16,1           16,4           16,7      |      16,10             16,13             16,16       |       16,19             16,22             16,25
         *  17,0                                         |                                                      |
         *  ---------------------------------------------|------------------------------------------------------|------------------------------------------------------
         *  18,0                                         |                                                      |
         *  19,0 19,1           19,4           19,7      |      19,10             19,13             19,16       |       19,19             19,22             19,25
         *  20,0                                         |                                                      |
         *  21,0                                         |                                                      |
         *  22,0 22,1           22,4           22,7      |      22,10             22,13             22,16       |       22,19             22,22             22,25
         *  23,0                                         |                                                      |
         *  24,0                                         |                                                      |
         *  25,0 25,1           25,4           25,7      |      25,10             25,13             25,16       |       25,19             25,22             25,25
         *  26,0                                         |                                                      |
         */
        match saved_puzzle {
            Some(_puzzle) => todo!(),
            None => {
                let mut mat:
                    [[u8; display::DISPLAY_MATRIX_COLUMNS]; display::DISPLAY_MATRIX_ROWS] =
                    [
                        [' ' as u8; display::DISPLAY_MATRIX_COLUMNS]
                        ; display::DISPLAY_MATRIX_ROWS
                    ];

                let mut diff_menu: DifficultyMenu = DifficultyMenu::new();
                if let MenuOption::DIFFICULTY_MENU(diff) = diff_menu.menu() {
                    diff_menu.set_difficulty_level(diff);
                }
                //TODO: The display matrix isn't looking quite right
                let grid: Grid = Grid::new(diff_menu.get_difficulty_level());
                for (i, cell) in grid2display {
                    mat[cell.y() as usize ][cell.x() as usize] = grid.at(*i);
                }

                (mat, grid)
            },
        }
    }

    /**
     * Creates a mapping between a cell in the display matrix and it's actual location on the
     * screen. Returns a `HashMap` containing a mapping of display offset coordinates to actual
     * non-offset coordinates.
     */
    fn map_display_matrix_offset () -> HashMap<Cell, Cell> {
        let mut offset2actual: HashMap<Cell, Cell> = HashMap::new();
        for i in 0..display::DISPLAY_MATRIX_ROWS as u8 {
            for j in 0..display::DISPLAY_MATRIX_COLUMNS as u8 {
                let TOTAL_OFFSETY: u8 = i + display::ORIGIN.y() + (i / CONTAINER_SIZE);
                let TOTAL_OFFSETX: u8 = j + display::ORIGIN.x() + (j / CONTAINER_SIZE);
                let actual: Cell = Cell::new(i, j);
                let offset: Cell = Cell::new(TOTAL_OFFSETY, TOTAL_OFFSETX);
                /* TODO: These may need to be switched? I thought I had it the other way (where
                 *       the actual gave you the offset), but the code makes it look like the
                 *       offset gives you the actual.
                 * TODO: Rename this to offset2actual? or actual2offset?
                 */
                offset2actual.insert(offset, actual);
            }
        }
        offset2actual
    }

    /**
     * 
     */
    pub fn start_game (&mut self, USE_IN_GAME_MENU: bool, SAVED_PUZZLE: Option<&SavedPuzzle>) {
        self.init_display(SAVED_PUZZLE);
        let LINE_OFFSET_TWEAK: u8 = 3;  // NOTE: # lines to get display output correct
        let DELAY: u8 = 2;              // NOTE: # seconds to delay after printing out results

        self.display_hotkey(USE_IN_GAME_MENU, LINE_OFFSET_TWEAK);
        display::mv(display::ORIGIN.y().into(), display::ORIGIN.x().into());
        self.cursor_pos.set(display::ORIGIN.y(), display::ORIGIN.x());
        display::refresh();

        let mut quit_game: bool = false;
        //nodelay(stdscr, true);
        display::timeout(250);
        while !quit_game {
            let input: Option<display::Input> = display::getch();
            match input {
                Some(display::Input::Character('q')) |
                Some(display::Input::Character('Q')) => quit_game = true,
                Some(display::Input::Character('m')) |
                Some(display::Input::Character('M')) => if USE_IN_GAME_MENU {
                    //TODO: Make this reusable somehow like in the C++ version...
                    let in_game_menu: InGameMenu = InGameMenu::new(&self.display_matrix);

                    //attron(COLOR_PAIR(MENU_SELECTION));
                    display::mvprintw(
                        display::get_max_y() - LINE_OFFSET_TWEAK as i32,
                        display::ORIGIN.x() as i32,
                         "m -> return to game"
                    );
                    //attroff(COLOR_PAIR(MENU_SELECTION));
                    display::clrtoeol();

                    in_game_menu.menu();
                    //NOTE: Save cursor position before (potentially) needing to reprint the puzzle
                    let saved_pos: Cell = self.cursor_pos;
                    if in_game_menu.get_window_resized() {
                        self.printw();
                    }

                    //NOTE: Toggle hotkey back to original meaning when leaving in-game menu
                    //attron(COLOR_PAIR(MENU_SELECTION));
                    display::mvprintw(
                        display::get_max_y() - LINE_OFFSET_TWEAK as i32,
                        display::ORIGIN.x().into(),
                        "m -> in-game menu"
                    );
                    //attroff(COLOR_PAIR(MENU_SELECTION));
                    display::clrtoeol();

                    display::refresh();
                    self.cursor_pos = saved_pos;
                    self.reset_cursor();
                },
                Some(display::Input::Character('s')) |
                Some(display::Input::Character('S')) => if !USE_IN_GAME_MENU {
                    self.save_game_prompt(DELAY);
                    self.reset_cursor();
                },
                Some(display::Input::KeyUp)    | Some(display::Input::Character('w')) |
                Some(display::Input::KeyDown)  | Some(display::Input::Character('s')) |
                Some(display::Input::KeyLeft)  | Some(display::Input::Character('a')) |
                Some(display::Input::KeyRight) | Some(display::Input::Character('d')) => {
                    //TODO
                },
                Some(display::Input::Character('1')) | Some(display::Input::Character('2')) |
                Some(display::Input::Character('3')) | Some(display::Input::Character('4')) |
                Some(display::Input::Character('5')) | Some(display::Input::Character('6')) |
                Some(display::Input::Character('7')) | Some(display::Input::Character('8')) |
                Some(display::Input::Character('9')) => {
                    //TODO
                },
                Some(display::Input::KeyBackspace) | Some(display::Input::KeyDC) => {
                    //TODO
                }
                Some(display::Input::KeyEnter) => {
                    //TODO
                }
                _ => if display::invalid_window_size_handler() {
                    let curr_pos: Cell = self.cursor_pos;
                    self.printw();
                    self.cursor_pos = curr_pos;
                    self.display_hotkey(USE_IN_GAME_MENU, LINE_OFFSET_TWEAK);
                    self.reset_cursor();
                }
            };
        }
        display::nodelay(false);
    }

    /**
     * Formerly Sudoku::printw(SavedPuzzle*) from the C++ version. Prints the entire sudoku
     * puzzle (the display matrix) to the screen for initial viewing.
     * 
     *      SAVED_PUZZLE -> Pointer to a SavedPuzzle object that represents a previously saved
     *                      game. If the user has selected to start a new game, this will be a
     *                      nullptr. If the user has selected to resume a saved game, this object
     *                      will be read in beforehand.
     */
    fn init_display (&mut self, SAVED_PUZZLE: Option<&SavedPuzzle>) {
        for i in 0..display::DISPLAY_MATRIX_ROWS{
            self.mv(Cell::new(i as u8, 0));
            for j in 0..display::DISPLAY_MATRIX_COLUMNS {
                //TODO: GET RID OF THIS self.map_display_matrix_offset(Cell::new(i as u8, j as u8));
                /*let mut color_pair: i16 = UNKNOWN;
                if let Some(saved_puzzle) = SAVED_PUZZLE {
                    if saved_puzzle.color_codes[i][j] == 'u' {
                        color_pair
                    }
                    //TODO
                }*/
                let color_pair: COLOR_PAIR = if let Some(saved_puzzle) = SAVED_PUZZLE {
                    match saved_puzzle.color_codes[i][j] {
                        'u' => COLOR_PAIR::UNKNOWN,
                        'r' => COLOR_PAIR::GIVEN,
                        'y' => {
                            //nc::attron(nc::A_BOLD);
                            //self.window.attron(pc::A_BOLD);
                            display::bold_set(true);
                            COLOR_PAIR::CANDIDATES_Y
                        },
                        'b' => {
                            //self.window.attron(pc::A_BOLD);
                            display::bold_set(true);
                            COLOR_PAIR::CANDIDATES_B
                        },
                        'g' => COLOR_PAIR::GUESS,
                        _ => COLOR_PAIR::DEFAULT, // NOTE: Also case 'n'
                    }
                }
                else {
                    COLOR_PAIR::DEFAULT
                };

                display::color_set(&color_pair);
                self.color_codes[i][j] = color_pair;
                display::addstr(format!("{}", self.display_matrix[i][j] as char).as_str());
                if let Some(_) = SAVED_PUZZLE {
                    display::color_set(&COLOR_PAIR::DEFAULT);
                    display::bold_set(false);
                }

                if j == 8 || j == 17 {
                    display::addstr("|");
                }
            }
            if i == 8 || i == 17 {
                display::mvprintw(
                    (i as u8 + display::ORIGIN.y() + (i as u8 / CONTAINER_SIZE) + 1) as i32,
                    display::ORIGIN.x() as i32,
                    "---------|---------|---------"
                );
            }
        }

        if let Some(_) = SAVED_PUZZLE {
            for i in 0..self.grid2display_map.len()  {
                let coords: Cell = self.grid2display_map[&(i as u8)];
                self.mv(coords);

                let row_index: usize = coords.y().into();
                let column_index: usize = coords.x().into();
                let color_pair: COLOR_PAIR = if self.grid.is_known(i) {
                    COLOR_PAIR::GIVEN
                }
                else {
                    COLOR_PAIR::UNKNOWN
                };
                display::color_set(&color_pair);
                self.color_codes[row_index][column_index] = color_pair;
                display::addstr(format!("{}",self.display_matrix[row_index][column_index] as char).as_str());
                display::color_set(&COLOR_PAIR::DEFAULT);
            }
        }
    }

    /**
     * Moves the cursor to its offset position for the initial printing of the display matrix
     * from Sudoku::printw. This is necessary so that the the display matrix offset can be mapped
     * correctly.
     * 
     *      COORDS -> Pre-offset display line and column numbers.
     */
    fn mv (&mut self, COORDS: Cell) {
        let TOTAL_OFFSETY: i32 =
            (COORDS.y() + display::ORIGIN.y() + (COORDS.y() / CONTAINER_SIZE)) as i32;
        let TOTAL_OFFSETX: i32 =
            (COORDS.x() + display::ORIGIN.x() + (COORDS.x() / CONTAINER_SIZE)) as i32;
        display::mv(TOTAL_OFFSETY, TOTAL_OFFSETX);

        //NOTE: Update cursor_pos after moving
        let (new_cursor_y, new_cursor_x): (i32, i32) = display::get_cur_yx();
        self.cursor_pos.set(new_cursor_y as u8, new_cursor_x as u8);
    }

    /**
     * Creates a mapping between a cell in the display matrix and it's actual location on the
     * screen. A call to this function is made for one cell at a time during the initial printing
     * of the display matrix to the screen.
     * 
     *      DISPLAY_INDECES -> Cell object containing the display line and display column number.
     * 
     * NOTE: This looks like it doesn't work as expected, but the use of the overloaded
     *       Sudoku::move in printw takes care of applying the offset before this function is
     *       called.
     */
    /*fn map_display_matrix_offset (&mut self, DISPLAY_INDECES: Cell) {
        let (y, x): (i32, i32) = display::get_cur_yx();
        self.display_matrix_offset.insert(Cell::new(y as u8, x as u8), DISPLAY_INDECES);
    }*/

    /**
     * Displays the hotkey command available in the bottom left corner depending on whether the
     * in-game menu is enabled.
     * 
     *      USE_IN_GAME_MENU -> Boolean controlling whether or not the in-game menu is enabled.
     *                          This is determined based on whether or not the user runs this
     *                          program with the "--no-in-game-menu" or "-n" command line
     *                          options. This also controls which hotkey is available.
     *      LINE_OFFSET_TWEAK -> Line offset from max line number used to display hotkey command
     *                           in an ideal location.
     */
    fn display_hotkey (&self, USE_IN_GAME_MENU: bool, LINE_OFFSET_TWEAK: u8) {
        let hotkey_string: &str = if !USE_IN_GAME_MENU {
            "s -> save game"
        }
        else {
            "m -> in-game menu"
        };
        //attron(COLOR_PAIR(MENU_SELECTION));
        display::mvprintw(
            display::get_max_y() - LINE_OFFSET_TWEAK as i32,
            display::ORIGIN.x().into(),
            hotkey_string,
        );
        //attroff(COLOR_PAIR(MENU_SELECTION));
    }

    /**
     * Prints the entire sudoku puzzle (the display matrix) to the screen whenever
     * there has been an update by the player (i.e. removal or insertion of a
     * value).
     */
    fn printw (&mut self) {
        for i in 0..display::DISPLAY_MATRIX_ROWS {
            self.mv(Cell::new(i as u8, 0));
            for j in 0..display::DISPLAY_MATRIX_COLUMNS {
                if self.color_codes[i][j] == COLOR_PAIR::CANDIDATES_Y ||
                   self.color_codes[i][j] == COLOR_PAIR::CANDIDATES_B {
                        display::bold_set(true);
                    }
                display::color_set(&self.color_codes[i][j]);
                display::addstr(format!("{}", self.display_matrix[i][j]).as_str());
                display::color_set(&COLOR_PAIR::DEFAULT);
                display::bold_set(false);

                if j == 8 || j == 17 {
                    display::addstr("|");
                }
            }
            if i == 8 || i == 17 {
                display::mvprintw(
                    i as i32 + display::ORIGIN.y() as i32 + (i as i32 / CONTAINER_SIZE as i32) + 1,
                    display::ORIGIN.x().into(),
                    "---------|---------|---------"
                );
            }
        }
    }

    /**
     * Resets the cursor to its last officially recorded position. This is mainly used after
     * needing to temporarily move to another cell to read or remove a value and gives the
     * appearance that the cursor never moved at all.
     */
    fn reset_cursor (&self) {
        display::mv(self.cursor_pos.y().into(), self.cursor_pos.x().into());
    }

    /**
     * Prompts the user for the name to save the game under before saving the game. Displays a
     * status message after having saved. This performs a similar function to
     * `InGameMenu::saved_game_prompt` for when the in-game menu functionality is disabled.
     * 
     *      DELAY -> Amount of time to display the status message before clearing it and
     *               resuming play.
     */
    fn save_game_prompt (&self, DELAY: u8) {
        let DISPLAY_LINE: i32 =
            display::ORIGIN.y() as i32 + display::DISPLAY_MATRIX_ROWS as i32 + 3;
        display::mv(DISPLAY_LINE, 1);
        display::clrtoeol();
        display::addstr("Enter save file name: ");

        /* NOTE: Copy the display matrix int oa pointer in order to pass along to
         *       InGameMenu::save_game
         */
        let name = self.save_game();
        display::mv(DISPLAY_LINE, 1);
        display::clrtoeol();
        //NOTE: Turn off cursor while displaying
        display::curs_set(CURSOR_VISIBILITY::NONE);
        display::addstr(format!("{} saved!", name).as_str());
        display::refresh();

        //NOTE: Clear output after a delay
        display::napms(DELAY.into());
        display::mv(DISPLAY_LINE, 0);
        display::clrtoeol();
        display::curs_set(CURSOR_VISIBILITY::BLOCK);
    }

    /**
     * 
     */
    fn save_game (&self) -> String {
        let NAME_SIZE: usize = 16;              //NOTE: NAME_SIZE limited by window width
        let mut name: String = String::new();   //      requirements of no in-game menu mode
        display::nodelay(false);
        pc::echo();
        display::getnstr(&mut name, NAME_SIZE - 1);
        pc::noecho();
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

struct Grid {
    grid_map: HashMap<u8, Cell>,
    known_positions: [bool; GRID_SIZE as usize],
    rows: [Row; CONTAINER_SIZE as usize],
    columns: [Column; CONTAINER_SIZE as usize],
    boxes: [Box; CONTAINER_SIZE as usize],
}

impl Grid {
    /**
     * Begins initialization of internal Container data structures based on the difficulty level
     * chosen by the user.
     * 
     *      diff -> Enum value of difficulty level chosen by the user from the main menu.
     */
    pub fn new (diff: DifficultyMenuOption) -> Self {
        let mut grid: Grid = Self::init(diff);
        //TODO: Test from here
        grid.set_starting_positions(diff);
        grid
    }

    /**
     * Creates an empty Sudoku grid. This helps facilitate some of the later setup functions in
     * `Grid::new`.
     * 
     *      unused DifficultyMenuOption -> Enum value of difficulty level chosen by the user from the main menu.
     */
    //TODO: Get rid of the DifficultyMenuOption parameter if not needed
    fn init (_: DifficultyMenuOption) -> Self {
        let grid_map: HashMap<u8, Cell> = Self::create_map();
        let known_positions: [bool; GRID_SIZE as usize] = Self::init_known_positions();
        //let rows: [Row; CONTAINER_SIZE as usize] = [Row::new(CONTAINER::ROW, [0; CONTAINER_SIZE as usize]); NUM_CONTAINERS as usize];
        let space: u8 = ' ' as u8;
        let rows: [Row; NUM_CONTAINERS as usize] =
            array::from_fn(|_| Row::new(CONTAINER::ROW, [space; CONTAINER_SIZE as usize]));
        let columns: [Column; NUM_CONTAINERS as usize] =
            array::from_fn(|_| Column::new(CONTAINER::COLUMN, [space; CONTAINER_SIZE as usize]));
        let boxes: [Box; NUM_CONTAINERS as usize] =
            array::from_fn(|_| Box::new(CONTAINER::BOX, [space; CONTAINER_SIZE as usize]));
        
        /*display::tui_end();
        println!("grid_map:\n{:?}", grid_map);
        println!("known_positions:\n{:?}", known_positions);
        println!("rows, columns, and boxes:");
        for (r, c, b) in itertools::izip!(rows, columns, boxes) {
            println!("r: {:?}", r.arr);
            println!("c: {:?}", c.arr);
            println!("b: {:?}\n", b.arr);
        }
        std::process::exit(1);*/
        
        Self {
            grid_map: grid_map,
            known_positions: known_positions,
            rows: rows,
            columns: columns,
            boxes: boxes,
        }
    }
    
    //TODO: use from_save for the name in place of the third constructor

    /**
     * Creates a mapping of all 81 grid positions to a 9x9 matrix.
     */
    fn create_map () -> HashMap<u8, Cell> {
        let mut grid_map: HashMap<u8, Cell> = HashMap::new();
        for i in 0..NUM_CONTAINERS {
            for j in 0..NUM_CONTAINERS {
                grid_map.insert(i * CONTAINER_SIZE + j, Cell::new(i, j));
            }
        }
        grid_map
    }

    /**
     * Initializes the Grid's internal boolean array known_positions to all false values.
     */
    fn init_known_positions () -> [bool; GRID_SIZE as usize] {
        [false; GRID_SIZE as usize]
    }

    /**
     * Set starting grid positions for a new game from a randomly generated solved puzzle.
     * 
     *      NUM_POSITIONS -> The number of given positions to initialize the puzzle. This value
     *                       will be based on difficulty level chosen by the user from the main
     *                       menu.
     */
    fn set_starting_positions (&mut self, diff: DifficultyMenuOption) {
        let seed: i32 = unsafe {
            time(0x0)
        };
        //TODO
        let solved_puzzle = self.generate_solved_puzzle(&seed);
        //Self::generate_solved_puzzle(unsafe { time(0x0) }); //This will also work

        let mut generator = thread_rng();
        let mut positions: [u8; GRID_SIZE as usize] = array::from_fn(|i| i as u8);
        positions.shuffle(&mut generator);

        let NUM_POSITIONS: usize = match diff {
            DifficultyMenuOption::EASY => 60,
            DifficultyMenuOption::MEDIUM => 45,
            DifficultyMenuOption::HARD => 30,
            DifficultyMenuOption::EXPERT => 17,
        };
        for i in 0..NUM_POSITIONS {
            let POS: u8 = positions[i];
            self.set_value(POS, solved_puzzle[POS as usize]);
            self.known_positions[POS as usize] = true;
        }

        //TODO: Either uncomment this or get rid of it depending on if it's needed
        /*for i in NUM_POSITIONS..GRID_SIZE as usize {
            self.known_positions[positions[i] as usize] = false;
        }*/
    }

    /**
     * Generates and returns a solved sudoku puzzle. This puzzle is later used to created a
     * solvable puzzle. The puzzle is generated randomly using a Mersenne-Twister engine.
     * 
     *      unused `&i32` -> Seed for the pseudo-random number sequence
     */
    fn generate_solved_puzzle (&mut self, _: &i32) -> [u8; GRID_SIZE as usize] {
        //TODO: Is there an easy way to "flatten" a matrix into an array (2D -> 1D)?
        let mut soln: [u8; GRID_SIZE as usize] = [0; GRID_SIZE as usize];

        //NOTE: Initialize the solution matrix with '?' placeholders
        let mut soln_matrix: [[u8; NUM_CONTAINERS as usize]; NUM_CONTAINERS as usize] =
            [['?' as u8; NUM_CONTAINERS as usize]; NUM_CONTAINERS as usize];
        let mut generator = thread_rng();   /* NOTE: The original C++ code used a
                                                        *       Mersenne-Twister engine. This
                                                        *       just uses the default RNG
                                                        *       created by rand.
                                                        */
        //NOTE: Random numbers with values of 1-81 will be uniformly generated.
        let _ = Uniform::new_inclusive(1, CONTAINER_SIZE + 1);
        let mut values: [u8; CONTAINER_SIZE as usize] = array::from_fn(|i| i as u8 + 1);

        /* NOTE: Fill in boxes along the diagonal first.On an empty puzzle, boxes 1, 5, and 9 are
         *       independent of each other, and can be randomly filled in a more trivial manner.
         */
        let mut i: usize = 0;
        while i < NUM_CONTAINERS as usize {
            values.shuffle(&mut generator);
            let mut count: usize = 0;
            for j in i..i+3 {
                for k in i..i+3 {
                    soln_matrix[j][k] = values[count];
                    count += 1;
                }
            }

            i += 3;
        }

        /* NOTE: Create row, column, and box objects from partial solution matrix. These arrays
         *       will be used to finish solving the sudoku puzzle recursively.
         */
        let mut soln_rows: [Row; NUM_CONTAINERS as usize] =
            array::from_fn(|i| Row::new(CONTAINER::ROW, soln_matrix[i]));
        let mut soln_columns: [Column; NUM_CONTAINERS as usize] = array::from_fn(
            |i| {
                /*let mut temp_col: [u8; NUM_CONTAINERS as usize] = [0; NUM_CONTAINERS as usize];
                for j in 0..NUM_CONTAINERS {
                    temp_col[j] = soln_matrix[j][i];
                }
                Column::new(CONTAINER::COLUMN, temp_col)*/
                Column::new(CONTAINER::COLUMN, array::from_fn(|j| soln_matrix[j][i]))
            }
        );
        let mut soln_boxes: [Box; NUM_CONTAINERS as usize] = array::from_fn(
            |i| {
                let (j, k): (usize, usize) = if i == 0 { (1, 1) }
                    else if i == 1 { (1, 4) }
                    else if i == 2 { (1, 7) }
                    else if i == 3 { (4, 1) }
                    else if i == 4 { (4, 4) }
                    else if i == 5 { (4, 7) }
                    else if i == 6 { (7, 1) }
                    else if i == 7 { (7, 4) }
                    else /*if i == 8*/ { (7, 7) };
                
                let arr: [u8; NUM_CONTAINERS as usize] = [
                    soln_matrix[j-1][k-1],
                    soln_matrix[j-1][k],
                    soln_matrix[j-1][k+1],
                    soln_matrix[j][k-1],
                    soln_matrix[j][k],
                    soln_matrix[j][k+1],
                    soln_matrix[j+1][k-1],
                    soln_matrix[j+1][k],
                    soln_matrix[j+1][k+1],
                ];
                Box::new(CONTAINER::BOX, arr)
            }
        );

        display::tui_end();
        let _: bool = self.solve(1, 1, &mut soln_rows, &mut soln_columns, &mut soln_boxes);

        //println!("soln:\n{:?}", soln_matrix);
        println!("soln:");
        for row in soln_matrix {
            println!("\t{:?}", row);
        }
        println!("");
        for (r, c, b) in itertools::izip!(soln_rows, soln_columns, soln_boxes) {
            println!("r: {:?}", r.arr);
            println!("c: {:?}", c.arr);
            println!("b: {:?}\n", b.arr);
        }
        std::process::exit(1);
        for i in 0..NUM_CONTAINERS as usize {
            for j in 0..NUM_CONTAINERS as usize {
                if soln_matrix[i][j] == '?' as u8 {
                    soln_matrix[i][i] = soln_rows[i].at(j);
                }
            }
        }

        for i in 0..NUM_CONTAINERS as usize {
            for j in 0..NUM_CONTAINERS as usize {
                soln[i * CONTAINER_SIZE as usize + j] = soln_matrix[i][j];
            }
        }
        soln
    }

    /* Bowman's Bingo Algorithm:
     * args <- box # [1-3, 5-7], value # [1-9], row array, column array, box array
     * queue <- available positions on board [0-80]
     * do next_pos <- queue.pop() while recursive call <- false
     *    add value to next_pos in appropriate row, column, and box if possible     STEP 1
     *    return true if box=7, value=9, queue not empty                            STEP 2
     *    return false otherwise (queue empty)                                      STEP 3
     *    next_box <- 5 if box=3                                                    STEP 4
     *             <- 1 if box=7
     *             <- box+1 otherwise
     *    next_value <- value+1 if box=7                                            STEP 5
     *               <- same otherwise
     *    remove value from row, column, and box if recursive call <- false         STEP 6
     * end do-while
     */
    /**
     * Recursively generates a solved sudoku puzzle using the Bowman's Bingo technique. The
     * algorithm recursively focuses on placing the same value into each box before working to
     * place the next value (i.e. each box is iterated through placing a 1 in a valid position,
     * then the same is done for 2, followed by 3, etc.). Even though it is technically possible
     * for false to be returned up the recursive chain to generate_solved_puzzle, indicating that
     * a solved puzzle couldn't be generated, this logically should never happen (i.e. this
     * function always returns a solved puzzle). The solved puzzle is "returned" in the sense that
     * the Row, Column, and Box parameters will be filled after this function successfully
     * returns. The algorithm for this is described below the parameters list, but like all good
     * algorithms is coded in practice slightly out of order.
     * 
     *      BOX -> Box number of the current recursive iteration.
     *      VALUE -> The numerical value 1-9 being placed in the current Box.
     *      rows -> Array of Row objects each representing a row of the solved puzzle. All
     *              recursive iterations have access to the same array.
     *      columns -> Array of Column objects each representing a column of the solved puzzle.
     *                 All recursive iterations have access to the same array.
     *      boxes -> Array of Box objects each representing a box of the solved puzzle. All
     *               recursive iterations have access to the same array.
     */
    fn solve (&mut self, BOX: u8, VALUE: u8, rows: &mut [Row; NUM_CONTAINERS as usize],
              columns: &mut [Column; NUM_CONTAINERS as usize],
              boxes: &mut [Box; NUM_CONTAINERS as usize]) -> bool {
        /* NOTE: Figure out positions in box based on box number.
         *       Start with upper right.
         *
         * 0   | 3   | 6
         *     |     | 
         *     |     | 
         * ----|-----|----
         * 27  | 30  | 33
         *     |     | 
         *     |     | 
         * ----|-----|----
         * 54  | 57  | 60
         *     |     |
         *     |     |
         */
        let mut available_pos: Queue<u8> = Queue::new();
        let mut positions: [u8; CONTAINER_SIZE as usize] = [0; CONTAINER_SIZE as usize];

        let mut i = BOX;
        while i >= 3 {
            positions[0] += 27;
            i -= 3;
        }
        positions[0] += 3 * (BOX % 3);

        // Figure out the remaining 8 positions in box
        for i in 1..CONTAINER_SIZE {
            positions[i as usize] = positions[0] + CONTAINER_SIZE * (i / 3) + i % 3;
        }

        /* NOTE: Figure out positions VALUE can and can't be placed.
         *       Map row and column (box shouldn't be needed).
         */
        for i in 0..CONTAINER_SIZE as usize {
            let ROW_NUMBER: usize = Self::map_row(positions[i]);
            let COLUMN_NUMBER: usize = Self::map_column(positions[i]);
            println!("y: {}, x: {}, val: {}, y_exists: {}, x_exists: {}, is_known: {}",
                ROW_NUMBER, COLUMN_NUMBER, VALUE, rows[ROW_NUMBER].value_exists(VALUE),
                columns[COLUMN_NUMBER].value_exists(VALUE), self.is_known(positions[i] as usize)
            );
            if !rows[ROW_NUMBER].value_exists(VALUE) &&
               !columns[COLUMN_NUMBER].value_exists(VALUE) &&
               self.is_known(positions[i] as usize) {
                println!("Trying to add to the queue...");
                let _ =available_pos.add(positions[i]);
            }
        }

        println!("testing before recursion...");
        /* NOTE: set_value cannot be used here because the rows, columns, and boxes being used
         *       are not the Grid's internal Containers. They belong to the solution matrix and
         *       are completely separate. Interesting things happened when I tested that out
         *       before I realized why it wouldn't work.
         */
        let mut stop: bool = false;
        let mut soln: bool = true;
        //while true {
        while !stop {
            //TODO: This isn't being updated like it should
            if available_pos.size() == 0 {
                return false
            }
            println!("After available_pos.size() check...");

            let next_available_pos = available_pos.peek()
                .expect("Error retrieving next position while solving...");
            //let ROW_NUMBER: usize = Self::map_row(next_available_pos);
            //let COLUMN_NUMBER: usize = Self::map_column(next_available_pos);
            //let BOX_NUMBER: usize = BOX as usize;
            let (ROW_NUMBER, COLUMN_NUMBER, BOX_NUMBER): (usize, usize, usize) =
                Self::map_containers(next_available_pos);
            //let ROW_INDEX: usize = Self::get_row_index(next_available_pos);
            //let COLUMN_INDEX: usize = Self::get_column_index(next_available_pos);
            //let BOX_INDEX: usize = Self::get_box_index(next_available_pos);
            let (ROW_INDEX, COLUMN_INDEX, BOX_INDEX): (usize, usize, usize) =
                Self::get_container_indeces(next_available_pos);
            
            rows[ROW_NUMBER].set_value(ROW_INDEX, VALUE);                  //NOTE: STEP 1
            columns[COLUMN_NUMBER].set_value(COLUMN_INDEX, VALUE);
            boxes[BOX_NUMBER].set_value(BOX_INDEX, VALUE);
            self.known_positions[next_available_pos as usize] = true;

            if BOX == 7 && VALUE == 9 {                                    //NOTE: STEP 2
                return true;
            }

            let next_box: u8 = if      BOX == 3 { 5 }                      //NOTE: STEP 4
                               else if BOX == 7 { 1 }
                               else             { BOX + 1 };
            let next_value: u8 = if BOX == 7 { VALUE + 1 }                 //NOTE: STEP 5
                                 else        { VALUE };

            soln = self.solve(next_box, next_value, rows, columns, boxes);
            //if soln { break; }  //TODO: Find a better way to do this
            if soln { stop = true; }
            //if soln { return soln; }
            else {
                rows[ROW_NUMBER].set_value(ROW_INDEX, '?' as u8);   //NOTE: STEP 6
                columns[COLUMN_NUMBER].set_value(COLUMN_INDEX, '?' as u8);
                boxes[BOX_NUMBER].set_value(BOX_INDEX, '?' as u8);
                self.known_positions[next_available_pos as usize] = false;
                let _ = available_pos.remove();
            }
            println!("stop: {}", stop);
        }
        
        for row in rows {
            println!("row: {:?}", row.arr);
        }
        println!("");
        soln
    }

    /**
     * Returns the row number based on the grid position.
     * 
     *      POS -> Grid position 0-80 used to map the appropriate row number.
     */
    fn map_row (POS: u8) -> usize {
        (POS / NUM_CONTAINERS) as usize
    }

    /**
     * Returns the column number based on the grid position.
     * 
     *      POS -> Grid position 0-80 used to map the appropriate column number.
     */
    fn map_column (POS: u8) -> usize {
        (POS % NUM_CONTAINERS) as usize
    }

    /**
     * Returns the box number based on the grid position. This function is reliant on the row and
     * column having been mapped prior to being called. This simplifies mapping the box number as
     * the row and column numbers aren't calculated a second time, and is logically sound since
     * there is never a situation where boxes are mapped independently of rows and columns.
     * 
     *      ROW -> Previously mapped row number 0-8 used to map the appropriate box.
     *      COLUMN -> Previously mapped column number 0-8 used to map the appropriate box.
     */
    fn map_box (ROW: usize, COLUMN: usize) -> usize {
        /* NOTE: Side-by-side numbering of array-like positions and matrix-like positions
         * 
         *           NUMBERED (0-80)                NUMBERED (ROW x COLUMN)
         *    0  1  2 |  3  4  5 |  6  7  8      00 01 02 | 03 04 05 | 06 07 08
         *    9 10 11 | 12 13 14 | 15 16 17      10 11 12 | 13 14 15 | 16 17 18
         *   18 19 20 | 21 22 23 | 24 25 26      20 21 22 | 23 24 25 | 26 27 28
         *   ---------|----------|---------      ---------|----------|---------
         *   27 28 29 | 30 31 32 | 33 34 35      30 31 32 | 33 34 35 | 36 37 38
         *   36 37 38 | 39 40 41 | 42 43 44      40 41 42 | 43 44 45 | 46 47 48
         *   45 46 47 | 48 49 50 | 51 52 53      50 51 52 | 53 54 55 | 56 57 58
         *   ---------|----------|---------      ---------|----------|---------
         *   54 55 56 | 57 58 59 | 60 61 62      60 61 62 | 63 64 65 | 66 67 68
         *   63 64 65 | 66 67 68 | 69 70 71      70 71 72 | 73 74 75 | 76 77 78
         *   72 73 74 | 75 76 77 | 78 79 80      80 81 82 | 83 84 85 | 86 87 88
         */
        if ROW < 3 {
            if      COLUMN < 3 { 0 }
            else if COLUMN < 6 { 1 }
            else               { 2 }
        }
        else if ROW < 6 {
            if      COLUMN < 3 { 3 }
            else if COLUMN < 6 { 4 }
            else               { 5 }
        }
        else {
            if      COLUMN < 3 { 6 }
            else if COLUMN < 6 { 7 }
            else               { 8 }
        }
    }

    /**
     * Returns the row, column, and box numbers based on the grid position.
     * 
     *      POS -> Grid position 0-80 used to map the appropriate row, column, and box numbers.
     */
    fn map_containers (POS: u8) -> (usize, usize, usize) {
        let row_number: usize = Self::map_row(POS);
        let column_number: usize = Self::map_column(POS);
        (row_number, column_number, Self::map_box(row_number, column_number))
    }

    /**
     * Returns whether a value at a given Grid index is known. Known values correspond to given
     * values from when the puzzle was first generated.
     * 
     *      INDEX -> Index of the Grid to check.
     */
    fn is_known (&self, INDEX: usize) -> bool {
        self.known_positions[INDEX]
    }

    /**
     * Returns the index of a Row object based on the grid position.
     * 
     *      POS -> Grid position 0-80 used to map the appropriate Row index.
     */
    fn get_row_index (POS: u8) -> usize {
        /* NOTE: Side-by-side numbering of array-like positions and matrix-like positions
         * 
         *           NUMBERED (0-80)                NUMBERED (ROW x COLUMN)
         *    0  1  2 |  3  4  5 |  6  7  8      00 01 02 | 03 04 05 | 06 07 08
         *    9 10 11 | 12 13 14 | 15 16 17      10 11 12 | 13 14 15 | 16 17 18
         *   18 19 20 | 21 22 23 | 24 25 26      20 21 22 | 23 24 25 | 26 27 28
         *   ---------|----------|---------      ---------|----------|---------
         *   27 28 29 | 30 31 32 | 33 34 35      30 31 32 | 33 34 35 | 36 37 38
         *   36 37 38 | 39 40 41 | 42 43 44      40 41 42 | 43 44 45 | 46 47 48
         *   45 46 47 | 48 49 50 | 51 52 53      50 51 52 | 53 54 55 | 56 57 58
         *   ---------|----------|---------      ---------|----------|---------
         *   54 55 56 | 57 58 59 | 60 61 62      60 61 62 | 63 64 65 | 66 67 68
         *   63 64 65 | 66 67 68 | 69 70 71      70 71 72 | 73 74 75 | 76 77 78
         *   72 73 74 | 75 76 77 | 78 79 80      80 81 82 | 83 84 85 | 86 87 88
         */
        (POS % CONTAINER_SIZE) as usize
    }

    /**
     * Returns the index of a Column object based on the grid position.
     * 
     *      POS -> Grid position 0-80 used to map the appropriate Column index.
     */
    fn get_column_index (POS: u8) -> usize {
        (POS / CONTAINER_SIZE) as usize
    }

    /**
     * Returns the index of a Box object based on the grid position. This function can't benefit
     * in a similar manner as map_box since there are times when box indeces are needed
     * independent of rows and columns.
     * 
     *      POS -> Grid position 0-80 used to map the appropriate Box index.
     */
    fn get_box_index (POS: u8) -> usize {
        let ROW: usize = Self::get_row_index(POS);
        let COLUMN: usize = Self::get_column_index(POS);
        3 * (COLUMN % 3) + ROW % 3
    }

    /**
     * Returns the indeces of a Row, Column, and Box objects based on the grid position.
     * 
     *      POS -> Grid position 0-80 used to map the appropriate Row, Column, and Box indeces.
     */
    fn get_container_indeces (POS: u8) -> (usize, usize, usize) {
        (Self::get_row_index(POS), Self::get_column_index(POS), Self::get_box_index(POS))
    }

    /**
     * Returns an address to the Row Container from this Grid's internal Row array. This
     * allows the Row object to be mutable from the Grid when an input is passed from the Sudoku
     * object.
     * 
     *      INDEX -> The index to return from the Grid's internal Row array.
     */
    fn get_row (&self, INDEX: usize) -> &Row {
        &self.rows[INDEX]
    }

    /**
     * Returns an address to the Column Container from this Grid's internal Column array. This
     * allows the Column object to be mutable from the Grid when an input is passed from the
     * Sudoku object.
     * 
     *      INDEX -> The index to return from the Grid's internal Column array.
     */
    fn _get_column (&self, INDEX: usize) -> &Column {
        &self.columns[INDEX]
    }

    /**
     * Returns an address to the Box Container from this Grid's internal Box array. This allows
     * the Box object to be mutable from the Grid when an input is passed from the Sudoku
     * object.
     * 
     *      INDEX -> The index to return from the Grid's internal Box array.
     */
    fn _get_box (&self, INDEX: usize) -> &Box {
        &self.boxes[INDEX]
    }

    /**
     * Returns a mutable address to the Row Container from this Grid's internal Row array. This
     * allows the Row object to be mutable from the Grid when an input is passed from the Sudoku
     * object.
     * 
     *      INDEX -> The index to return from the Grid's internal Row array.
     */
    fn get_row_mut (&mut self, INDEX: usize) -> &mut Row {
        &mut self.rows[INDEX]
    }

    /**
     * Returns a mutable address to the Column Container from this Grid's internal Column array.
     * This allows the Column object to be mutable from the Grid when an input is passed from the
     * Sudoku object.
     * 
     *      INDEX -> The index to return from the Grid's internal Column array.
     */
    fn get_column_mut (&mut self, INDEX: usize) -> &mut Column {
        &mut self.columns[INDEX]
    }

    /**
     * Returns a mutable address to the Box Container from this Grid's internal Box array. This
     * allows the Box object to be mutable from the Grid when an input is passed from the Sudoku
     * object.
     * 
     *      INDEX -> The index to return from the Grid's internal Box array.
     */
    fn get_box_mut (&mut self, INDEX: usize) -> &mut Box {
        &mut self.boxes[INDEX]
    }

    /**
     * Places a value into the correct position (row, column, and box) in the grid.
     * 
     *      POS -> The grid position 0-80 where the value will be placed.
     *      VALUE -> The value to be placed in the grid.
     */
    fn set_value (&mut self, POS: u8, VALUE: u8) {
        let ROW_NUMBER: usize = Self::map_row(POS);
        let COLUMN_NUMBER: usize = Self::map_column(POS);
        let BOX_NUMBER: usize = Self::map_box(ROW_NUMBER, COLUMN_NUMBER);
        let (INDEX_ROW, INDEX_COLUMN, INDEX_BOX): (usize, usize, usize) = Self::get_container_indeces(POS);

        /*
         * NOTE: Check the row, column, and box for the value and add value from solved puzzle
         *       to empty puzzle.
         */
        let row: &mut Row = self.get_row_mut(ROW_NUMBER);
        row.set_value(INDEX_ROW, VALUE);

        let column: &mut Column = self.get_column_mut(COLUMN_NUMBER);
        column.set_value(INDEX_COLUMN, VALUE);

        let r#box: &mut Box = self.get_box_mut(BOX_NUMBER);
        r#box.set_value(INDEX_BOX, VALUE);
    }

    /**
     * Returns the value at a given index from the Grid. This can be done using Rows, Columns, or
     * Boxes. Only one type of container needs to return the value, although all three have been
     * tested for correctness.
     * 
     *      INDEX -> Index of the grid to return the value from.
     */
    fn at (&self, INDEX: u8) -> u8 {
        self.get_row(Self::map_row(INDEX)).at(Self::get_row_index(INDEX))
        //self.get_column(self.map_column(INDEX)).at(self.get_column_index(INDEX))
        //self.get_box(self.map_box_index(INDEX)).at(self.get_box_index(INDEX))
    }
}

enum CONTAINER {
    ROW,
    COLUMN,
    BOX,
}

//use Container as House;
use Container as Row;
use Container as Column;
use Container as Box;
struct Container {
    container_type: CONTAINER,
    arr: [u8; CONTAINER_SIZE as usize],
}
//

impl Container {
    /**
     * Initializes internal array to the same values as it's array parameter.
     * 
     *      container_type -> 
     *      arr -> 
     */
    pub fn new (container_type: CONTAINER, arr: [u8; CONTAINER_SIZE as usize]) -> Self {
        Self {
            container_type: container_type,
            arr: arr,
        }
    }

    /**
     * Retrieves a value from the internal array.
     * 
     *      INDEX -> Index of the Container's internal array.
     */
    fn at (&self, INDEX: usize) -> u8 {
        self.arr[INDEX]
    }

    /**
     * Checks whether a value 1-9 already exists in the internal array.
     * 
     *      VALUE -> Integer value that is searched for in the container's internal array.
     */
    fn value_exists (&self, VALUE: u8) -> bool {
        /*let CONVERTED: u8 = match VALUE as char {
            '?' => VALUE,
            _ => VALUE + '0' as u8,
        };*/
        if VALUE == '?' as u8 {
            false
        }
        else {
            let mut exists: bool = false;
            for i in 0..CONTAINER_SIZE as usize {
                if self.at(i) == VALUE {
                    exists = true;
                }
            }
            exists
        }
    }

    /**
     * Sets an element of the internal array to a specfic value 1-9.
     * 
     *      INDEX -> Integer index of the internal array
     *      VALUE -> Integer value to be placed into the internal array
     */
    fn set_value (&mut self, INDEX: usize, VALUE: u8) {
        self.arr[INDEX] = VALUE;
    }
}
