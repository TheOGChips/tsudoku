use crate::{
    terminal::{
        display::{
            DISPLAY_MATRIX_ROWS,
            DISPLAY_MATRIX_COLUMNS,
            ORIGIN,
        },
        Cell,
    },
    menu::{
        DifficultyMenu,
        Menu,
        DifficultyMenuOption,
        MenuOption,
    },
};
use std::{
    collections::HashMap,
    array::from_fn,
};
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
use rand::{
    thread_rng,
    distributions::{
        Distribution,
        Uniform,
    },
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
    pub fn new (saved_puzzle: &SavedPuzzle) -> Self {
        let (grid2display, display2grid) = Self::create_maps();
        Self::set_color_pairs();
        let (display_matrix, grid) = Self::init_display_matrix(Some(saved_puzzle), &grid2display);
        //TODO: Return display_matrix, color_codes?, and grid from init_display_matrix
        Self {
            display_matrix: display_matrix,
            color_codes: todo!()/*[[' '; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS]*/,
            grid: grid,
            grid2display_map: grid2display,
            display2grip_map: display2grid,
            ORIGIN: ORIGIN,
            cursor_pos: ORIGIN,
            display_matrix_offset: todo!()/*HashMap::new()*/,
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
     * 
     *      saved_puzzle -> Pointer to a SavedPuzzle object that represents a previously
     *                      saved game. If the user has selected to start a new game,
     *                      this will be a nullptr. If the user has selected to resume a
     *                      saved game, this object will be read in beforehand.
     */
    fn init_display_matrix (saved_puzzle: Option<&SavedPuzzle>, grid2display: &HashMap<u8, Cell>)
        -> ([[u8; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS], Grid) {
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
            Some(puzzle) => todo!(),
            None => {
                let mut mat: [[u8; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS] =
                    [[' ' as u8; DISPLAY_MATRIX_COLUMNS]; DISPLAY_MATRIX_ROWS];

                let mut diff_menu: DifficultyMenu = DifficultyMenu::new();
                if let MenuOption::DIFFICULTY_MENU(diff) = diff_menu.menu() {
                    diff_menu.set_difficulty_level(diff);
                }
                
                let grid: Grid = Grid::new(diff_menu.get_difficulty_level());
                for (i, cell) in grid2display {
                    mat[cell.y() as usize ][cell.x() as usize] = grid.at(*i);
                }

                (mat, grid)
            },
        }
        //TODO: Return the Grid?
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
     * Creates an empty Sudoku grid. This helps facilitate some of the later setup functions in
     * `Grid::new`.
     * 
     *      diff -> Enum value of difficulty level chosen by the user from the main menu.
     */
    fn init (diff: DifficultyMenuOption) -> Self {
        let grid_map: HashMap<u8, Cell> = Self::create_map();
        let known_positions: [bool; GRID_SIZE as usize] = Self::init_known_positions();
        //let rows: [Row; CONTAINER_SIZE as usize] = [Row::new(CONTAINER::ROW, [0; CONTAINER_SIZE as usize]); NUM_CONTAINERS as usize];
        let rows: [Row; NUM_CONTAINERS as usize] = from_fn(|_| Row::new(CONTAINER::ROW, [0; CONTAINER_SIZE as usize]));
        let columns: [Column; NUM_CONTAINERS as usize] = from_fn(|_| Column::new(CONTAINER::COLUMN, [0; CONTAINER_SIZE as usize]));
        let boxes: [Box; NUM_CONTAINERS as usize] = from_fn(|_| Box::new(CONTAINER::BOX, [0; CONTAINER_SIZE as usize]));
        
        Self {
            grid_map: grid_map,
            known_positions: known_positions,
            rows: rows,
            columns: columns,
            boxes: boxes,
        }
    }
    
    /**
     * Begins initialization of internal Container data structures based on the difficulty level
     * chosen by the user.
     * 
     *      diff -> Enum value of difficulty level chosen by the user from the main menu.
     */
    pub fn new (diff: DifficultyMenuOption) -> Self {
        let mut grid: Grid = Self::init(diff);
        grid.set_starting_positions(diff);
        grid
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
        let solved_puzzle = self.generate_solved_puzzle(&seed);
        //Self::generate_solved_puzzle(unsafe { time(0x0) }); //This will also work

        let mut generator = thread_rng();
        let mut positions: [u8; GRID_SIZE as usize] = from_fn(|i| i as u8);
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
     *      SEED -> Seed for the pseudo-random number sequence
     */
    fn generate_solved_puzzle (&self, seed: &i32) -> [u8; GRID_SIZE as usize] {
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
        let dist = Uniform::new_inclusive(1, CONTAINER_SIZE + 1);
        let mut values: [u8; CONTAINER_SIZE as usize] = from_fn(|i| i as u8 + 1);

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
        let soln_rows: [Row; NUM_CONTAINERS as usize] = from_fn(|i| Row::new(CONTAINER::ROW, soln_matrix[i]));
        let soln_columns: [Column; NUM_CONTAINERS as usize] = from_fn(
            |i| {
                /*let mut temp_col: [u8; NUM_CONTAINERS as usize] = [0; NUM_CONTAINERS as usize];
                for j in 0..NUM_CONTAINERS {
                    temp_col[j] = soln_matrix[j][i];
                }
                Column::new(CONTAINER::COLUMN, temp_col)*/
                Column::new(CONTAINER::COLUMN, from_fn(|j| soln_matrix[j][i]))
            }
        );
        let soln_boxes: [Box; NUM_CONTAINERS as usize] = from_fn(
            |_| {
                let mut arr: [u8; NUM_CONTAINERS as usize] = [0; NUM_CONTAINERS as usize];
                for j in [1, 4, 7] {
                    for k in [1, 4, 7] {
                        arr[0] = soln_matrix[j-1][k-1];
                        arr[1] = soln_matrix[j-1][k];
                        arr[2] = soln_matrix[j-1][k+1];
                        arr[3] = soln_matrix[j][k-1];
                        arr[4] = soln_matrix[j][k];
                        arr[5] = soln_matrix[j][k+1];
                        arr[6] = soln_matrix[j+1][k-1];
                        arr[7] = soln_matrix[j+1][k];
                        arr[8] = soln_matrix[j+1][k+1];
                    }
                }
                Box::new(CONTAINER::BOX, arr)
            }
        );

        let soln_found: bool = self.solve(1, 1, &soln_rows, &soln_columns, &soln_boxes);
        
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
    fn solve (&self, BOX: u8, VALUE: u8, rows: &[Row; NUM_CONTAINERS as usize],
              columns: &[Column; NUM_CONTAINERS as usize],
              boxes: &[Box; NUM_CONTAINERS as usize]) -> bool {
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
            if !rows[ROW_NUMBER].value_exists(VALUE) &&
               !columns[COLUMN_NUMBER].value_exists(VALUE) &&
               self.is_known(positions[i] as usize) {
                available_pos.add(positions[i]);
            }
        }

        /* NOTE: set_value cannot be used here because the rows, columns, and boxes being used
         *       are not the Grid's internal Containers. They belong to the solution matrix and
         *       are completely separate. Interesting things happened when I tested that out
         *       before I realized why it wouldn't work.
         */
        let stop: bool = false;
        let soln: bool = true;
        //while true {
        while !stop {
            if available_pos.size() == 0 {
                return false
            }

            let next_available_pos = available_pos.peek()
                .expect("Error retrieving next position while solving...");
            //let ROW_NUMBER: usize = Self::map_row(next_available_pos);
            //let COLUMN_NUMBER: usize = Self::map_column(next_available_pos);
            //let BOX_NUMBER: usize = BOX as usize;
            let (ROW_NUMBER, COLUMN_NUMBER, BOX_NUMBER): (usize, usize, usize) =
                Self::map_containers(available_pos);
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
                available_pos.remove();
            }            
        }
        
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
    fn get_row (&self, INDEX: usize) -> &mut Row {
        &mut self.rows[INDEX]
    }

    /**
     * Returns an address to the Column Container from this Grid's internal Column array. This
     * allows the Column object to be mutable from the Grid when an input is passed from the
     * Sudoku object.
     * 
     *      INDEX -> The index to return from the Grid's internal Column array.
     */
    fn get_column (&self, INDEX: usize) -> &mut Column {
        &mut self.columns[INDEX]
    }

    /**
     * Returns an address to the Box Container from this Grid's internal Box array. This allows
     * the Box object to be mutable from the Grid when an input is passed from the Sudoku
     * object.
     * 
     *      INDEX -> The index to return from the Grid's internal Box array.
     */
    fn get_box (&self, INDEX: usize) -> &mut Box {
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

        // NOTE: Check the row, column, and box for the value.
        let row: &mut Row = self.get_row(ROW_NUMBER);
        let column: &mut Column = self.get_column(COLUMN_NUMBER);
        let r#box: &mut Box = self.get_box(BOX_NUMBER);

        // NOTE: Add value from solved puzzle to empty puzzle
        row.set_value(INDEX_ROW, VALUE);
        column.set_value(INDEX_COLUMN, VALUE);
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

use Container as House;
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
        let CONVERTED: u8 = match VALUE as char {
            '?' => VALUE,
            _ => VALUE + '0' as u8,
        };
        
        let mut exists: bool = false;
        for i in 0..CONTAINER_SIZE as usize {
            if self.at(i) == CONVERTED {
                exists = true;
            }
        }
        exists
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
