#include "./../Dice.hpp"

class Dice10F : public Dice
{
    
    public:

    int number_faces [10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    Dice10F()
    {
        this->faces = 10;
        this->prize = 50;
    }
};