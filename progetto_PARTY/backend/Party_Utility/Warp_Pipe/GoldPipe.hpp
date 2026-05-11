#include "./../Items.hpp"

#include <unordered_map>

//TODO fare logica del goldpipe
class GoldPipe : public Items
{
    public:

    std::unordered_map <int, int> coordinate;

    GoldPipe(): Items("Gold Pipe", "Warps you to a place of your choice", ItemPrices::DELUXE)
    {}
};