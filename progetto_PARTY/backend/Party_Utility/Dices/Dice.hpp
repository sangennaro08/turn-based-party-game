#pragma once

#include "./../Items.hpp"

#include <array>
#include <random>

template <std::size_t N, std::size_t Trow = 1>

class Dice : public Items
{
    public:

    const std::array<int, N> faces;

    Dice(std::string name,std::string desc,std::array<int, N> f, int price) 
        : Items(name, desc, price) ,faces(f)
        {}
    
    //TODO vedere come applicare la logica del roll the dice  
    
    //for RollDice algorithm
    int TrowDice()
    {

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, N - 1);

        return faces[dist(gen)];
    }
};