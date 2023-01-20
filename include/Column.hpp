#ifndef COLUMN_HPP
#define COLUMN_HPP

#include "Container.hpp"    //uint8_t

class Column : public Container {
    public:
        Column();
        Column (uint8_t[9]);
        ~Column() {}
};

#endif
