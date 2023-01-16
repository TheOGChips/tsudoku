#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include "values.hpp"

class Container {
    public:
        virtual uint8_t at (uint8_t) = PURE_VIRTUAL;
        virtual uint8_t operator [] (uint8_t) = PURE_VIRTUAL;
        virtual bool value_exists (uint8_t) = PURE_VIRTUAL;
        virtual void set_value (uint8_t, uint8_t) = PURE_VIRTUAL;
        virtual bool evaluate() = PURE_VIRTUAL;
};

typedef Container House;

#endif
