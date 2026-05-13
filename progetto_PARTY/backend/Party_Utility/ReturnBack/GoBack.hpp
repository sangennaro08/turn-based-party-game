#pragma once

#include "./../Items.hpp"

#include <random>


//TODO fare logica del go back
class GoBack: public Items
{
    public:

    int Blocks;

    GoBack(): Items("Go where you belong!", 
                    "IF applied to someone, he will go back an undeterminate times of blocks",
                    ItemPrices::BUDGET)
    {}

    ~GoBack(){}
};
   
