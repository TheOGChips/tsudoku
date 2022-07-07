#include "Matrix_9x9.hpp"
#include <cstdlib>
#include <ctime>

using namespace std;

Matrix_9x9::Matrix_9x9 ()
{
    init_positions();
    time_t seed = time(nullptr);
    position_generator = mt19937(seed);
    position_dist = uniform_int_distribution<uint8_t>(0, 80);
    generator = mt19937(seed);
    dist = uniform_int_distribution<uint8_t>(0, 8);
}

ostream& operator << (ostream& os, const Matrix_9x9 mat)
{
    mat.print();
    return os;
}

void Matrix_9x9::print () const
{
    const bool SUBMATRIX_PRINTING = false,
               ROW_PRINTING = true,
               COLUMN_PRINTING = false;
    /*
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
                    for (uint8_t k = 0; k < 3; k++) {
                        cout << this->submatrix(j)[k + offset];
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
                    cout << column(j)[i];
                }
                else {  //if (ROW_PRINTING)
                    cout << row(i)[j];
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

Matrix_3x3 Matrix_9x9::submatrix (uint8_t index) const
{
    return matrices[index];
}

Row Matrix_9x9::row (uint8_t index) const
{
    return rows[index];
}

Column Matrix_9x9::column (uint8_t index) const
{
    return cols[index];
}

void Matrix_9x9::init_positions()
{
    for (uint8_t i = 0; i < 81; i++) {
        positions[i] = false;
    }
}

uint8_t Matrix_9x9::next()
{
    return dist(generator);
}
