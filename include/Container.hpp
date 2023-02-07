#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include "values.hpp"

class Container {
    private:
        char arr[CONTAINER_SIZE];
        
    public:
        Container ();
        Container (uint8_t[CONTAINER_SIZE]);
        ~Container () {}
        
        uint8_t at (uint8_t);
        uint8_t operator [] (uint8_t);
        bool value_exists (uint8_t);
        void set_value (uint8_t, uint8_t);
        bool evaluate();
};

typedef Container House;
typedef Container Box;
typedef Container Column;
typedef Container Row;

#endif
