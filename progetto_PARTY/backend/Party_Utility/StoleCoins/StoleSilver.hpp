#pragma once

#include "./../Items.hpp"

#include <iostream>
#include <random>


class StoleSilver : public Items
{

    public:

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<int> Stolable;

    //int Stolen;

    StoleSilver(): Items("Silver Stealer", "If applied against someone you will rob part of their silver coins",
                         ItemPrices::EXPENSIVE),
    gen(rd()),
    Stolable(1, 50)
    {}

    //TODO implementare logica,vedere come fare e se funziona
    int StealSilver(){return Stolable(gen);}
};