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

const bool DEBUG = false;

/* NOTE:
 * Name: Class Constructor (default)
 * Purpose: Initializes internal array to all '?'.
 * Parameters: None
 */
Grid::Grid (const uint8_t GRID[NUM_CONTAINERS][NUM_CONTAINERS]) {
    grid_map = this->create_map();
    init_positions();
    set_starting_positions(GRID);
}

/* NOTE:
 * Name: Class Constructor (default)
 * Purpose: 
 * Parameters: 
 */
Grid::Grid (const difficulty_level DIFF) {
    grid_map = this->create_map();
    init_positions();
    set_starting_positions(static_cast<uint8_t>(DIFF));
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
void Grid::printw (const bool COLUMN_PRINTING, const bool BOX_PRINTING) {
    uint8_t y,
            x;
    getyx(stdscr, y, x);
    mvprintw(y, x, COLUMN_PRINTING, BOX_PRINTING);
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
void Grid::mvprintw (const uint8_t YCOORD, const uint8_t XCOORD, const bool COLUMN_PRINTING,
                     const bool BOX_PRINTING) {
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
    uint8_t y = YCOORD,
            x = XCOORD;

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

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
Box& Grid::get_box (const uint8_t INDEX) {
    return boxes[INDEX];
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
Row& Grid::get_row (const uint8_t INDEX) {
    return rows[INDEX];
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
Column& Grid::get_column (const uint8_t INDEX) {
    return cols[INDEX];
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
void Grid::init_positions() {
    for (uint8_t i = 0; i < GRID_SIZE; i++) {
        known_positions[i] = false;
    }
}

/*
 * NOTE: ALGORITHM FOR SOLVING SUDOKU PUZZLE (essentially Bowman's Bingo technique)
 * args <- box # [1-3, 5-7], value # [1-9], row array, column array, box array
 * queue <- available positions on board [0-80]
 * do next_pos <- queue.pop() while recursive call <- false
 *    add value to next_pos in appropriate row, column, and box if possible
 *    return true if box=7, value=9, queue not empty
 *    return false otherwise (queue empty)
 *    next_box <- 5 if box=3
 *                   <- 1 if box=7
 *                   <- box+1 otherwise
 *    next_value <- value+1 if box=7
 *               <- same otherwise
 *    remove value from row, column, and box if recursive call <- false
 * end do-while
 */
/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
bool Grid::solve(const uint8_t BOX, const uint8_t VALUE, Row rows[NUM_CONTAINERS],
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
    
    bool soln;
    while (true) {  //NOTE: Doing it this way gets rid of a compiler warning
        if (available_pos.empty()) return false;
        
        const uint8_t ROW_NUMBER = map_row(available_pos.front()),
                      COLUMN_NUMBER = map_column(available_pos.front()),
                      BOX_NUMBER = BOX,
                      ROW_INDEX = get_row_index(available_pos.front()),
                      COLUMN_INDEX = get_column_index(available_pos.front()),
                      BOX_INDEX = get_box_index(available_pos.front());
        uint8_t next_box,
                next_value;
        rows[ROW_NUMBER].set_value(ROW_INDEX, VALUE + ZERO);
        columns[COLUMN_NUMBER].set_value(COLUMN_INDEX, VALUE + ZERO);
        boxes[BOX_NUMBER].set_value(BOX_INDEX, VALUE + ZERO);
        known_positions[available_pos.front()] = true;
        
        if (BOX == 7 and VALUE == 9) return true;
        
        if (BOX == 3) next_box = 5;
        else if (BOX == 7) next_box = 1;
        else next_box = BOX + 1;
        next_value = (BOX == 7) ? VALUE + 1 : VALUE;
        
        if ((soln = solve(next_box, next_value, rows, columns, boxes))) return soln;
        else {
            rows[ROW_NUMBER].set_value(ROW_INDEX, '?');
            columns[COLUMN_NUMBER].set_value(COLUMN_INDEX, '?');
            boxes[BOX_NUMBER].set_value(BOX_INDEX, '?');
            known_positions[available_pos.front()] = false;
            available_pos.pop();
        }
    }
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
array<uint8_t, GRID_SIZE> Grid::generate_solved_puzzle (const time_t SEED) {
    array<uint8_t, GRID_SIZE> soln;
    uint8_t soln_matrix[NUM_CONTAINERS][NUM_CONTAINERS];
    mt19937 generator(SEED);
    uniform_int_distribution<uint8_t> dist (1, CONTAINER_SIZE);
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

    //NOTE: Fill in boxes along the diagonal first
    for (uint8_t i = 0; i < NUM_CONTAINERS; i += 3) {
        shuffle(begin(values), end(values), generator);
        uint8_t count = 0;
        for (uint8_t j = i; j < i + 3; j++) {
            for (uint8_t k = i; k < i + 3; k++) {
                soln_matrix[j][k] = values[count] + ZERO;
                count++;
            }
        }
    }

    //NOTE: Create row, column, and box objects from partial solution matrix
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
    
    for (uint8_t i = 0; i < NUM_CONTAINERS; i++) {
        for (uint8_t j = 0; j < NUM_CONTAINERS; j++) {
            soln[i * CONTAINER_SIZE + j] = soln_matrix[i][j];
        }
    }
    return soln;
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
void Grid::set_starting_positions (const uint8_t GRID[NUM_CONTAINERS][NUM_CONTAINERS]) {
    for (uint8_t i = 0; i < GRID_SIZE; i++) {
        const uint8_t ROW_NUMBER = map_row(i),
                      COLUMN_NUMBER = map_column(i),
                      BOX_NUMBER = map_box(ROW_NUMBER, COLUMN_NUMBER),
                      INDEX_ROW = get_row_index(i),
                      INDEX_COLUMN = get_column_index(i),
                      INDEX_BOX = get_box_index(i),
                      VALUE = GRID[i/NUM_CONTAINERS][i%NUM_CONTAINERS];
                      
        //NOTE: Check the row, column, and box for the value.
        //NOTE: Why these require the ampersand, I'm not really sure.
        Row &row = get_row(ROW_NUMBER);
        Column &column = get_column(COLUMN_NUMBER);
        Box &box = get_box(BOX_NUMBER);
        
        //NOTE: Add value from solved puzzle to empty puzzle
        row.set_value(INDEX_ROW, VALUE);
        column.set_value(INDEX_COLUMN, VALUE);
        box.set_value(INDEX_BOX, VALUE);
        
        known_positions[i] = (VALUE == '?') ? false : true;
    }
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
void Grid::set_starting_positions (const uint8_t NUM_POSITIONS) {
    time_t seed = time(nullptr);
    array<uint8_t, GRID_SIZE> solved_puzzle = generate_solved_puzzle(seed);
    
    uint8_t positions[GRID_SIZE];
    for (uint8_t i = 0; i < GRID_SIZE; i++) positions[i] = i;
    shuffle(begin(positions), end(positions), mt19937(seed));
    
    for (uint8_t i = 0; i < NUM_POSITIONS; i++) {
        uint8_t pos = positions[i],
                value,
                index_row,
                index_column,
                index_box;
        
        const uint8_t ROW_NUMBER = map_row(pos),
                      COLUMN_NUMBER = map_column(pos),
                      BOX_NUMBER = map_box(ROW_NUMBER, COLUMN_NUMBER);
                      
        //NOTE: Check the row, column, and box for the value.
        //NOTE: Why these require the ampersand, I'm not really sure...
        Row &row = get_row(ROW_NUMBER);
        Column &column = get_column(COLUMN_NUMBER);
        Box &box = get_box(BOX_NUMBER);

        //NOTE: Get indeces for particular row, column, and box
        index_row = get_row_index(pos);
        index_column = get_column_index(pos);
        index_box = get_box_index(pos);
        
        //NOTE: Add value from solved puzzle to empty puzzle
        value = solved_puzzle[pos];
        row.set_value(index_row, value);
        column.set_value(index_column, value);
        box.set_value(index_box, value);
        
        known_positions[pos] = true;

        if (DEBUG) {    //TODO: Change this to use a macro
            clear();
            printw(false, false);
            this_thread::sleep_for(chrono::seconds(1));
            refresh();
        }
    }
    
    //NOTE: For some reason, this is actually required, even though it seems redundant.
    for (uint8_t i = NUM_POSITIONS; i < GRID_SIZE; i++) {
        known_positions[positions[i]] = false;
    }
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
uint8_t Grid::map_row (const uint8_t POS) {
    return POS / NUM_CONTAINERS;
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
uint8_t Grid::map_column (const uint8_t POS) {
    return POS % NUM_CONTAINERS;
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
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
 * Name: 
 * Purpose: 
 * Parameters: 
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
 * Name: 
 * Purpose: 
 * Parameters: 
 */
uint8_t Grid::get_column_index (const uint8_t POS) {
    return POS / CONTAINER_SIZE;
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
uint8_t Grid::get_box_index (const uint8_t POS) {
    const uint8_t ROW = get_row_index(POS),
                  COLUMN = get_column_index (POS);
    return 3 * (COLUMN % 3) + ROW % 3;
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
map<uint8_t, cell> Grid::create_map() {
    map<uint8_t, cell> m;
    for (uint8_t i = 0; i < NUM_CONTAINERS; i++) {
        for (uint8_t j = 0; j < NUM_CONTAINERS; j++) {
            m[i * CONTAINER_SIZE + j] = cell(i, j);
        }
    }
    return m;
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
const cell Grid::get_position (const uint8_t INDEX) {
    return grid_map[INDEX];
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
uint8_t Grid::get_map_size() const {
    return grid_map.size();
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
uint8_t Grid::at(const uint8_t INDEX) {
    return get_row(map_row(INDEX))[get_row_index(INDEX)];
    //return get_column(map_column(INDEX))[get_column_index(INDEX)];
    //return get_box(map_box_index(INDEX))[get_box_index(INDEX)];
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
uint8_t Grid::operator [] (const uint8_t INDEX) {
    return at(INDEX);
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
 */
bool Grid::is_known (uint8_t index) {
    return known_positions[index];
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters: 
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
