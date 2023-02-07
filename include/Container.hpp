#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include "misc.hpp"

/* NOTE:
 * Class: Container
 * Purpose: Represents a Sudoku container (i.e. a 3x3 box, a column, or a row).
 * Private variables:
 *      arr -> Internal array housing the container's contents.
 */
class Container {
    private:
        char arr[CONTAINER_SIZE];
        
    public:
        Container ();
        Container (const uint8_t[CONTAINER_SIZE]);
        ~Container () {}    //NOTE: Class Destructor: Does nothing.
        
        uint8_t at (const uint8_t) const;
        uint8_t operator [] (const uint8_t);
        bool value_exists (uint8_t) const;
        void set_value (const uint8_t, const uint8_t);
        bool evaluate () const;
};

typedef Container House;    //NOTE: These typedefs are provided because it's easier to think about
typedef Container Box;      //      using rows, columns, or boxes rather than just general 
typedef Container Column;   //      containers, even though all three will behave the same. A house
typedef Container Row;      //      is just an alternate name for a container, and is really just
                            //      provided for posterity.
#endif
