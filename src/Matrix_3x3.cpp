#include "Matrix_3x3.hpp"
#include <cstdint>

Matrix_3x3::Matrix_3x3 ()
{
    for (uint8_t i = 0; i < 3; i++) {
        for (uint8_t j = 0; j < 3; j++) {
            mat[i][j] = '-';
        }
    }
}
