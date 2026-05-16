#pragma once

#include "./../Items.hpp"

#include <variant>

//TODO fare logica del goldpipe
class GoldPipe : public Items
{
    public:

    std::variant<int, std::string> coordinates[13][13];

    GoldPipe(): Items("Gold Pipe", "Warps you to a place of your choice", ItemPrices::DELUXE)
    {}
};