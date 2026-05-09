#include "./../Dice.hpp"

class DoubleDice : public Dice<6>
{

    DoubleDice() : Dice<6>("Double Dice", "2 6-sided Dice",     
                           {1, 2, 3, 4, 5, 6},ItemPrices::EXPENSIVE, 2)
    {}

};