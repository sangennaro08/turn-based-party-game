
#include "./../../bots/Bot.hpp"

#include <random>

//it Rolls a third die if you get 2 equals numbers form the dice before
inline int TripleDie()
{
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution <int> number(1, 6); 

    return number(generator);
}