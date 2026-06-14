#pragma once

#include "./../Party_Utility/Items.hpp"
#include "./../Inventory/Inventory.hpp"

class Player
{

    public:

    const std::string name;
    const std::string id;

    bool MoreDice = false;
    bool UseSpecialDie = false;

    Inventory Inv;

    Player(std::string name, std::string id): 
    name(name),
    id(id),
    Inv(Inventory())
    {}

};