#include "./../Dice.hpp"

class Dice_0_1 : public Dice
{

    public:

    int number_faces [6] = {0,0,0,1,1,1};

    Dice_0_1()
    {
        this->faces = 6;
        this->prize = 10;
    }
};