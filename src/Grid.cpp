#include <ncurses.h>
#include "Grid.hpp"
#include <ctime>    //time_t, time()
#include "colors.hpp"
#include <thread>
#include <chrono>
#include <iterator>
#include <algorithm>
#include <cstdlib>
#include <queue>

using namespace std;

/* NOTE:
 * Name: Class Constructor (overloaded)
 * Purpose: Begins initialization of internal Container data structures using the GRID parameter.
 * Parameters:
 *      GRID -> Array of values that fill in the values of the Grid's Box, Row, and Column member
 *              variables.
 */
Grid::Grid (const uint8_t GRID[NUM_CONTAINERS][NUM_CONTAINERS]) {
    create_map();
    init_known_positions();
    set_starting_positions(GRID);
}

/* NOTE:
 * Name: Class Constructor (overloaded)
 * Purpose: Begins initialization of internal Container data structures based on the difficulty
 *          level chosen by the user.
 * Parameters:
 *      DIFF -> Enum value of difficulty level chosen by the user from the main menu.
 */
Grid::Grid (const difficulty_level DIFF) {
    create_map();
    init_known_positions();
    set_starting_positions(static_cast<uint8_t>(DIFF));
}

#if DEBUG
/* NOTE:
 * Name: printw
 * Purpose: Prints out current Grid contents at the current cursor location based on how they are
 *          stored as either Rows, Columns, or Boxes. This is a wrapper for NCurses's printw
 *          function.
 * Parameters:
 *      COLUMN_PRINTING -> Boolean informing whether to print using Columns.
 *      BOX_PRINTING -> Boolean informing whether to print using Boxes.
 */
void Grid::printw (const bool COLUMN_PRINTING, const bool BOX_PRINTING) {
    uint8_t y,
            x;
    getyx(stdscr, y, x);
    mvprintw(cell {y, x}, COLUMN_PRINTING, BOX_PRINTING);
}

/* NOTE:
 * Name: mvprintw
 * Purpose: Moves cursor to a position and prints out current Grid contents based on how they are
 *          stored as either Rows, Columns, or Boxes. This is a wrapper for NCurses's printw
 *          function.
 * Parameters:
 *      COORDS -> Cell (line, column) where cursor is moved to before printing.
 *      COLUMN_PRINTING -> Boolean informing whether to print using Columns.
 *      BOX_PRINTING -> Boolean informing whether to print using Boxes.
 */
void Grid::mvprintw (const cell COORDS, const bool COLUMN_PRINTING, const bool BOX_PRINTING) {
    /*  NOTE: (i,j) mapping of sudoku grid
     * 
     *        NUMBERED (ROW x COLUMN)
     *    00 01 02 | 10 11 12 | 20 21 22
     *    03 04 05 | 13 14 15 | 23 24 25
     *    06 07 08 | 16 17 18 | 26 27 28
     *    ---------|----------|---------
     *    30 31 32 | 40 41 42 | 50 51 52
     *    33 34 35 | 43 44 45 | 53 54 55
     *    36 37 38 | 46 47 48 | 56 57 58
     *    ---------|----------|---------
     *    60 61 62 | 70 71 72 | 80 81 82
     *    63 64 65 | 73 74 75 | 83 84 85
     *    66 67 68 | 76 77 78 | 86 87 88
     */
    uint8_t y = COORDS.first,
            x = COORDS.second;

    if (BOX_PRINTING) {   //NOTE: For printing from a box
        for (uint8_t i = 0; i < CONTAINER_SIZE; i += 3) {
            uint8_t count = 0,
                    offset = 0;
            while (count < 3) {
                move(y, x);
                for (uint8_t j = i; j < i + 3; j++) {
                    Box box = get_box(j);
                    for (uint8_t k = 0; k < 3; k++) {
                        if (box[k + offset] >= ONE and box[k + offset] <= NINE) {
                            attron(COLOR_PAIR(GIVEN));
                            ::printw("%c", box[k + offset]);
                            attroff(COLOR_PAIR(GIVEN));
                        }
                        else ::printw("%c", box[k + offset]);
                    }

                    if (j != 2 and j != 5 and j != 8) ::printw("|");
                }
                
                count++;
                offset += 3;
                y++;
            }

            if (i < 6) {
                ::mvprintw(y, x, "---|---|---");
                y++;
            }
        }
    }
    else {
        for (uint8_t i = 0; i < CONTAINER_SIZE; i++) {
            move(y, x);
            for (uint8_t j = 0; j < CONTAINER_SIZE; j++) {
                if (COLUMN_PRINTING) {  //NOTE: For printing using columns
                    if (get_column(j)[i] >= ONE and get_column(j)[i] <= NINE) {
                        attron(COLOR_PAIR(GIVEN));
                        ::printw("%c", get_column(j)[i]);
                        attroff(COLOR_PAIR(GIVEN));
                    }
                    else ::printw("%c", get_column(j)[i]);
                }
                else {  //NOTE: For printing using rows
                    if (get_row(i)[j] >= ONE and get_row(i)[j] <= NINE) {
                        attron(COLOR_PAIR(GIVEN));
                        ::printw("%c", get_row(i)[j]);
                        attroff(COLOR_PAIR(GIVEN));
                    }
                    else ::printw("%c", get_row(i)[j]);
                }
                
                if (j == 2 or j == 5) ::printw("|");
            }
            
            if (i == 2 or i == 5) {
                ::mvprintw(y + 1, x, "---|---|---");
                y += 2;
            }
            else y++;
        }
    }
}
#endif

