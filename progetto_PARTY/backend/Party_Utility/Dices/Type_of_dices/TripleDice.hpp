#pragma once

#include "./../Dice.hpp"

class TripleDice: public Dice<6, 3>
{

    public:

    TripleDice(): Dice("Triple Dice", "You can trow 3 dice in 1 turn!!!", 
                       {1, 2, 3, 4, 5, 6}, ItemPrices::EXPENSIVE)
    {}                   
};