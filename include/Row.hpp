#ifndef ROW_HPP
#define ROW_HPP

#include "Container.hpp"
#include <cstdint>  //uint8_t

class Row : public Container {
    private:
        char row[9];

    public:
        Row();
        Row (uint8_t[9]);
        ~Row() {}
        uint8_t at (uint8_t) override;
        uint8_t operator [] (uint8_t) override;
        bool value_exists (uint8_t) override;
        void set_value (uint8_t, uint8_t) override;
        bool evaluate() override;
};

#endif
