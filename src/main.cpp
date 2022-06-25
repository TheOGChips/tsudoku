//#include "../include/Matrix_9x9.hpp"
#include "../include/sudoku.hpp"

int main ()
{
    //IDEA: Each of these could be their own data structure,
    //      The 9x9 matrix would consist of 9 of each of these data structures
    //Inside the matrix (9x9)
        //Every row must contain the numbers 1-9 with no repeats
        //Every column must contain the numbers 1-9 with no repeats
        //Every sub-matrix (3x3) must contain the numbers 1-9 with no repeats
    
    Matrix_9x9 puzzle = sudoku_init();  //initialize a sudoku puzzle
    return 0;
}
