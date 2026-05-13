#pragma once

#include "./../Dice.hpp"

class DoubleDice : public Dice<6, 2>
{

    public:

    DoubleDice() : Dice<6, 2>("Double Dice", "2 6-sided Dice",     
                              {1, 2, 3, 4, 5, 6},ItemPrices::MODERATE)
    {}
     
};