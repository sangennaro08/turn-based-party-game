#pragma once

#include "./../Items.hpp"
#include <array>

template <std::size_t N>

class Dice : public Items
{
    public:

    std::array<unsigned int, N> faces;

    int trows;

    Dice(std::string name,std::string desc,std::array<unsigned int, N> f, unsigned int price, int trows = 1) 
        : Items(name, desc, price) ,faces(f), trows(trows)
        {}  
};