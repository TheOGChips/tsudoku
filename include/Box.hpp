#ifndef BOX_HPP
#define BOX_HPP

#include "Container.hpp"    //uint8_t
#include <map>      //std::map
#include <utility>  //std::pair

using namespace std;

typedef pair<uint8_t, uint8_t> cell;

class Box : public Container {
    public:
        Box ();
        Box (uint8_t[9]);
        ~Box () {}
};

#endif
