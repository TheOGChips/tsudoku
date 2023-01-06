#include "Matrix_9x9.hpp"
#include <ctime>    //time_t, time()
#include <ncurses.h>
#include "colors.hpp"
#include <thread>
#include <chrono>
#include <iterator>
#include <algorithm>
#include <cstdlib>
#include <queue>

using namespace std;

const bool DEBUG = false;

Matrix_9x9::Matrix_9x9 ()
{
    _map_ = this->create_map();
    init_positions();

    //position_generator = mt19937(seed);
    //position_dist = uniform_int_distribution<uint8_t>(0, 80);
    //generator = mt19937(seed);
    //dist = uniform_int_distribution<uint8_t>(1, 9);
    set_starting_positions(17);   //TODO: Implement the following difficulty modes
    //set_starting_positions(80);     //      EASY -> 40, MEDIUM -> 30, HARD -> 17
}                                   //NOTE: This appears to be more complicated than I at
                                    //      first thought. See note in
                                    //      set_starting_positions for more details.
#if false
void Matrix_9x9::print (const bool COLUMN_PRINTING, const bool SUBMATRIX_PRINTING)
{
    /*
            NUMBERED (ROW x COLUMN)
        00 01 02 | 10 11 12 | 20 21 22
        03 04 05 | 13 14 15 | 23 24 25
        06 07 08 | 16 17 18 | 26 27 28
        ---------|----------|---------
        30 31 32 | 40 41 42 | 50 51 52
        33 34 35 | 43 44 45 | 53 54 55
        36 37 38 | 46 47 48 | 56 57 58
        ---------|----------|---------
        60 61 62 | 70 71 72 | 80 81 82
        63 64 65 | 73 74 75 | 83 84 85
        66 67 68 | 76 77 78 | 86 87 88
    */
    if (SUBMATRIX_PRINTING) {   //for printing from a submatrix
        for (uint8_t i = 0; i < 9; i += 3) {
            uint8_t count = 0,
                    offset = 0;
            while (count < 3) {
                for (uint8_t j = i; j < i + 3; j++) {
                    Matrix_3x3 submatrix = get_submatrix(j);
                    for (uint8_t k = 0; k < 3; k++) {
                        if (submatrix[k + offset] >= ONE and submatrix[k + offset] <= NINE) {
                            cout << RED << submatrix[k + offset] << WHITE;
                        }
                        else {
                            cout << submatrix[k + offset];
                        }
                    }

                    if (j != 2 and j != 5 and j != 8) {
                        cout << "|";
                    }
                }
                
                count++;
                offset += 3;
                cout << endl;
            }

            if (i < 6) {
                cout << "---|---|---" << endl;
            }
        }
    }

    else {  //for printing using either rows or columns
        for (uint8_t i = 0; i < 9; i++) {
            for (uint8_t j = 0; j < 9; j++) {
                if (COLUMN_PRINTING) {
                    if (get_column(j)[i] >= ONE and get_column(j)[i] <= NINE) {
                        cout << RED << get_column(j)[i] << WHITE;
                    }
                    else {
                        cout << get_column(j)[i];
                    }
                }
                else {  //if (ROW_PRINTING)
                    if (get_row(i)[j] >= ONE and get_row(i)[j] <= NINE) {
                        cout << RED << get_row(i)[j] << WHITE;
                    }
                    else {
                        cout << get_row(i)[j];
                    }
                }
                
                if (j == 2 or j == 5) {
                    cout << "|";
                }
            }

            cout << endl;
            if (i == 2 or i == 5) {
                cout << "---|---|---" << endl;
            }
        }
    }
}
#endif

void Matrix_9x9::printw (const bool COLUMN_PRINTING, const bool SUBMATRIX_PRINTING)
{
    uint8_t y,
            x;
    getyx(stdscr, y, x);
    mvprintw(y, x, COLUMN_PRINTING, SUBMATRIX_PRINTING);
}

