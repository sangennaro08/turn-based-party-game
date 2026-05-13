#pragma once

#include "./../Party_Utility/Items.hpp"

#include <iostream>

#include <array>
#include <memory>

class Inventory
{
    public:

    int silverCoins = 0;
    int goldCoins = 0;
    
    //TODO implementare logica dell'array di oggetti di elementi da poter utilizzare
    std::array <std::unique_ptr<Items>, 3> Utilities;
    int countUtilities = 0;

    Inventory(){}

    //TODO implementare logica,vedere come fare
    void UseItem();

    //TODO vedere come fare, può essere che l'array è pieno o si può aggiungere,posso ritornare come no
    void AddItem(const Items& Item);

    void RemoveItem(const Items& Item)
    {
        for(auto& Utility : Utilities)
        {
            if(Utility.get() == &Item)
            {
                Utility = nullptr;
                countUtilities--;
                break;
            }
        }
    }
};