#pragma once

#include "./../Party_Utility/Items.hpp"
#include "./../Inventory/Inventory.hpp"

class Player
{

    public:

    const std::string name;

    Inventory Inv;

    Player(std::string name): name(name), Inv(Inventory())
    {}

};
