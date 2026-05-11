#include "./../Items.hpp"

#include <unordered_map>

//TODO fare la logica del normal pipe
class NormalPipe: public Items
{

    std::unordered_map<int, int> coordinate;

    NormalPipe(): Items("Pipe", "Warps you to place in a radius within 4 blocks", ItemPrices::EXPENSIVE)
    {}
};