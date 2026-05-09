#pragma once

#include "./../../Party_Utility/Dices/Type_of_dices/Dice_0_1.hpp"
#include "./../../Party_Utility/Dices/Type_of_dices/Dice_4_5_6.hpp"
#include "./../../Party_Utility/Dices/Type_of_dices/Dice10F.hpp"
#include "./../../Party_Utility/Dices/Type_of_dices/DoubleDice.hpp"

#include "./../../bots/Bot.hpp"

//TODO finire questo file


//find best place based on current situation
//TODO fare il file durante la creazione del frontend e comunicazione
#include "./../FindBestPlace/BestPlace.hpp"

#include <array>
#include <random>
#include <unordered_map>

const std::unordered_map<std::string, int> DiceSteps
{
    {"beginner",   4},
    {"easy",       4},
    {"normal",     3},
    {"hard",       3},
    {"extreme",    2},
    {"impossible", 1}
};

template <std::size_t N>

inline void RollDice(Bot bot, Dice<N>& die)
{

    constexpr int MAX_LENGTH = 100;

    int Rolls[MAX_LENGTH];

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution <int> values(0, N - 1);
    
    //TODO chiamare funzione per trovare il posto migliore ritorna un numero
    //find best place
    //function for best place.hpp
    int favourable_place = 10;

    int Rigged_Rolls = bot.percentage;
    int pendence = DiceSteps.at(bot.difficulty);

    if(die.trows > 1 || bot.MoreDice)
    {   
        
        //-1
        for(int trow = 0; trow < Rigged_Rolls - 1; trow++)
        {

            std::array<int, 2> numbers = die.Dice2Trow();
            int sum = numbers[0] + numbers[1];

            if(sum > favourable_place - pendence && 
            sum < favourable_place + pendence)
            {
                Rolls[trow] = numbers[0];
                Rolls[trow + 1] = numbers[1];
            }else{
                trow--;
            }
        }

        if(bot.MoreDice)
            bot.MoreDice = !bot.MoreDice;

    }else{
        
        //do single dice format

        for(int trow = 0; trow < Rigged_Rolls; trow++)
        {
            int number = die.Dice1Trow();

            if( number > favourable_place - pendence && 
                number < favourable_place + pendence)
            {
                Rolls[trow] = number;    
            }else{
                trow--;
            }
        }
    }

    for(int trows = MAX_LENGTH - Rigged_Rolls; trows < MAX_LENGTH; trows++)
        Rolls[trows] = die.faces[values(generator)];

    //TODO fare il lancio dei risultati quando si farà la transizione con python    
}