void Matrix_9x9::mvprintw (const uint8_t YCOORD, const uint8_t XCOORD, const bool COLUMN_PRINTING, const bool SUBMATRIX_PRINTING)
{
    /*
            NUMBERED (ROW x COLUMN)
        00 01 02 | 10 11 12 | 20 21 22
        03 04 05 | 13 14 15 | 23 24 25
        06 07 08 | 16 17 18 | 26 27 28
        ---------|----------|---------
        30 31 32 | 40 41 42 | 50 51 52
        33 34 35 | 43 44 45 | 53 54 55
        36 37 38 | 46 47 48 | 56 57 58
        ---------|----------|---------
        60 61 62 | 70 71 72 | 80 81 82
        63 64 65 | 73 74 75 | 83 84 85
        66 67 68 | 76 77 78 | 86 87 88
    */
    uint8_t y = YCOORD,
            x = XCOORD;
    //move(YCOORD, XCOORD);
    //getyx(stdscr, y, x);

    if (SUBMATRIX_PRINTING) {   //for printing from a submatrix
        for (uint8_t i = 0; i < 9; i += 3) {
            uint8_t count = 0,
                    offset = 0;
            while (count < 3) {
                move(y, x);
                for (uint8_t j = i; j < i + 3; j++) {
                    Matrix_3x3 submatrix = get_submatrix(j);
                    for (uint8_t k = 0; k < 3; k++) {
                        if (submatrix[k + offset] >= ONE and submatrix[k + offset] <= NINE) {
                            attron(COLOR_PAIR(KNOWN));
                            ::printw("%c", submatrix[k + offset]);
                            attroff(COLOR_PAIR(KNOWN));
                        }
                        else {
                            ::printw("%c", submatrix[k + offset]);
                        }
                    }

                    if (j != 2 and j != 5 and j != 8) {
                        ::printw("|");
                    }
                }
                
                count++;
                offset += 3;
                y++;
                //x++;
                //::printw("\n");
            }

            if (i < 6) {
                //::printw("---|---|---\n");
                ::mvprintw(y, x, "---|---|---");
                y++;
            }
        }
    }
    else {  //for printing using either rows or columns
        for (uint8_t i = 0; i < 9; i++) {
            move(y, x);
            for (uint8_t j = 0; j < 9; j++) {
                if (COLUMN_PRINTING) {
                    if (get_column(j)[i] >= ONE and get_column(j)[i] <= NINE) {
                        attron(COLOR_PAIR(KNOWN));
                        ::printw("%c", get_column(j)[i]);
                        attroff(COLOR_PAIR(KNOWN));
                    }
                    else {
                        ::printw("%c", get_column(j)[i]);
                    }
                }
                else {  //if (ROW_PRINTING)
                    if (get_row(i)[j] >= ONE and get_row(i)[j] <= NINE) {
                        attron(COLOR_PAIR(KNOWN));
                        ::printw("%c", get_row(i)[j]);
                        attroff(COLOR_PAIR(KNOWN));
                    }
                    else {
                        ::printw("%c", get_row(i)[j]);
                    }
                }
                
                if (j == 2 or j == 5) {
                    ::printw("|");
                }
            }

            //::printw("\n");
            //y++;
            if (i == 2 or i == 5) {
                //::printw("---|---|---\n");
                ::mvprintw(y + 1, x, "---|---|---");
                y += 2;
            }
            else y++;
        }
    }
}

Matrix_3x3& Matrix_9x9::get_submatrix (uint8_t index)
{
    return matrices[index];
}

Row& Matrix_9x9::get_row (uint8_t index)
{
    return rows[index];
}

Column& Matrix_9x9::get_column (uint8_t index)
{
    return cols[index];
}

void Matrix_9x9::init_positions()
{
    for (uint8_t i = 0; i < 81; i++) {
        known_positions[i] = false;
    }
}

/*
 * NOTE: ALGORITHM FOR SOLVING SUDOKU PUZZLE
 * args <- submatrix # [1-3, 5-7], value # [1-9], row array, column array, submatrix array
 * queue <- available positions on board [0-80]
 * do next_pos <- queue.pop() while recursive call <- false
 *    add value to next_pos in appropriate row, column, and submatrix if possible
 *    return true if submatrix=7, value=9, queue not empty
 *    return false otherwise (queue empty)
 *    next_submatrix <- 5 if submatrix=3
 *                   <- 1 if submatrix=7
 *                   <- submatrix+1 otherwise
 *    next_value <- value+1 if submatrix=7
 *               <- same otherwise
 *    remove value from row, column, and submatrix if recursive call <- false
 * end do-while
 */
