#pragma once

#include "./../Items.hpp"

#include <random>


//TODO fare logica del go back
class GoBack: public Items
{
    public:

    int Blocks;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<int> dist;

    GoBack(): Items("Go where you belong!", 
                    "IF applied to someone, he will go back an undeterminate times of blocks",
                    ItemPrices::BUDGET),
    gen(rd()),
    dist(1, 5)                
    {}

    ~GoBack(){}

    std::variant<std::string, int> UseItem() override {return dist(gen);}
};
   