/* NOTE:
 * Name: get_box
 * Purpose: Returns an address to the Box Container from this Grid's internal Box array. This allows
 *          the Box object to be mutable from the Grid when an input is passed from the Sudoku
 *          object.
 * Parameters:
 *      INDEX -> The index to return from the Grid's internal Box array.
 */
Box& Grid::get_box (const uint8_t INDEX) {
    return boxes[INDEX];
}

/* NOTE:
 * Name: get_row
 * Purpose: Returns an address to the Row Container from this Grid's internal Row array. This allows
 *          the Row object to be mutable from the Grid when an input is passed from the Sudoku
 *          object.
 * Parameters:
 *      INDEX -> The index to return from the Grid's internal Row array.
 */
Row& Grid::get_row (const uint8_t INDEX) {
    return rows[INDEX];
}

/* NOTE:
 * Name: get_column
 * Purpose: Returns an address to the Column Container from this Grid's internal Column array. This
 *          allows the Column object to be mutable from the Grid when an input is passed from the
 *          Sudoku object.
 * Parameters:
 *      INDEX -> The index to return from the Grid's internal Row array.
 */
Column& Grid::get_column (const uint8_t INDEX) {
    return cols[INDEX];
}

/* NOTE:
 * Name: init_known_positions
 * Purpose: Initializes the Grid's internal boolean array known_positions to all false values.
 * Parameters: None
 */
void Grid::init_known_positions () {
    for (uint8_t i = 0; i < GRID_SIZE; i++) {
        known_positions[i] = false;
    }
}

