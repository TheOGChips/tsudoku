#ifndef BOX_HPP
#define BOX_HPP

#include "values.hpp"
#include <map>      //std::map
#include <utility>  //std::pair
#include <cstdint>  //uint8_t
#include <array>

using namespace std;

typedef pair<uint8_t, uint8_t> cell;

class Box {
    private:
        char mat[3][3];
        map<uint8_t, cell> _map_;
        map<uint8_t, cell> create_map();

    public:
        Box ();
        Box (uint8_t[9]);
        ~Box () {}
        uint8_t at (uint8_t);               //TODO: Virtual function candidate
        uint8_t operator [] (uint8_t);      //TODO: Virtual function candidate
        void print_map();
        bool value_exists (uint8_t);        //TODO: Virtual function candidate
        void set_value (uint8_t, uint8_t);  //TODO: Virtual function candidate
        bool evaluate();                    //TODO: Virtual function candidate
};

#endif
