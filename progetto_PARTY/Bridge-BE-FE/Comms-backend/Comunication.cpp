#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

// librerie standard per l'invio e ricevimento dei file JSON
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <nlohmann/json.hpp>
#include <array> 

//librerie per interfacciare le funzioni C++ al server python
#include "./../../backend/Player/Player.hpp"
#include "./../../backend/bots/Bot.hpp"

#include "./../../backend/Party_Utility/Dices/Dice.hpp"
#include "./../../backend/Party_Utility/Dices/Type_of_dices/Dice_0_1.hpp"
#include "./../../backend/Party_Utility/Dices/Type_of_dices/Dice_4_5_6.hpp"
#include "./../../backend/Party_Utility/Dices/Type_of_dices/Dice10F.hpp"
#include "./../../backend/Party_Utility/Dices/Type_of_dices/DoubleDice.hpp"
#include "./../../backend/Party_Utility/Dices/Type_of_dices/TripleDice.hpp"

#include "./../../backend/Party_Utility/GOahead/GOahead.hpp"
#include "./../../backend/Party_Utility/ReturnBack/GoBack.hpp"
#include "./../../backend/Party_Utility/StoleCoins/StoleSilver.hpp"
#include "./../../backend/Party_Utility/Warp_Pipe/NormalPipe.hpp"
#include "./../../backend/Party_Utility/Warp_Pipe/GoldPipe.hpp"

#include "./../../backend/Inventory/Inventory.hpp"

using json = nlohmann::json;

PYBIND11_MODULE(Comunication, m)
{

    pybind11::enum_<Items::ItemPrices>(m, "ItemPrices")
        .value("cheap", Items::cheap)
        .value("budget", Items::budget)
        .value("moderate", Items::moderate)
        .value("expensive", Items::expensive)
        .value("premium", Items::premium)
        .value("deluxe", Items::deluxe);
        //.export_values();cosa fa? devo scirvere in maniera diversa per entrare nei valori dell'enum su python  

    pybind11::class_<Player>(m, "Player")
        .def(pybind11::init<std::string, std::string>())
        .def_readonly("name", &Player::name)
        .def_readonly("id", &Player::id)
        .def_readwrite("Inv", &Player::Inv)
        .def_readwrite("MoreDice", &Player::MoreDice)
        .def_readwrite("UseSpecialDie", &Player::UseSpecialDie);

    pybind11::class_<Bot>(m, "Bot")
        .def(pybind11::init<std::string, std::string, std::string>())
        .def_readonly("name", &Bot::name)
        .def_readonly("id", &Bot::id)
        .def_readonly("difficulty", &Bot::difficulty)
        .def_readonly("percentage", &Bot::percentage)
        .def_readwrite("MoreDice", &Bot::MoreDice)
        .def_readwrite("UseSpecialDie", &Bot::UseSpecialDie)
        .def_readwrite("Inv", &Bot::Inv)
        .def_readonly_static("difficultiness", &Bot::difficultiness);
        //.def_readwrite("th", &Bot::th)

    pybind11::class_<Inventory>(m, "Inventory")
        .def(pybind11::init<>())  
        .def_readwrite("silverCoins", &Inventory::silverCoins)
        .def_readwrite("goldCoins", &Inventory::goldCoins)
        .def_readwrite("Utilities", &Inventory::Utilities)
        .def("UseItem", &Inventory::UseItem, pybind11::arg("Item") = nullptr)
        .def("AddItem", &Inventory::AddItem, pybind11::arg("Item") = nullptr)
        .def("RemoveItem", &Inventory::RemoveItem, pybind11::arg("Item") = nullptr); 
    
    pybind11::class_<Items, std::shared_ptr<Items>>(m, "Items")
        .def(pybind11::init<std::string, std::string, int>())  
        .def("UseItem", &Items::UseItem)
        .def_readonly("name", &Items::name)
        .def_readonly("description", &Items::description)
        .def_readonly("price", &Items::price);

    //TODO vedere se dover aggiungere il random device e altri per i numeri randomici
    //=======================================================//
    //==========================DICES========================//
    //=======================================================//
    pybind11::class_<Dice<6>, Items, std::shared_ptr<Dice<6>>>(m, "Dice6")
        .def(pybind11::init<std::string, std::string, std::array<int, 6>, int>())
        .def("UseItem", &Dice<6>::UseItem)
        .def_readonly("faces", &Dice<6>::faces);

    pybind11::class_<Dice<6, 2>, Items, std::shared_ptr<Dice<6, 2>>>(m, "Dice6_2")
        .def(pybind11::init<std::string, std::string, std::array<int, 6>, int>())
        .def("UseItem", &Dice<6, 2>::UseItem)
        .def_readonly("faces", &Dice<6, 2>::faces);
        
    pybind11::class_<Dice<6, 3>, Items, std::shared_ptr<Dice<6, 3>>>(m, "Dice6_3")
        .def(pybind11::init<std::string, std::string, std::array<int, 6>, int>())
        .def("UseItem", &Dice<6, 3>::UseItem)
        .def_readonly("faces", &Dice<6, 3>::faces);    


    pybind11::class_<Dice<10>, Items, std::shared_ptr<Dice<10>>>(m, "Dice10")
        .def(pybind11::init<std::string, std::string, std::array<int, 10>, int>())
        .def("UseItem", &Dice<10>::UseItem)
        .def_readonly("faces", &Dice<10>::faces); 
        
    pybind11::class_<Dice_0_1, Dice<6>, std::shared_ptr<Dice_0_1>>(m , "Dice_0_1")  
        .def(pybind11::init<>());
        
    pybind11::class_<Dice_4_5_6, Dice<6>, std::shared_ptr<Dice_4_5_6>>(m , "Dice_4_5_6")  
        .def(pybind11::init<>());
        
    pybind11::class_<Dice10F, Dice<10>, std::shared_ptr<Dice10F>>(m , "Dice10F")  
        .def(pybind11::init<>());

    pybind11::class_<DoubleDice, Dice<6, 2>, std::shared_ptr<DoubleDice>>(m , "DoubleDice")  
        .def(pybind11::init<>());
        
    pybind11::class_<TripleDice, Dice<6, 3>, std::shared_ptr<TripleDice>>(m , "TripleDice")  
        .def(pybind11::init<>());  
        
    //=================================================//
    //==================OTHER UTILITIES================//
    //=================================================//
    pybind11::class_<GOahead, Items, std::shared_ptr<GOahead>>(m, "GOahead")
        .def(pybind11::init<>());
        
    pybind11::class_<GoBack, Items, std::shared_ptr<GoBack>>(m, "GOBack")
        .def(pybind11::init<>());

    pybind11::class_<StoleSilver, Items, std::shared_ptr<StoleSilver>>(m, "StoleSilver")
        .def(pybind11::init<>());    
    
        
    //TODO vedere per queste due classi    
    pybind11::class_<NormalPipe, Items, std::shared_ptr<NormalPipe>>(m, "NormalPipe")
        .def(pybind11::init<>());     

    pybind11::class_<GoldPipe, Items, std::shared_ptr<GoldPipe>>(m, "GoldPipe")
        .def(pybind11::init<>());

    //TODO fare altre funzioni che permettono di far ricereve a python le info come file json senza che lui debba farle    
       
}
