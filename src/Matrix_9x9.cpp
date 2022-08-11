#include "Matrix_9x9.hpp"
#include <ctime>    //time_t, time()
#include <ncurses.h>
#include "colors.hpp"

using namespace std;

Matrix_9x9::Matrix_9x9 ()
{
    _map_ = this->create_map();
    init_positions();
    time_t seed = time(nullptr);
    position_generator = mt19937(seed);
    position_dist = uniform_int_distribution<uint8_t>(0, 80);
    generator = mt19937(seed);
    dist = uniform_int_distribution<uint8_t>(1, 9);
    set_starting_positions(17);
}

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
                ::printw("\n");
            }

            if (i < 6) {
                ::printw("---|---|---\n");
            }
        }
    }
    else {  //for printing using either rows or columns
        for (uint8_t i = 0; i < 9; i++) {
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

            ::printw("\n");
            if (i == 2 or i == 5) {
                ::printw("---|---|---\n");
            }
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
        positions[i] = false;
    }
}

void Matrix_9x9::set_starting_positions (const uint8_t NUM_POSITIONS)
{
    for (uint8_t i = 0; i < NUM_POSITIONS; i++) {
        uint8_t pos,
                value,
                index_row,
                index_column,
                index_submatrix;
        bool valid_position = false,
             valid_value = false;
        while (!valid_position) {
            pos = next_position();      //get next position (random)
            if (!positions[pos]) {      //check if position is already filled
                valid_position = true;
                positions[pos] = true;
            }
        }
        
        const uint8_t ROW_NUMBER = map_row(pos),                                      //map position to row
                      COLUMN_NUMBER = map_column(pos),                                //map position to column
                      SUBMATRIX_NUMBER = map_submatrix(ROW_NUMBER, COLUMN_NUMBER);    //map position to 3x3 submatrix
                      
        //check the row, column, and submatrix for the value
        Row &row = get_row(ROW_NUMBER); //NOTE: why these require the ampersand, I'm not really sure
        Column &column = get_column(COLUMN_NUMBER);
        Matrix_3x3 &submatrix = get_submatrix(SUBMATRIX_NUMBER);
        while (!valid_value) {
            value = next_value();
            if (!row.value_exists(value) and !column.value_exists(value) and !submatrix.value_exists(value)) {
                valid_value = true;
            }
        }

        //Get indeces for particular row, column, and submatrix
        get_indeces(pos, index_row, index_column, index_submatrix);
        value += 48;    //convert in order to display proper character
        row.set_value(index_row, value);
        column.set_value(index_column, value);
        submatrix.set_value(index_submatrix, value);
        /*cout << "value: " << value << endl
             << "row[" << index_row+0 << "]: " << row[index_row] << endl;
        print(false, false);*/
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

uint8_t Matrix_9x9::next_position()
{
    return position_dist(position_generator);
}

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

uint8_t Matrix_9x9::next_value()
{
    return dist(generator);
}

void Matrix_9x9::get_indeces (const uint8_t POS, uint8_t& row, uint8_t& column, uint8_t& submatrix)
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
    row = POS % 9;

    //map column index
    column = POS / 9;

    //map submatrix index
    submatrix = 3 * (column % 3) + row % 3;
}

map<uint8_t, cell> Matrix_9x9::create_map()
{
    map<uint8_t, cell> m;

    for (uint8_t i = 0; i < 9; i++) {
        for (uint8_t j = 0; j < 9; j++) {
            m[i * 9 + j] = pair<uint8_t, uint8_t>(i, j);
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
