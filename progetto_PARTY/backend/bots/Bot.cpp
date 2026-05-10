
/*

Creating the instances of the bots that will play in the in the game

*/
#include "./Bot.hpp"
#include "./../Party_Utility/Items.hpp"

Bot::Bot(std::string name, std::string difficulty):
    name(std::move(name)),
    difficulty(std::move(difficulty)),
    percentage(difficultiness.at(this->difficulty))
    {}

Bot::~Bot(){}