bool Matrix_9x9::solve(uint8_t submatrix, uint8_t value, Row rows[9], Column columns[9], Matrix_3x3 submatrices[9]) {
    queue<uint8_t> available_pos;
    uint8_t positions[9];
    //Figure out positions in submatrix based on submatrix number
    //Start with upper right
    /*
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
    for (uint8_t i = submatrix; i >= 3; i -= 3) {
        positions[0] += 27;
    }
    positions[0] += 3 * (submatrix % 3);
    
    //Figure out remaining 8 positions in submatrix
    for (uint8_t i = 1; i < 9; i++) {
        positions[i] = positions[0] + 9 * (i / 3) + i % 3;
    }
    
    #if false
        printf("\nPRINTING POSITIONS IN SUBMATRIX\n");
        for (uint8_t i = 0; i < 9; i++) {
            printf("pos: %hhu\n", positions[i]);
        }
        exit(EXIT_SUCCESS);
    #endif
    
    //Figure out positions value can and can't be placed
    //map row and column (submatrix shouldn't be needed)
    for (uint8_t i = 0; i < 9; i++) {
        uint8_t row_number = map_row(positions[i]),
                column_number = map_column(positions[i]);
        if (not rows[row_number].value_exists(value) and
            not columns[column_number].value_exists(value) and not is_known(positions[i])) {
            available_pos.push(positions[i]);
        }
    }
    
    #if false
        printf("\nPRINTING AVAILABLE POSITIONS\n");
        while (!available_pos.empty()) {
            printf("pos: %hhu\n", available_pos.front());
            available_pos.pop();
        }
        exit(EXIT_SUCCESS);
    #endif
    
    //printf("submatrix: %d, value: %d\n", submatrix, value);
    bool soln;
    while (true) {  //NOTE: Doing it this way gets rid of a compiler warning
        if (available_pos.empty()) return false;
        
        uint8_t row_number = map_row(available_pos.front()),
                column_number = map_column(available_pos.front()),
                submatrix_number = submatrix,
                row_index = get_row_index(available_pos.front()),
                column_index = get_column_index(available_pos.front()),
                submatrix_index = get_submatrix_index(available_pos.front()),
                next_submatrix,
                next_value;
        rows[row_number].set_value(row_index, value + 48);
        columns[column_number].set_value(column_index, value + 48);
        submatrices[submatrix_number].set_value(submatrix_index, value + 48);
        known_positions[available_pos.front()] = true;
        
        if (submatrix == 7 and value == 9) return true;
        
        if (submatrix == 3) next_submatrix = 5;
        else if (submatrix == 7) next_submatrix = 1;
        else next_submatrix = submatrix + 1;
        next_value = (submatrix == 7) ? value + 1 : value;
        
        if ((soln = solve(next_submatrix, next_value, rows, columns, submatrices))) return soln;
        else {
            rows[row_number].set_value(row_index, '?');
            columns[column_number].set_value(column_index, '?');
            submatrices[submatrix_number].set_value(submatrix_index, '?');
            known_positions[available_pos.front()] = false;
            available_pos.pop();
        }
    }
}

array<uint8_t, 81> Matrix_9x9::generate_solved_puzzle (time_t seed) {
    array<uint8_t, 81> soln;
    uint8_t soln_matrix[9][9];
    mt19937 generator(0);   //TODO: Replace this with seed when the time comes
    uniform_int_distribution<uint8_t> dist (1, 9);
    uint8_t values[9];
    for (uint8_t i = 0; i < 9; i++) {
        values[i] = i + 1;
    }

    //Initialize matrix with '?' placeholders
    for (uint8_t i = 0; i < 9; i++) {
        for (uint8_t j = 0; j < 9; j++) {
            soln_matrix[i][j] = '?';
        }
    }

    //Fill in submatrices along the diagonal first
    for (uint8_t i = 0; i < 9; i += 3) {
    //for (uint8_t i = 0; i < 3; i += 3) {
        shuffle(begin(values), end(values), generator);
        uint8_t count = 0;
        for (uint8_t j = i; j < i + 3; j++) {
            for (uint8_t k = i; k < i + 3; k++) {
                soln_matrix[j][k] = values[count] + 48;
                count++;
                //printf("j: %u, k: %u\n", j, k);
            }
        }
    }

    //Create row, column, and submatrix objects from partial solution matrix
    Row soln_rows[9];
    Column soln_columns[9];
    Matrix_3x3 soln_matrices[9];

    for (uint8_t i = 0; i < 9; i++) {
        soln_rows[i] = Row(soln_matrix[i]);
    }

    uint8_t temp_col[9];
    for (uint8_t i = 0; i < 9; i++) {
        for (uint8_t j = 0; j < 9; j++) {
            temp_col[j] = soln_matrix[j][i];
        }
        soln_columns[i] = Column(temp_col);
    }

    uint8_t temp_submat[9];
    uint8_t count = 0;
    for (uint8_t i = 1; i < 9; i += 3) {
        for (uint8_t j = 1; j < 9; j += 3) {
            temp_submat[0] = soln_matrix[i-1][j-1];
            temp_submat[1] = soln_matrix[i-1][j];
            temp_submat[2] = soln_matrix[i-1][j+1];
            temp_submat[3] = soln_matrix[i][j-1];
            temp_submat[4] = soln_matrix[i][j];
            temp_submat[5] = soln_matrix[i][j+1];
            temp_submat[6] = soln_matrix[i+1][j-1];
            temp_submat[7] = soln_matrix[i+1][j];
            temp_submat[8] = soln_matrix[i+1][j+1];
            soln_matrices[count] = Matrix_3x3(temp_submat);
            count++;
        }
    }
    
    bool soln_found = solve(1, 1, soln_rows, soln_columns, soln_matrices);
    
    for (uint8_t i = 0; i < 9; i++) {
        for (uint8_t j = 0; j < 9; j++) {
            if (soln_matrix[i][j] == '?') {
                soln_matrix[i][j] = soln_rows[i][j];
            //if (soln_matrix[j][i] == '?') {
            //    soln_matrix[j][i] = soln_columns[i][j];
            }
        }
    }
    
    for (uint8_t i = 0; i < 9; i++) {
        for (uint8_t j = 0; j < 9; j++) {
            soln[i * 9 + j] = soln_matrix[i][j];
        }
    }
    return soln;
}

void Matrix_9x9::set_starting_positions (const uint8_t NUM_POSITIONS) {
    time_t seed = time(nullptr);
    array<uint8_t, 81> solved_puzzle = generate_solved_puzzle(seed);
    
    #if false
        printf("\n");
        for (uint8_t i = 0; i < 81; i++) {
            printf("%c", solved_puzzle[i]);
            if (i % 9 == 2 or i % 9 == 5) printf("|");
            if (i % 9 == 8) printf("\n");
            if (i == 26 or i == 53) printf("---|---|---\n");
        }
        exit(EXIT_SUCCESS);
    #endif
        
    uint8_t positions[81]/*,
            values[9]*/;
    for (uint8_t i = 0; i < 81; i++) positions[i] = i;
    //for (uint8_t i = 0; i < 9; i++) values[i] = i + 1;
    shuffle(begin(positions), end(positions), mt19937(seed));
    
    for (uint8_t i = 0; i < NUM_POSITIONS; i++) {
        //::clear();
        //::printw("iter: %d", i);
        //::getch();
        uint8_t pos = positions[i],
                value,
                index_row,
                index_column,
                index_submatrix;
        //bool //valid_position = false,
             //valid_value = false;
        /*while (!valid_position) {     
            pos = next_position();      //get next position (random)
            if (!positions[pos]) {      //check if position is already filled
                valid_position = true;
                positions[pos] = true;
            }
        }*/
        
        const uint8_t ROW_NUMBER = map_row(pos),                                      //map position to row
                      COLUMN_NUMBER = map_column(pos),                                //map position to column
                      SUBMATRIX_NUMBER = map_submatrix(ROW_NUMBER, COLUMN_NUMBER);    //map position to 3x3 submatrix
                      
        //check the row, column, and submatrix for the value
        Row &row = get_row(ROW_NUMBER); //NOTE: why these require the ampersand, I'm not really sure
        Column &column = get_column(COLUMN_NUMBER);
        Matrix_3x3 &submatrix = get_submatrix(SUBMATRIX_NUMBER);

        //shuffle(begin(values), end(values), mt19937(seed));
        //while (!valid_value) {
        /*for (uint8_t j = 0; j < 9; j++) {
            //value = next_value();
            value = values[j];
            #if false
                ::initscr();
                ::printw("value: %d\n", value);
                ::printw("pos: %d\n", pos + 1);
                ::printw("ROW #: %d\n", ROW_NUMBER + 1);
                ::printw("COL #: %d\n", COLUMN_NUMBER + 1);
                ::printw("MAT #: %d\n", SUBMATRIX_NUMBER + 1);
                ::printw("# pos: %d\n", i)
                enum print_by {r, c, s};
                for (uint8_t k = r; k <= s; k++) {
                    //::mvprintw(9, 40 + 20 * i, "Printing by ");
                    ::move(9, 40 + 20 * k);
                    if (k == s) {
                        ::printw("submatrix");
                    }
                    else if (k == c) {
                        ::printw("column");
                    }
                    else {
                        ::printw("row");
                    }
                    mvprintw(10, 40 + 20 * k, k & c, k & s);
                    //refresh();  //flush output to screen
                    //getch();    //wait for user input
                    //clear();    //clear the screen
                }
                ::refresh();
                ::getch();
                ::clear();
                ::endwin();
            #endif
            if (!row.value_exists(value) and !column.value_exists(value) and !submatrix.value_exists(value)) {
                //valid_value = true;
                break;
            }
        }*/

        //Get indeces for particular row, column, and submatrix
        index_row = get_row_index(pos);
        index_column = get_column_index(pos);
        index_submatrix = get_submatrix_index(pos);
        //value += 48;    //convert in order to display proper character
        value = solved_puzzle[pos];
        row.set_value(index_row, value);
        column.set_value(index_column, value);
        submatrix.set_value(index_submatrix, value);
        /*cout << "value: " << value << endl
             << "row[" << index_row+0 << "]: " << row[index_row] << endl;
        print(false, false);*/
        known_positions[pos] = true;

        if (DEBUG) {
            clear();
            printw(false, false);
            this_thread::sleep_for(chrono::seconds(1));
            refresh();
        }
    }
    
    for (uint8_t i = NUM_POSITIONS; i < 81; i++) {
        known_positions[positions[i]] = false;
    }
    
    /*cout << endl;
    for (uint8_t i = 0; i < 81; i++) {
        if (positions[i] == true) {
            cout << RED << positions[i] << WHITE;
        }
        else {
            cout << positions[i];
        }
    }*/
}
#if false
uint8_t Matrix_9x9::next_position()
{
    return position_dist(position_generator);
}
#endif
uint8_t Matrix_9x9::map_row (const uint8_t POS)
{
    return POS / 9;
}

