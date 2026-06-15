#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <variant>

class Items
{
    public:

    const std::string name;
    const std::string description;

    const int price;

    Items(std::string name, std::string desc, int price):
    name(name),
    description(desc),
    price(price)
    {}
    
    //TODO implementare logica,vedere come fare
    //questa funzione la usano:
    //Goback
    //StoleSilver
    //Goahead
    virtual int UseItem(){return 0;}

    /*void UseItem();

    //TODO vedere come fare, può essere che l'array è pieno o si può aggiungere,posso ritornare come no
    void AddItem(Bot& bot, const Items& Item);

    void RemoveItem(Bot& bot, const Items& Item)
    {
        for(auto& Utility : bot.Utilities)
        {
            if(Utility.get() == &Item)
            {
                Utility = nullptr;
                break;
            }
        }
    }*/

    virtual ~Items() = default;
    //virtual ~Items(){}

    enum ItemPrices : int
    {
        cheap     = 10,
        budget    = 15,
        moderate  = 20,
        expensive = 30,
        premium   = 40,
        deluxe    = 50
    };
};
