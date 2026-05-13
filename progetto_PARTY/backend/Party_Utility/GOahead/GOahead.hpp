#pragma once

#include "./../Items.hpp"

class GOahead: public Items
{
    public:

    int Boost = 5;

    GOahead(): Items("Sprint boost", "IF used you will have a 5 block boost when used a die", ItemPrices::MODERATE)
    {}
};