uint8_t Matrix_9x9::map_column (const uint8_t POS)
{
    return POS % 9;
}

uint8_t Matrix_9x9::map_submatrix (const uint8_t ROW, const uint8_t COLUMN)
{
    /*
                NUMBERED (0-80)                NUMBERED (ROW x COLUMN)
         0  1  2 |  3  4  5 |  6  7  8      00 01 02 | 03 04 05 | 06 07 08
         9 10 11 | 12 13 14 | 15 16 17      10 11 12 | 13 14 15 | 16 17 18
        18 19 20 | 21 22 23 | 24 25 26      20 21 22 | 23 24 25 | 26 27 28
        ---------|----------|---------      ---------|----------|---------
        27 28 29 | 30 31 32 | 33 34 35      30 31 32 | 33 34 35 | 36 37 38
        36 37 38 | 39 40 41 | 42 43 44      40 41 42 | 43 44 45 | 46 47 48
        45 46 47 | 48 49 50 | 51 52 53      50 51 52 | 53 54 55 | 56 57 58
        ---------|----------|---------      ---------|----------|---------
        54 55 56 | 57 58 59 | 60 61 62      60 61 62 | 63 64 65 | 66 67 68
        63 64 65 | 66 67 68 | 69 70 71      70 71 72 | 73 74 75 | 76 77 78
        72 73 74 | 75 76 77 | 78 79 80      80 81 82 | 83 84 85 | 86 87 88
    */
    //cout << "ROW: " << ROW+0 << ", COLUMN: " << COLUMN+0 << endl;
    if (ROW < 3) {
        if (COLUMN < 3) {
            return 0;
        }
        else if (COLUMN < 6) {
            return 1;
        }
        else {
            return 2;
        }
    }
    else if (ROW < 6) {
        if (COLUMN < 3) {
            return 3;
        }
        else if (COLUMN < 6) {
            return 4;
        }
        else {
            return 5;
        }
    }
    else {
        if (COLUMN < 3) {
            return 6;
        }
        else if (COLUMN < 6) {
            return 7;
        }
        else {
            return 8;
        }
    }
}
#if false
uint8_t Matrix_9x9::next_value()
{
    return dist(generator);
}
#endif
uint8_t Matrix_9x9::get_row_index (const uint8_t POS)
{
    /*
                NUMBERED (0-80)                NUMBERED (ROW x COLUMN)
         0  1  2 |  3  4  5 |  6  7  8      00 01 02 | 03 04 05 | 06 07 08
         9 10 11 | 12 13 14 | 15 16 17      10 11 12 | 13 14 15 | 16 17 18
        18 19 20 | 21 22 23 | 24 25 26      20 21 22 | 23 24 25 | 26 27 28
        ---------|----------|---------      ---------|----------|---------
        27 28 29 | 30 31 32 | 33 34 35      30 31 32 | 33 34 35 | 36 37 38
        36 37 38 | 39 40 41 | 42 43 44      40 41 42 | 43 44 45 | 46 47 48
        45 46 47 | 48 49 50 | 51 52 53      50 51 52 | 53 54 55 | 56 57 58
        ---------|----------|---------      ---------|----------|---------
        54 55 56 | 57 58 59 | 60 61 62      60 61 62 | 63 64 65 | 66 67 68
        63 64 65 | 66 67 68 | 69 70 71      70 71 72 | 73 74 75 | 76 77 78
        72 73 74 | 75 76 77 | 78 79 80      80 81 82 | 83 84 85 | 86 87 88
    */
    //map row index
    return POS % 9;
}

