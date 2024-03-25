use crate::terminal::{
    display::{
        DISPLAY_MATRIX_ROWS,
        DISPLAY_MATRIX_COLUMNS,
        ORIGIN,
    },
    Cell,
};
use std::collections::HashMap;
use ncurses::{
    has_colors,
    init_pair,
    COLOR_WHITE,
    COLOR_BLACK,
    COLOR_RED,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_GREEN,
};

const GRID_SIZE: u8 = 81;

/// Display matrix color codes
const UNKNOWN: i16 = 1;
const GIVEN: i16 = 2;
const CANDIDATES_Y: i16 = 3;
const CANDIDATES_B: i16 = 4;
const GUESS: i16 = 5;

enum neighbor_cells {
    TL,
    T,
    TR,
    L,
    R,
    BL,
    B,
    BR,
}

impl neighbor_cells {
    const fn NUM_BORDER_POSITIONS () -> u8 {
        8
    }
}

/// Holds data for a saved game that is selected to be resumed
#[derive(Clone)]
pub struct SavedPuzzle {
    /// A 9x9 matrix containing the values in the puzzle cells
    puzzle: [[u8; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS],
    /// A 9x9 matrix containing the current color codes of the puzzle cells
    color_codes: [[char; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS],
    /// The name of the file the puzzle is saved under
    filename: String,
}

impl SavedPuzzle {
    /// Returns an "empty" SavedPuzzle (zeroed/spaced arrays and empty filename).
    pub fn new () -> Self {
        Self {
            puzzle: [[0; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS],
            color_codes: [[' '; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS],
            filename: String::new(),
        }
    }

    /// Stores the values of the saved puzzle into an array.
    pub fn set_puzzle (&mut self, puzzle: [[u8; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS]) {
        self.puzzle = puzzle;
    }

    /// Stores the color codes of the saved puzzle into an array.
    pub fn set_color_codes (&mut self, color_codes: [[char; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS]) {
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
    display_matrix: [[u8; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS],
    color_codes: [[char; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS],
    grid: Grid,
    grid2display_map: HashMap<u8, Cell>,
    display2grip_map: HashMap<Cell, u8>,
    ORIGIN: Cell,
    cursor_pos: Cell,
    display_matrix_offset: HashMap<Cell, Cell>,
}

impl Sudoku {
    /**
     * Returns a Sudoku instance, a live interactive game of sudoku. Also coordinates 
     * setup of color mappings and display matrix initialization.
     */
    pub fn new (/* saved_puzzle: &SavedPuzzle */) -> Self {
        let (grid2display, display2grid) = Self::create_maps();
        Self::set_color_pairs();
        Self {
            display_matrix: [[0; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS],
            color_codes: [[' '; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS],
            grid: Grid,
            grid2display_map: grid2display,
            display2grip_map: display2grid,
            ORIGIN: ORIGIN,
            cursor_pos: ORIGIN,
            display_matrix_offset: HashMap::new(),
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
            if col / DISPLAY_MATRIX_COLUMNS as u8 != 0 {
                col %= DISPLAY_MATRIX_COLUMNS as u8;
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
     * Establishes the color pairs used while printing anywhere in the display matrix.
     * The color pair MENU_SELECTION is defined inside MainMenu.cpp, and its value is
     * carried over throughout the rest of the program. In the case coloring is not
     * available (in the event this somehow finds its way onto some old machine), a
     * monochrome mode is also provided where everything but guesses are the same color.
     */
    fn set_color_pairs () {
        if has_colors() {
            init_pair(UNKNOWN, COLOR_WHITE, COLOR_BLACK);
            init_pair(GIVEN, COLOR_RED, COLOR_BLACK);
            init_pair(CANDIDATES_Y, COLOR_YELLOW, COLOR_BLACK);
            init_pair(CANDIDATES_B, COLOR_BLUE, COLOR_BLACK);
            init_pair(GUESS, COLOR_GREEN, COLOR_BLACK);
        }
        else {  //Monochrome mode
            init_pair(UNKNOWN, COLOR_WHITE, COLOR_BLACK);
            init_pair(GIVEN, COLOR_BLACK, COLOR_WHITE); //Reversed to better stand out
            init_pair(CANDIDATES_Y, COLOR_WHITE, COLOR_BLACK);
            init_pair(CANDIDATES_B, COLOR_WHITE, COLOR_BLACK);
            init_pair(GUESS, COLOR_WHITE, COLOR_BLACK);
        }
    }

    /**
     * Initialiizes the display matrix with either a newly generated puzzle or a saved
     * game.
     */
    fn init_display_matrix (saved_puzzle: Option<&SavedPuzzle>) {
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
    }
}

struct Grid;
