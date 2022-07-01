#ifndef MATRIX_9X9_HPP
#define MATRIX_9X9_HPP

#include "values.hpp"
#include "Matrix_3x3.hpp"
#include "Row.hpp"
#include "Column.hpp"
#include <iostream>

using namespace std;

class Matrix_9x9 {
    private:
        Matrix_3x3 matrices[9];
                   /*TL, TM, TR,
                   L,  M,  R,
                   BL, BM, BR;*/
        Row rows[9];
            /*row1, row2, row3,
            row4, row5, row6,
            row7, row8, row9;*/
        Column cols[9];
               /*col1, col2, col3,
               col4, col5, col6,
               col7, col8, col9;*/

    public:
        Matrix_9x9();
        ~Matrix_9x9() {}
        Matrix_3x3 submatrix (uint8_t) const;
        Row row (uint8_t) const;
        Column column (uint8_t) const;
        void print() const;
        friend ostream& operator << (ostream&, Matrix_9x9);
};

#endif
