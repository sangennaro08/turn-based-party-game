#pragma once

#include <iostream>

class Items
{
    public:

    std::string name;
    std::string description;

    enum ItemPrices : unsigned int
    {

        CHEAP     = 10,
        BUDGET    = 15,
        MODERATE  = 20,
        EXPENSIVE = 30,
        PREMIUM   = 40,
        DELUXE    = 50
        
    };

    unsigned int price;

    Items(){}

    Items(std::string name, std::string desc, int price):
    name(name),
    description(desc),
    price(price)
    {}

    void UseItem();

    Items AddItem();
    void RemoveItem();

    //same thing
    virtual ~Items() = default;
    //virtual ~Items(){}
};