/* NOTE:
 * Name: solve
 * Purpose: Recursively generates a solved sudoku puzzle using the Bowman's Bingo technique. The
 *          algorithm recursively focuses on placing the same value into each box before working to
 *          place the next value (i.e. each box is iterated through placing a 1 in a valid position,
 *          then the same is done for 2, followed by 3, etc.). Even though it is technically
 *          possible for false to be returned up the recursive chain to generate_solved_puzzle,
 *          indicating that a solved puzzle couldn't be generated, this logically should never
 *          happen (i.e. this function always returns a solved puzzle). The solved puzzle is
 *          "returned" in the sense that the Row, Column, and Box parameters will be filled after
 *          this function successfully returns. The algorithm for this is described below the
 *          parameters list, but like all good algorithms is coded in practice slightly out of
 *          order.
 * Parameters:
 *      BOX -> Box number of the current recursive iteration.
 *      VALUE -> The numerical value 1-9 being placed in the current Box.
 *      rows -> Array of Row objects each representing a row of the solved puzzle. All recursive
 *              iterations have access to the same array.
 *      columns -> Array of Column objects each representing a column of the solved puzzle. All
 *                 recursive iterations have access to the same array.
 *      boxes -> Array of Box objects each representing a box of the solved puzzle. All recursive
 *               iterations have access to the same array.
 * 
 * Bowman's Bingo Algorithm:
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
bool Grid::solve (const uint8_t BOX, const uint8_t VALUE, Row rows[NUM_CONTAINERS],
                  Column columns[NUM_CONTAINERS], Box boxes[NUM_CONTAINERS]) {
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
    
    queue<uint8_t> available_pos;
    uint8_t positions[CONTAINER_SIZE];
    for (uint8_t i = BOX; i >= 3; i -= 3) {
        positions[0] += 27;
    }
    positions[0] += 3 * (BOX % 3);
    
    //Figure out remaining 8 positions in box
    for (uint8_t i = 1; i < CONTAINER_SIZE; i++) {
        positions[i] = positions[0] + CONTAINER_SIZE * (i / 3) + i % 3;
    }
    
    /* NOTE: Figure out positions VALUE can and can't be placed
     *       Map row and column (box shouldn't be needed)
     */
    for (uint8_t i = 0; i < CONTAINER_SIZE; i++) {
        const uint8_t ROW_NUMBER = map_row(positions[i]),
                      COLUMN_NUMBER = map_column(positions[i]);
        if (not rows[ROW_NUMBER].value_exists(VALUE) and
            not columns[COLUMN_NUMBER].value_exists(VALUE) and not is_known(positions[i])) {
            available_pos.push(positions[i]);
        }
    }
    
    /* NOTE: set_value cannot be used here because the rows, columns, and boxes being used are not
     *       the Grid's internal Containers. They belong to the solution matrix and are completely
     *       separate. Interesting things happened when I tested that out before I realized why it
     *       wouldn't work.
     */
    bool soln;
    while (true) {  //NOTE: Doing it this way gets rid of a compiler warning
        if (available_pos.empty()) return false;                        //NOTE: STEP 3
        
        const uint8_t ROW_NUMBER = map_row(available_pos.front()),
                      COLUMN_NUMBER = map_column(available_pos.front()),
                      BOX_NUMBER = BOX,
                      ROW_INDEX = get_row_index(available_pos.front()),
                      COLUMN_INDEX = get_column_index(available_pos.front()),
                      BOX_INDEX = get_box_index(available_pos.front());
        uint8_t next_box,
                next_value;
        rows[ROW_NUMBER].set_value(ROW_INDEX, VALUE + ZERO);            //NOTE: STEP 1
        columns[COLUMN_NUMBER].set_value(COLUMN_INDEX, VALUE + ZERO);
        boxes[BOX_NUMBER].set_value(BOX_INDEX, VALUE + ZERO);
        known_positions[available_pos.front()] = true;
        
        if (BOX == 7 and VALUE == 9) return true;                       //NOTE: STEP 2
        
        if (BOX == 3) next_box = 5;                                     //NOTE: STEP 4
        else if (BOX == 7) next_box = 1;
        else next_box = BOX + 1;
        next_value = (BOX == 7) ? VALUE + 1 : VALUE;                    //NOTE: STEP 5
        
        if ((soln = solve(next_box, next_value, rows, columns, boxes))) return soln;
        else {
            rows[ROW_NUMBER].set_value(ROW_INDEX, '?');                 //NOTE: STEP 6
            columns[COLUMN_NUMBER].set_value(COLUMN_INDEX, '?');
            boxes[BOX_NUMBER].set_value(BOX_INDEX, '?');
            known_positions[available_pos.front()] = false;
            available_pos.pop();
        }
    }
}

/* NOTE:
 * Name: generate_solved_puzzle
 * Purpose: Generates and returns a solved sudoku puzzle. This puzzle is later used to created a
 *          solvable puzzle. The puzzle is generated randomly using a Mersenne-Twister engine.
 * Parameters:
 *      SEED -> Seed for the psuedo-random number sequence.
 */
