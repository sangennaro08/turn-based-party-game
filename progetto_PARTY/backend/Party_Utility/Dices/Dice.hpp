#pragma once

#include "./../Items.hpp"

#include <array>
#include <random>

template <int N, int Trow = 1>

class Dice : public Items
{
    public:

    const std::array<int, N> faces;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<int> dist;


    Dice(std::string name,std::string desc,std::array<int, N> f, int price) 
        : Items(name, desc, price) ,faces(f),
        gen(rd()),
        dist(0, N - 1)
        {}
    
    //TODO vedere come applicare la logica del roll the dice  

    int TrowDice()
    {
        return faces[dist(gen)];
    }

    //for RollDice algorithm
    int UseItem() override
    {
        return TrowDice();
    }
};