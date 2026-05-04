#include "./../Dice.hpp"

class Dice_7_8_9 : public Dice
{

    public:

    int number_faces [6] = {7, 7, 8, 8, 9, 9};

    Dice_7_8_9()
    {
        this->faces = 6;
        this->prize = 30;
    }

};