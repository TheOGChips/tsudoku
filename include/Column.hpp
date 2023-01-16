#ifndef COLUMN_HPP
#define COLUMN_HPP

#include "Container.hpp"
#include <cstdint>  //uint8_t

class Column : public Container {
    private:
        char col[9];

    public:
        Column();
        Column (uint8_t[9]);
        ~Column() {}
        uint8_t at (uint8_t) override;
        uint8_t operator [] (uint8_t) override;
        bool value_exists (uint8_t) override;
        void set_value (uint8_t, uint8_t) override;
        bool evaluate() override;
};

#endif
