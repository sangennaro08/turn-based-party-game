#pragma once

#include "./../Party_Utility/Items.hpp"
#include "./../Party_Utility/Dices/Dice.hpp"

#include <iostream>

#include <array>
#include <memory>
#include <variant>

class Inventory
{
    public:

    int silverCoins = 0;
    int goldCoins = 0;
    
    //TODO implementare logica dell'array di oggetti di elementi da poter utilizzare
    std::array <std::unique_ptr<Items>, 3> Utilities;

    Inventory(){}

    virtual ~Inventory(){}

    //TODO implementare logica,vedere come fare, farla virtuale così ogni oggetto ha il so comportamento

    //                          vedere se usare una map per le coordinate della mappa
    int UseItem(std::unique_ptr<Items>& Item)
    {
        if(Item != nullptr)
            return Item->UseItem();

        return -1;
    }

    //TODO vedere come vedere se l'aggiunta dell o elima dell oggetto è andata con successo

    //TODO vedere come fare, può essere che l'array è pieno o si può aggiungere,posso ritornare come no
    int AddItem(std::unique_ptr<Items>& Item)
    {
        for(auto& Utility : Utilities)
        {
            if(Utility == nullptr)
            {
                Utility = std::move(Item);
                return 0;
            }
        }

        return -1;
    }

    int RemoveItem(const Items& Item)
    {
        for(auto& Utility : Utilities)
        {
            if(Utility.get() == &Item)
            {
                Utility = nullptr;
                return 0;
            }
        }

        return -1;
    }
};