#pragma once

#include "./../../Party_Utility/Dices/Type_of_dices/Dice_0_1.hpp"
#include "./../../Party_Utility/Dices/Type_of_dices/Dice_4_5_6.hpp"
#include "./../../Party_Utility/Dices/Type_of_dices/Dice10F.hpp"
#include "./../../Party_Utility/Dices/Type_of_dices/DoubleDice.hpp"

#include "./../../bots/Bot.hpp"

//TODO finire questo file


//TODO vedere come togliere oggetto se usato, si intende su JS a livello grafico e logico sincronizzarli.

#include "./../FindBestPlace/BestPlace.hpp"

#include <array>
#include <random>
#include <unordered_map>

const std::unordered_map<std::string, int> DiceSteps
{
    {"beginner",   4},
    {"easy",       3},
    {"normal",     3},
    {"hard",       2},
    {"extreme",    2},
    {"impossible", 1}
};


template <std::size_t N>

inline void RemoveUtility(Bot& bot, const Dice<N>& die)
{
    if(bot.UseSpecialDie)
    {
        bot.UseSpecialDie = !bot.UseSpecialDie;
        bot.countUtilities--;

        for(auto& Utilities : bot.Utilities)
        {
            if(Utilities.get() == &die)
            {
                Utilities = nullptr;
                break;
            } 
        }
    } 
}

template <std::size_t N>

inline std::array<int, 2> RollDice(Bot& bot, Dice<N>& die)
{

    constexpr int MAX_LENGTH = 100;
    int Rolls[MAX_LENGTH];

    std::random_device rd;
    std::mt19937 generator(rd());

    std::uniform_int_distribution <int> values (0, N - 1);
    std::uniform_int_distribution <int> get_number (0, MAX_LENGTH - 1);
    
    //TODO fare funzione BestPlace, da fare in base a come il programma JS è composto
    //find best place
    //function for best place.hpp
    int favourable_place = BestPlace();

    int Rigged_Rolls = bot.percentage;
    int pendence = DiceSteps.at(bot.difficulty);

    //TODO se hai doppio dado dopo aver vinto un minigioco non puoi usare UseSpecialDie
    if(bot.MoreDice || ( bot.UseSpecialDie && die.trows > 1 ))
    {   
        //-1
        for(int trow = 0; trow < (Rigged_Rolls - 1); trow++)
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

        for(int trows = MAX_LENGTH - Rigged_Rolls; trows < MAX_LENGTH; trows++)
            Rolls[trows] = die.faces[values(generator)]; 
            
        if(bot.MoreDice)
            bot.MoreDice = !bot.MoreDice;
        
        //TODO vedere se fare un altro file per ordinare il tutto e rendere tutto il più sincronizzato con il front-end
        RemoveUtility(bot, die);

        return { Rolls[get_number(generator)] , Rolls[get_number(generator)]};

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

        RemoveUtility(bot, die);

        for(int trows = MAX_LENGTH - Rigged_Rolls; trows < MAX_LENGTH; trows++)
            Rolls[trows] = die.faces[values(generator)];
   
    }

    return { Rolls[get_number(generator)], -1};
    //TODO fare il lancio dei risultati quando si farà la transizione con python    
}