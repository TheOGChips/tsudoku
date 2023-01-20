#ifndef ROW_HPP
#define ROW_HPP

#include "Container.hpp"    //uint8_t

class Row : public Container {
    public:
        Row();
        Row (uint8_t[9]);
        ~Row() {}
};

#endif
