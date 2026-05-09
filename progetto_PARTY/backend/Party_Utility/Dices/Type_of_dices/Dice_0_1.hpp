#include "./../Dice.hpp"

class Dice_0_1 : public Dice<6>
{

    public:

    Dice_0_1() : Dice<6>("0 DICE", "A 6-sided Die with only ones and zeros",
                        {0, 0, 0, 1, 1, 1}, ItemPrices::CHEAP)
    {}
};