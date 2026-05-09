#pragma once

#include <iostream>

class Items
{
    public:

    const std::string name;
    const std::string description;

    const int price;

    Items();

    Items(std::string name, std::string desc, int price):
    name(name),
    description(desc),
    price(price)
    {}

    //TODO implementare logica,vedere come fare
    void UseItem();

    Items AddItem();
    void RemoveItem();

    protected:

    //why an enum and not an enum class?
    const enum ItemPrices : int
    {
        CHEAP     = 10,
        BUDGET    = 15,
        MODERATE  = 20,
        EXPENSIVE = 30,
        PREMIUM   = 40,
        DELUXE    = 50
        
    };
    //same thing
    virtual ~Items() = default;
    //virtual ~Items(){}
};

