#pragma once

#include "./../Items.hpp"

#include <array>
#include <random>

template <std::size_t N>

class Dice : public Items
{
    public:

    const std::array<int, N> faces;

    int trows;

    Dice(std::string name,std::string desc,std::array<int, N> f, int price, int trows = 1) 
        : Items(name, desc, price) ,faces(f), trows(trows)
        {}
    
    //TODO vedere come applicare la logica del roll the dice  
    
    //for RollDice algorithm
    int Dice1Trow()
    {

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, N - 1);

        return faces[dist(gen)];
    }

    std::array<int, 2> Dice2Trow()
    {
        std::array<int, 2> numbers = {};

        std::random_device rd;
        std::mt19937 generator(rd());

        std::uniform_int_distribution <int> dist(0, faces.size() - 1);

        for(int trows = 0; trows < 2; trows++)
            numbers[trows] = dist(generator);

        return numbers;
    }
};