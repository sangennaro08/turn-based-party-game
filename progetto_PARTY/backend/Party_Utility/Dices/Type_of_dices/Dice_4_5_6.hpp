#include "./../Dice.hpp"

class Dice_4_5_6 : public Dice<6>
{

    public:

    Dice_4_5_6() : Dice<6>("4UP DICE", "A 6-sided Die that goes from 4 to 6",
                          {4, 4, 5, 5, 6, 6}, ItemPrices::MODERATE)
    {}

};