uint8_t Matrix_9x9::get_column_index (const uint8_t POS)
{
    //map column index
    return POS / 9;
}

uint8_t Matrix_9x9::get_submatrix_index (const uint8_t POS)
{
    uint8_t row = get_row_index(POS),
            column = get_column_index (POS);
    //map submatrix index
    return 3 * (column % 3) + row % 3;
}

map<uint8_t, cell> Matrix_9x9::create_map()
{
    map<uint8_t, cell> m;

    for (uint8_t i = 0; i < 9; i++) {
        for (uint8_t j = 0; j < 9; j++) {
            //m[i * 9 + j] = pair<uint8_t, uint8_t>(i, j);
            m[i * 9 + j] = cell(i, j);
        }
    }

    return m;
}

const cell Matrix_9x9::get_map (uint8_t index)
{
    return _map_[index];
}

uint8_t Matrix_9x9::get_map_size() const
{
    return _map_.size();
}

uint8_t Matrix_9x9::at(uint8_t index)
{
    return get_row(map_row(index))[get_row_index(index)];
    //return get_column(map_column(index))[get_column_index(index)];
    //return get_submatrix(map_submatrix_index(index))[get_submatrix_index(index)];
}
//TODO: Make parameter const in all classes with this operator
uint8_t Matrix_9x9::operator [] (uint8_t index)
{
    return at(index);
}

bool Matrix_9x9::is_known (uint8_t index) {
    return known_positions[index];
}
