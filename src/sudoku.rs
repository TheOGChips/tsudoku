use crate::terminal::{
    display::{
        DISPLAY_MATRIX_ROWS,
        DISPLAY_MATRIX_COLUMNS,
        ORIGIN,
    },
    Cell,
};
use std::collections::HashMap;

const GRID_SIZE: u8 = 81;

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
    pub fn new (/*saved_puzzle: &SavedPuzzle*/) -> Self {
        let (grid2display, display2grid) = Self::create_maps();
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
}

struct Grid;
