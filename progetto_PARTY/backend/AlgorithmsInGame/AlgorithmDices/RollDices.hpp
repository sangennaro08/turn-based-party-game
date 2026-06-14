#pragma once

#include "./../../Party_Utility/Dices/Dice.hpp"
#include "./../../Party_Utility/Dices/Type_of_dices/Dice_0_1.hpp"
#include "./../../Party_Utility/Dices/Type_of_dices/Dice_4_5_6.hpp"
#include "./../../Party_Utility/Dices/Type_of_dices/Dice10F.hpp"
#include "./../../Party_Utility/Dices/Type_of_dices/DoubleDice.hpp"
#include "./../../Party_Utility/Dices/Type_of_dices/TripleDice.hpp"

#include "./../../bots/Bot.hpp"
#include "./../../Inventory/Inventory.hpp"
#include "./../../Party_Utility/Items.hpp"

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

template <int N, int T>

//TODO gestire in FE quale dado usare se non usa speciali è quello di default
//TODO il dado normale sarà fatto su FE  e favuorable place verrà passato alla funzione
//e calcolato nella pagina web
inline auto RollDice(Bot& bot, Dice<N, T>& die)
{

    constexpr int MAX_LENGTH = 100;
    int Rolls[MAX_LENGTH];

    //used to get thèe best numbers and as a return of the 3 rolled numbers
    std::array <int, T> numbers;

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

    for(int trow = 0; trow < Rigged_Rolls; trow += T)
    {
        int sum = 0;

        for(int i = 0; i < numbers.size(); i++)
        {
            numbers.at(i) = die.UseItem();
            sum += numbers.at(i);
        }
         
        if(sum > favourable_place - pendence &&
           sum < favourable_place + pendence)
        {
            for(int i = trow; i < (trow + T) && i < MAX_LENGTH; i++)
                Rolls[i] = numbers.at(i - trow);
        }else{
            trow -= T;
        }
    }

    //insert normal values after rigged ones
    for(int trows = Rigged_Rolls; trows < MAX_LENGTH; trows++)
        Rolls[trows] = die.faces[values(generator)];

    //get rigged numbers
    for(int rolls = 0; rolls < T; rolls++)
        numbers.at(rolls) = Rolls[get_number(generator)];

    //NB: you use special dice if you have it or you didn't win a minigame(wi will implement just Blackjack )
    if(bot.UseSpecialDie)
    {
        //die.RemoveItem(bot, die);
        bot.Inv.RemoveItem(die);
        bot.UseSpecialDie = !bot.UseSpecialDie;
    }

    return numbers;  
}