#include <iostream>
#include "./../Items.hpp"

//TODO rendere più astratto il concetto...
//fare in modo tale di aggiungere più attributi utili e 
//fare un array in modo decente e non hard coded

class Dice : public Items
{
    public:

    int faces;
    int prize;
};