array<uint8_t, GRID_SIZE> Grid::generate_solved_puzzle (const time_t SEED) {
    array<uint8_t, GRID_SIZE> soln;
    uint8_t soln_matrix[NUM_CONTAINERS][NUM_CONTAINERS];
    mt19937 generator(SEED);    //NOTE: Start a psuedo-random number generator using a
                                //      Mersenne-Twister engine
    uniform_int_distribution<uint8_t> dist (1, CONTAINER_SIZE); //NOTE: Random numbers with values
                                                                //      of 1-81 will be uniformly
                                                                //      distributed.
    uint8_t values[CONTAINER_SIZE];
    for (uint8_t i = 0; i < CONTAINER_SIZE; i++) {
        values[i] = i + 1;
    }

    //NOTE: Initialize matrix with '?' placeholders
    for (uint8_t i = 0; i < NUM_CONTAINERS; i++) {
        for (uint8_t j = 0; j < NUM_CONTAINERS; j++) {
            soln_matrix[i][j] = '?';
        }
    }

    //NOTE: Fill in boxes along the diagonal first. On an empty puzzle, boxes 1, 5, and 9 are
    //      independent of each other, and can be randomly filled in a more trivial manner.
    for (uint8_t i = 0; i < NUM_CONTAINERS; i += 3) {
        shuffle(begin(values), end(values), generator); //NOTE: All grid positions 1-81 are randomly
        uint8_t count = 0;                              //      shuffled.
        for (uint8_t j = i; j < i + 3; j++) {
            for (uint8_t k = i; k < i + 3; k++) {
                soln_matrix[j][k] = values[count] + ZERO;
                count++;
            }
        }
    }

    //NOTE: Create row, column, and box objects from partial solution matrix. These arrays will be
    //      used to finish solving the sudoku puzzle recursively.
    Row soln_rows[NUM_CONTAINERS];
    Column soln_columns[NUM_CONTAINERS];
    Box soln_boxes[NUM_CONTAINERS];

    for (uint8_t i = 0; i < NUM_CONTAINERS; i++) {
        soln_rows[i] = Row(soln_matrix[i]);
    }

    uint8_t temp_col[NUM_CONTAINERS];
    for (uint8_t i = 0; i < NUM_CONTAINERS; i++) {
        for (uint8_t j = 0; j < NUM_CONTAINERS; j++) {
            temp_col[j] = soln_matrix[j][i];
        }
        soln_columns[i] = Column(temp_col);
    }

    uint8_t temp_submat[NUM_CONTAINERS];
    uint8_t count = 0;
    for (uint8_t i = 1; i < NUM_CONTAINERS; i += 3) {
        for (uint8_t j = 1; j < NUM_CONTAINERS; j += 3) {
            temp_submat[0] = soln_matrix[i-1][j-1];
            temp_submat[1] = soln_matrix[i-1][j];
            temp_submat[2] = soln_matrix[i-1][j+1];
            temp_submat[3] = soln_matrix[i][j-1];
            temp_submat[4] = soln_matrix[i][j];
            temp_submat[5] = soln_matrix[i][j+1];
            temp_submat[6] = soln_matrix[i+1][j-1];
            temp_submat[7] = soln_matrix[i+1][j];
            temp_submat[8] = soln_matrix[i+1][j+1];
            soln_boxes[count] = Box(temp_submat);
            count++;
        }
    }
    
    bool soln_found = solve(1, 1, soln_rows, soln_columns, soln_boxes);
    
    for (uint8_t i = 0; i < NUM_CONTAINERS; i++) {
        for (uint8_t j = 0; j < NUM_CONTAINERS; j++) {
            if (soln_matrix[i][j] == '?') soln_matrix[i][j] = soln_rows[i][j];
        }
    }
    
    for (uint8_t i = 0; i < NUM_CONTAINERS; i++) {              //NOTE: Transfer solution matrix to
        for (uint8_t j = 0; j < NUM_CONTAINERS; j++) {          //      std::array to simplify
            soln[i * CONTAINER_SIZE + j] = soln_matrix[i][j];   //      returning back to the
        }                                                       //      caller.
    }
    return soln;
}

/* NOTE:
 * Name: set_starting_positions (overloaded)
 * Purpose: Set starting grid positions using a previously saved game.
 * Parameters:
 *      GRID -> Matrix containing values at all 81 positions from a previously saved game.
 */
void Grid::set_starting_positions (const uint8_t GRID[NUM_CONTAINERS][NUM_CONTAINERS]) {
    for (uint8_t i = 0; i < GRID_SIZE; i++) {
        const uint8_t VALUE = GRID[i/NUM_CONTAINERS][i%NUM_CONTAINERS];
        set_value(i, VALUE);
        known_positions[ as usizei] = (VALUE == '?') ? false : true;
    }
}

