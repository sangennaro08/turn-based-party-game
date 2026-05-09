#include "./../Dice.hpp"

class Dice10F : public Dice<10>
{
    
    public:

    Dice10F() : Dice<10>("10 FACES DICE","A 10-sided Die that goes from 1 to 10",
                        {1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10}, ItemPrices::PREMIUM)
    {}
};