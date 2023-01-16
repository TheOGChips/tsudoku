#ifndef BOX_HPP
#define BOX_HPP

#include "Container.hpp"
#include <map>      //std::map
#include <utility>  //std::pair
#include <cstdint>  //uint8_t

using namespace std;

typedef pair<uint8_t, uint8_t> cell;

class Box : public Container {
    private:
        char mat[3][3];
        map<uint8_t, cell> _map_;
        map<uint8_t, cell> create_map();

    public:
        Box ();
        Box (uint8_t[9]);
        ~Box () {}
        uint8_t at (uint8_t) override;
        uint8_t operator [] (uint8_t) override;
        void print_map();
        bool value_exists (uint8_t) override;
        void set_value (uint8_t, uint8_t) override;
        bool evaluate() override;
};

#endif