/* NOTE:
 * Name: set_starting_positions (overloaded)
 * Purpose: Set starting grid positions for a new game from a randomly generated solved puzzle.
 * Parameters:
 *      NUM_POSITIONS -> The number of given positions to initialize the puzzle. This value will be
 *                       based on difficulty level chosen by the user from the main menu.
 */
void Grid::set_starting_positions (const uint8_t NUM_POSITIONS) {
    time_t seed = time(nullptr);
    array<uint8_t, GRID_SIZE> solved_puzzle = generate_solved_puzzle(seed);
    
    uint8_t positions[GRID_SIZE];
    for (uint8_t i = 0; i < GRID_SIZE; i++) positions[i] = i;
    shuffle(begin(positions), end(positions), mt19937(seed));
    
    for (uint8_t i = 0; i < NUM_POSITIONS; i++) {
        const uint8_t POS = positions[i];
        set_value(POS, solved_puzzle[POS]);
        known_positions[POS] = true;

        #if DEBUG
            clear();
            printw(false, false);
            this_thread::sleep_for(chrono::seconds(1));
            refresh();
        #endif
    }
    
    //NOTE: For some reason, this is actually required, even though it seems redundant.
    for (uint8_t i = NUM_POSITIONS; i < GRID_SIZE; i++) {
        known_positions[positions[i]] = false;
    }
}

/* NOTE:
 * Name: map_row
 * Purpose: Returns the row number based on the grid position.
 * Parameters:
 *      POS -> Grid position 0-80 used to map the appropriate row number.
 */
uint8_t Grid::map_row (const uint8_t POS) {
    return POS / NUM_CONTAINERS;
}

/* NOTE:
 * Name: map_column
 * Purpose: Returns the column number based on the grid position.
 * Parameters:
 *      POS -> Grid position 0-80 used to map the appropriate column number.
 */
uint8_t Grid::map_column (const uint8_t POS) {
    return POS % NUM_CONTAINERS;
}

/* NOTE:
 * Name: map_box
 * Purpose: Returns the box number based on the grid position. This function is reliant on the row
 *          and column having been mapped prior to being called. This simplifies mapping the box
 *          number as the row and column numbers aren't calculated a second time, and is logically
 *          sound since there is never a situation where boxes are mapped independently of rows and
 *          columns.
 * Parameters:
 *      ROW -> Previously mapped row number 0-8 used to map the appropriate box.
 *      COLUMN -> Previously mapped column number 0-8 used to map the appropriate box.
 */
uint8_t Grid::map_box (const uint8_t ROW, const uint8_t COLUMN) {
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
    
    if (ROW < 3) {
        if (COLUMN < 3) return 0;
        else if (COLUMN < 6) return 1;
        else return 2;
    }
    else if (ROW < 6) {
        if (COLUMN < 3) return 3;
        else if (COLUMN < 6) return 4;
        else return 5;
    }
    else {
        if (COLUMN < 3) return 6;
        else if (COLUMN < 6) return 7;
        else return 8;
    }
}

/* NOTE:
 * Name: get_row_index
 * Purpose: Returns the index of a Row object based on the grid position.
 * Parameters:
 *      POS -> Grid position 0-80 used to map the appropriate Row index.
 */
uint8_t Grid::get_row_index (const uint8_t POS) {
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
    return POS % CONTAINER_SIZE;
}

/* NOTE:
 * Name: get_column_index
 * Purpose: Returns the index of a Column object based on the grid position.
 * Parameters:
 *      POS -> Grid position 0-80 used to map the appropriate Column index.
 */
uint8_t Grid::get_column_index (const uint8_t POS) {
    return POS / CONTAINER_SIZE;
}

/* NOTE:
 * Name: get_box_index
 * Purpose: Returns the index of a Box object based on the grid position. This function can't
 *          benefit in a similar manner as map_box since there are times when box indeces are needed
 *          independent of rows and columns.
 * Parameters:
 *      POS -> Grid position 0-80 used to map the appropriate Box index.
 */
uint8_t Grid::get_box_index (const uint8_t POS) {
    const uint8_t ROW = get_row_index(POS),
                  COLUMN = get_column_index (POS);
    return 3 * (COLUMN % 3) + ROW % 3;
}

