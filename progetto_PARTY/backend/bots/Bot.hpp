#pragma once 

#include "./../Party_Utility/Items.hpp"

/*

Creating the instances of the bots that will play in the in the game

*/
#include <iostream>
#include <unordered_map>

#include <thread>
#include <memory>

const std::unordered_map<std::string, int> difficultiness
{
    {"beginner", 10},
    {"easy", 30},
    {"normal", 50},
    {"hard", 70},
    {"extreme", 80},
    {"impossible", 90}
};

class Bot
{
    public:

    const std::string name;
    const std::string difficulty;
    const short int percentage;

    unsigned int silverCoins = 0;
    unsigned int goldCoins = 0;
     
    //TODO implementare logica dell'array di oggetti di elementi da poter utilizzare
    std::unique_ptr<Items> Utilites [3]; 

    std::thread th;

    Bot(std::string name, std::string difficulty);
    ~Bot();
};