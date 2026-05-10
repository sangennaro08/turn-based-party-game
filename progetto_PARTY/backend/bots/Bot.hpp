#pragma once 

#include "./../Party_Utility/Items.hpp"

/*

Creating the instances of the bots that will play in the in the game

*/
#include <iostream>

#include <unordered_map>
#include <thread>
#include <memory>

class Bot
{
    public:

    const std::unordered_map<std::string, int> difficultiness
    {
        {"beginner",   10},
        {"easy",       30},
        {"normal",     50},
        {"hard",       70},
        {"extreme",    80},
        {"impossible", 90}
    };

    const std::string name;
    const std::string difficulty;
    const int percentage;

    //true if you get a second dice if you win a minigame
    bool MoreDice = false;
    bool UseSpecialDie = false;

    int silverCoins = 0;
    int goldCoins = 0;
     
    //TODO implementare logica dell'array di oggetti di elementi da poter utilizzare
    std::unique_ptr<Items> Utilities [3];
    int countUtilities = 0;

    std::thread th;

    Bot(std::string name, std::string difficulty);
    
    ~Bot();
};