/* NOTE:
 * Name: set_value
 * Purpose: Places a value into the correct position (row, column, and box) in the grid.
 * Parameters:
 *      POS -> The grid position 0-80 where the value will be placed.
 *      VALUE -> The value to be placed in the grid.
 */
void Grid::set_value (const uint8_t POS, const uint8_t VALUE) {
    const uint8_t ROW_NUMBER = map_row(POS),
                  COLUMN_NUMBER = map_column(POS),
                  BOX_NUMBER = map_box(ROW_NUMBER, COLUMN_NUMBER),
                  INDEX_ROW = get_row_index(POS),
                  INDEX_COLUMN = get_column_index(POS),
                  INDEX_BOX = get_box_index(POS);
                  
    //NOTE: Check the row, column, and box for the value.
    //NOTE: Why these require the ampersand, I'm not really sure...
    Row &row = get_row(ROW_NUMBER);
    Column &column = get_column(COLUMN_NUMBER);
    Box &box = get_box(BOX_NUMBER);
    
    //NOTE: Add value from solved puzzle to empty puzzle
    row.set_value(INDEX_ROW, VALUE);
    column.set_value(INDEX_COLUMN, VALUE);
    box.set_value(INDEX_BOX, VALUE);
}

/* NOTE:
 * Name: create_map
 * Purpose: Creates a mapping of all 81 grid positions to a 9x9 matrix.
 * Parameters: None
 */
void Grid::create_map () {
    for (uint8_t i = 0; i < NUM_CONTAINERS; i++) {
        for (uint8_t j = 0; j < NUM_CONTAINERS; j++) {
            grid_map[i * CONTAINER_SIZE + j] = cell(i, j);
        }
    }
}

/* NOTE:
 * Name: get_position
 * Purpose: Returns the cell mapped to the grid index as mapped in the internal grid_map.
 * Parameters:
 *      INDEX -> Index 0-80 used to return the correct cell based on the mapping in grid_map.
 */
const cell Grid::get_position (const uint8_t INDEX) {
    return grid_map[INDEX];
}

/* NOTE:
 * Name: get_map_size
 * Purpose: Returns the size of the internal grid_map. This function is technically unnecessarily
 *          since the size of grid_map will always be 81.
 * Parameters: None
 */
uint8_t Grid::get_map_size () const {
    return grid_map.size();
}

/* NOTE:
 * Name: at
 * Purpose: Returns the value at a given index from the Grid. This can be done using Rows, Columns,
 *          or Boxes. Rows are used for simplicity, although all three have been tested for
 *          correctness.
 * Parameters:
 *      INDEX -> Index of the grid to return the value from.
 */
uint8_t Grid::at (const uint8_t INDEX) {
    return get_row(map_row(INDEX))[get_row_index(INDEX)];
    //return get_column(map_column(INDEX))[get_column_index(INDEX)];
    //return get_box(map_box_index(INDEX))[get_box_index(INDEX)];
}

/* NOTE:
 * Name: Operator [] (overloaded)
 * Purpose: Returns the value at a given index from the Grid.
 * Parameters:
 *      INDEX -> Index of the Grid to return the value from.
 */
uint8_t Grid::operator [] (const uint8_t INDEX) {
    return at(INDEX);
}

/* NOTE:
 * Name: is_known
 * Purpose: Returns whether a value at a given Grid index is known. Known values correspond to given
 *          values from when the puzzle was first generated.
 * Parameters:
 *      INDEX -> Index of the Grid to check.
 */
bool Grid::is_known (uint8_t index) {
    return known_positions[index];
}

/* NOTE:
 * Name: evaluate
 * Purpose: Evaluates whether the Grid's internal Container arrays values are valid for a solved
 *          sudoku puzzle (i.e. exactly one each of the values 1-9 in each array). These can each be
 *          checked independently for correctness. Any remaining '?' values in any array
 *          automatically result in a return value of false.
 * Parameters: None
 */
bool Grid::evaluate () {
    for (uint8_t i = 0; i < NUM_CONTAINERS; i++) {
        Row row = rows[i];
        Column column = cols[i];
        Box box = boxes[i];
        
        if (not row.evaluate() or not column.evaluate() or not box.evaluate()) return false;
    }
    return true;
}
