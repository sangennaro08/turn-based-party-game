/*
 * cpp_bridge.cpp
 * Game Core Bridge - Espone le funzioni di gioco C++ al Python server
 * 
 * Architettura:
 * C++ (Game Logic) → JSON via TCP/HTTP → Python (Bridge) → WebSocket → Frontend
 * 
 * Usa le VERE CLASSI del progetto:
 * - Player (name, Inv)
 * - Bot (name, difficulty, percentage, MoreDice, UseSpecialDie, Inv)
 * - Dice<N> template per tutti i dadi con TrowDice()
 * - GOahead, GoBack, StoleSilver (con UseItem())
 * - NormalPipe, GoldPipe
 */

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

// Includi le tue classi originali
#include "./../backend/Player/Player.hpp"
#include "./../backend/bots/Bot.hpp"
#include "./../backend/Party_Utility/Dices/Dice.hpp"
#include "./../backend/Party_Utility/GOahead/GOahead.hpp"
#include "./../backend/Party_Utility/ReturnBack/GoBack.hpp"
#include "./../backend/Party_Utility/StoleCoins/StoleSilver.hpp"
#include "./../backend/Party_Utility/Warp_Pipe/NormalPipe.hpp"
#include "./../backend/Party_Utility/Warp_Pipe/GoldPipe.hpp"
#include "./../backend/Inventory/Inventory.hpp"

using json = nlohmann::json;

// ==================== BUFFER GLOBALE ====================
static char result_buffer[8192];

const json json_to_cstring(const json& j) {
    std::string json_str = j.dump();
    if (json_str.length() >= sizeof(result_buffer))
    {
        std::cerr << "[C++] JSON too large: " << json_str.length() << std::endl;
        return "{}";
    }
    std::strcpy(result_buffer, json_str.c_str());
    return result_buffer;
}

// ==================== UTILITY FUNCTIONS ====================

/**
 * Converte Inventory.Utilities (array di unique_ptr<Items>) in JSON array
 */
json inventory_to_json(const std::array<std::unique_ptr<Items>, 3>& utilities)
{
    json items_array = json::array();

    for (const auto& item : utilities) 
    {
        if (item != nullptr)
        {
            items_array.push_back({
                {"name", item->name},
                {"description", item->description},
                {"price", item->price}
            });
        }else{
            items_array.push_back(nullptr);
        }
    }
    return items_array;
}

// ==================== GAME ENTITY WRAPPER ====================

struct GameEntity
{
    
    int id;
    int position = 0;
    std::string name;
    bool is_bot = false;
    std::string difficulty = "";  // Solo per bot
    int percentage = 0;            // Solo per bot
    bool MoreDice = false;         // Solo per bot
    bool UseSpecialDie = false;    // Solo per bot
    
    // Puntatore alla vera istanza (Player o Bot)
    std::shared_ptr<Player> player_ptr;
    std::shared_ptr<Bot> bot_ptr;
    
    // Dadi
    std::shared_ptr<Dice<6>> dice_standard;           // 1-6
    std::shared_ptr<Dice<2>> dice_0_1;                // 0-1
    std::shared_ptr<Dice<3>> dice_4_5_6;              // 4-5-6
    std::shared_ptr<Dice<6>> dice_triple;             // 1-6 x3
    
    // Items con flag di attivazione
    std::shared_ptr<GOahead> go_ahead;
    bool go_ahead_active = false;
    
    std::shared_ptr<GoBack> go_back;
    bool go_back_active = false;
    
    std::shared_ptr<StoleSilver> steal_silver;
    bool steal_silver_active = false;
    
    bool steal_gold_active = false;
    
    std::shared_ptr<NormalPipe> normal_pipe;
    bool normal_pipe_active = false;
    
    std::shared_ptr<GoldPipe> gold_pipe;
    bool gold_pipe_active = false;
    
    // Riferimento all'inventory vero
    Inventory* get_inventory()
    {
        if (is_bot && bot_ptr) 
            return &bot_ptr->Inv;
        if (!is_bot && player_ptr)
            return &player_ptr->Inv;

        return nullptr;
    }
};

std::map<int, GameEntity> game_entities;

// ==================== C INTERFACE ====================
extern "C" {

    // ==================== INITIALIZATION ====================
    
    /**
     * Inizializza il gioco con i giocatori
     * @param players_json Array di JSON: [{"id": 1, "name": "Alice", "is_bot": false}, ...]
     */
    const json init_game(const json players_json) {
        try {
            if (!players_json) return "{}";
            
            json entities_data = json::parse(players_json);
            game_entities.clear();
            
            for (auto& entity_data : entities_data)
            {
                int id = entity_data.value("id", 0);
                std::string name = entity_data.value("name", "Player");
                bool is_bot = entity_data.value("is_bot", false);
                
                GameEntity entity;
                entity.id = id;
                entity.name = name;
                entity.is_bot = is_bot;
                
                if (is_bot)
                {
                    // Crea un vero Bot
                    std::string difficulty = entity_data.value("difficulty", "normal");
                    entity.bot_ptr = std::make_shared<Bot>(name, difficulty);
                    entity.difficulty = difficulty;
                    entity.percentage = entity.bot_ptr->percentage;
                    entity.MoreDice = false;
                    entity.UseSpecialDie = false;
                } else {
                    // Crea un vero Player
                    entity.player_ptr = std::make_shared<Player>(name);
                }
                
                // Inizializza dadi
                std::array<int, 6> faces_1_6 = {1, 2, 3, 4, 5, 6};
                entity.dice_standard = std::make_shared<Dice<6>>("Standard", "Standard 1-6", faces_1_6, 0);
                
                std::array<int, 2> faces_0_1 = {0, 1};
                entity.dice_0_1 = std::make_shared<Dice<2>>("Binary", "0 or 1", faces_0_1, 0);
                
                std::array<int, 3> faces_4_5_6 = {4, 5, 6};
                entity.dice_4_5_6 = std::make_shared<Dice<3>>("High", "4, 5 or 6", faces_4_5_6, 0);
                
                std::array<int, 6> faces_triple = {1, 2, 3, 4, 5, 6};
                entity.dice_triple = std::make_shared<Dice<6>>("Triple", "Triple roll 1-6", faces_triple, 0);
                
                // Inizializza items
                entity.go_ahead = std::make_shared<GOahead>();
                entity.go_back = std::make_shared<GoBack>();
                entity.steal_silver = std::make_shared<StoleSilver>();
                entity.normal_pipe = std::make_shared<NormalPipe>();
                entity.gold_pipe = std::make_shared<GoldPipe>();
                
                game_entities[id] = entity;
            }
            
            json response =
            {
                {"status", "initialized"},
                {"num_entities", game_entities.size()},
                {"game_id", "game_001"}
            };
            
            std::cout << "[C++] Game initialized with " << game_entities.size() 
                      << " entities" << std::endl;
            
            return json_to_cstring(response);
        } catch (const std::exception& e) {
            json error = {
                {"status", "error"},
                {"message", e.what()}
            };
            return json_to_cstring(error);
        }
    }

    // ==================== ENTITY STATE ====================
    
    /**
     * Ottiene lo stato di un'entità (Player o Bot)
     */
    const json get_entity_state(int entity_id) {
        try {
            if (game_entities.find(entity_id) == game_entities.end()) {
                json error = {
                    {"status", "error"},
                    {"message", "Entity not found"}
                };
                return json_to_cstring(error);
            }
            
            GameEntity& e = game_entities[entity_id];
            Inventory* inv = e.get_inventory();
            
            if (!inv) {
                json error = {
                    {"status", "error"},
                    {"message", "No inventory found"}
                };
                return json_to_cstring(error);
            }
            
            json state = {
                {"entity_id", e.id},
                {"name", e.name},
                {"is_bot", e.is_bot},
                {"position", e.position},
                {"silver_coins", inv->silverCoins},
                {"gold_coins", inv->goldCoins},
                {"inventory", inventory_to_json(inv->Utilities)},
                {"active_items", {
                    {"go_ahead", e.go_ahead_active},
                    {"go_back", e.go_back_active},
                    {"steal_silver", e.steal_silver_active},
                    {"steal_gold", e.steal_gold_active},
                    {"normal_pipe", e.normal_pipe_active},
                    {"gold_pipe", e.gold_pipe_active}
                }}
            };
            
            // Aggiungi info bot se è un bot
            if (e.is_bot) {
                state["difficulty"] = e.difficulty;
                state["percentage"] = e.percentage;
                state["MoreDice"] = e.MoreDice;
                state["UseSpecialDie"] = e.UseSpecialDie;
            }
            
            return json_to_cstring(state);
        } catch (const std::exception& e) {
            json error = {
                {"status", "error"},
                {"message", e.what()}
            };
            return json_to_cstring(error);
        }
    }

    // ==================== DICE ACTIVATION & ACTIONS ====================
    
    /**
     * Attiva un tipo di dado
     */
    const json activate_dice(int entity_id, const json dice_type) {
        try {
            if (game_entities.find(entity_id) == game_entities.end()) {
                json error = {
                    {"status", "error"},
                    {"message", "Entity not found"},
                    {"action", "activate_dice"}
                };
                return json_to_cstring(error);
            }
            
            GameEntity& e = game_entities[entity_id];
            std::string type = dice_type ? std::string(dice_type) : "";
            
            json response = {
                {"status", "success"},
                {"action", "activate_dice"},
                {"entity_id", entity_id},
                {"dice_type", type},
                {"activated", true}
            };
            
            std::cout << "[C++] Entity " << entity_id << " activated " << type 
                      << " dice" << std::endl;
            
            return json_to_cstring(response);
        } catch (const std::exception& e) {
            json error = {
                {"status", "error"},
                {"message", e.what()},
                {"action", "activate_dice"}
            };
            return json_to_cstring(error);
        }
    }

    /**
     * Lancia il dado attivo
     */
    const json roll_dice(int entity_id, const json dice_type) {
        try {
            if (game_entities.find(entity_id) == game_entities.end()) {
                json error = {
                    {"status", "error"},
                    {"message", "Entity not found"},
                    {"action", "roll_dice"}
                };
                return json_to_cstring(error);
            }
            
            GameEntity& e = game_entities[entity_id];
            std::string type = dice_type ? std::string(dice_type) : "standard";
            
            int roll_result = 0;
            int rolls_count = 1;
            
            // Usa TrowDice() della tua classe Dice
            if (type == "standard") {
                roll_result = e.dice_standard->TrowDice();
            }
            else if (type == "0_1") {
                roll_result = e.dice_0_1->TrowDice();
            }
            else if (type == "4_5_6") {
                roll_result = e.dice_4_5_6->TrowDice();
            }
            else if (type == "triple") {
                roll_result = e.dice_triple->TrowDice() + 
                             e.dice_triple->TrowDice() + 
                             e.dice_triple->TrowDice();
                rolls_count = 3;
            }
            else {
                json error = {
                    {"status", "error"},
                    {"message", "Unknown dice type"},
                    {"action", "roll_dice"}
                };
                return json_to_cstring(error);
            }
            
            // Aggiorna posizione
            e.position += roll_result;
            
            json result = {
                {"status", "success"},
                {"action", "roll_dice"},
                {"entity_id", entity_id},
                {"dice_type", type},
                {"rolls_count", rolls_count},
                {"result", roll_result},
                {"new_position", e.position}
            };
            
            std::cout << "[C++] Entity " << entity_id << " rolled " << roll_result 
                      << " on " << type << " dice" << std::endl;
            
            return json_to_cstring(result);
        } catch (const std::exception& e) {
            json error = {
                {"status", "error"},
                {"message", e.what()},
                {"action", "roll_dice"}
            };
            return json_to_cstring(error);
        }
    }

    // ==================== ITEM ACTIVATION & ACTIONS ====================
    
    /*
     * Attiva un item
     */
    const json activate_item(int entity_id, const json item_type) {
        try {
            if (game_entities.find(entity_id) == game_entities.end()) {
                json error = {
                    {"status", "error"},
                    {"message", "Entity not found"},
                    {"action", "activate_item"}
                };
                return json_to_cstring(error);
            }
            
            GameEntity& e = game_entities[entity_id];
            std::string type = item_type ? std::string(item_type) : "";
            
            // Disattiva tutti gli item
            e.go_ahead_active = false;
            e.go_back_active = false;
            e.steal_silver_active = false;
            e.steal_gold_active = false;
            e.normal_pipe_active = false;
            e.gold_pipe_active = false;
            
            // Attiva l'item selezionato
            if (type == "go_ahead") {
                e.go_ahead_active = true;
            } else if (type == "go_back") {
                e.go_back_active = true;
            } else if (type == "steal_silver") {
                e.steal_silver_active = true;
            } else if (type == "steal_gold") {
                e.steal_gold_active = true;
            } else if (type == "normal_pipe") {
                e.normal_pipe_active = true;
            } else if (type == "gold_pipe") {
                e.gold_pipe_active = true;
            } else {
                json error = {
                    {"status", "error"},
                    {"message", "Unknown item type"},
                    {"action", "activate_item"}
                };
                return json_to_cstring(error);
            }
            
            json response = {
                {"status", "success"},
                {"action", "activate_item"},
                {"entity_id", entity_id},
                {"item_type", type},
                {"activated", true}
            };
            
            std::cout << "[C++] Entity " << entity_id << " activated " << type 
                      << " item" << std::endl;
            
            return json_to_cstring(response);
        } catch (const std::exception& e) {
            json error = {
                {"status", "error"},
                {"message", e.what()},
                {"action", "activate_item"}
            };
            return json_to_cstring(error);
        }
    }

    /**
     * Usa l'item attivo
     */
    const json use_item(int entity_id, int target_entity_id) {
        try {
            if (game_entities.find(entity_id) == game_entities.end()) {
                json error = {
                    {"status", "error"},
                    {"message", "Entity not found"},
                    {"action", "use_item"}
                };
                return json_to_cstring(error);
            }
            
            GameEntity& e = game_entities[entity_id];
            Inventory* inv = e.get_inventory();
            
            if (!inv) {
                json error = {
                    {"status", "error"},
                    {"message", "No inventory"},
                    {"action", "use_item"}
                };
                return json_to_cstring(error);
            }
            
            json action_result;
            std::string used_item = "none";

            // ========== GOahead ==========
            if (e.go_ahead_active) {
                used_item = "go_ahead";
                int boost = e.go_ahead->UseItem();
                e.position += boost;
                action_result = {
                    {"item_effect", "boost"},
                    {"boost_amount", boost},
                    {"new_position", e.position}
                };
                e.go_ahead_active = false;
                std::cout << "[C++] Entity " << entity_id << " used GO_AHEAD" << std::endl;
            }
            
            // ========== GoBack ==========
            else if (e.go_back_active) {
                used_item = "go_back";
                if (game_entities.find(target_entity_id) == game_entities.end()) {
                    json error = {
                        {"status", "error"},
                        {"message", "Target entity not found"},
                        {"action", "use_item"}
                    };
                    return json_to_cstring(error);
                }
                
                int blocks_back = e.go_back->UseItem();
                GameEntity& target = game_entities[target_entity_id];
                target.position -= blocks_back;
                if (target.position < 0) target.position = 0;
                
                action_result = {
                    {"item_effect", "send_back"},
                    {"target_entity_id", target_entity_id},
                    {"blocks_moved_back", blocks_back},
                    {"target_new_position", target.position}
                };
                e.go_back_active = false;
                std::cout << "[C++] Entity " << entity_id << " sent Entity " 
                          << target_entity_id << " back " << blocks_back << " blocks" << std::endl;
            }
            
            // ========== StoleSilver ==========
            else if (e.steal_silver_active) {
                used_item = "steal_silver";
                if (game_entities.find(target_entity_id) == game_entities.end()) {
                    json error = {
                        {"status", "error"},
                        {"message", "Target entity not found"},
                        {"action", "use_item"}
                    };
                    return json_to_cstring(error);
                }
                
                int stolen = e.steal_silver->UseItem();
                GameEntity& target = game_entities[target_entity_id];
                Inventory* target_inv = target.get_inventory();
                
                if (!target_inv) {
                    json error = {
                        {"status", "error"},
                        {"message", "Target has no inventory"},
                        {"action", "use_item"}
                    };
                    return json_to_cstring(error);
                }
                
                stolen = std::min(stolen, target_inv->silverCoins);
                target_inv->silverCoins -= stolen;
                inv->silverCoins += stolen;
                
                action_result = {
                    {"item_effect", "steal"},
                    {"coin_type", "silver"},
                    {"target_entity_id", target_entity_id},
                    {"amount_stolen", stolen},
                    {"thief_total_silver", inv->silverCoins},
                    {"victim_remaining_silver", target_inv->silverCoins}
                };
                e.steal_silver_active = false;
                std::cout << "[C++] Entity " << entity_id << " stole " << stolen 
                          << " silver" << std::endl;
            }
            
            // ========== StoleGold ==========
            // TODO: Quando crei la classe StoleGold, usare lo stesso pattern
            else if (e.steal_gold_active) {
                used_item = "steal_gold";
                if (game_entities.find(target_entity_id) == game_entities.end()) {
                    json error = {
                        {"status", "error"},
                        {"message", "Target entity not found"},
                        {"action", "use_item"}
                    };
                    return json_to_cstring(error);
                }
                
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(1, 50);
                int stolen = dis(gen);
                
                GameEntity& target = game_entities[target_entity_id];
                Inventory* target_inv = target.get_inventory();
                
                if (!target_inv) {
                    json error = {
                        {"status", "error"},
                        {"message", "Target has no inventory"},
                        {"action", "use_item"}
                    };
                    return json_to_cstring(error);
                }
                
                stolen = std::min(stolen, target_inv->goldCoins);
                target_inv->goldCoins -= stolen;
                inv->goldCoins += stolen;
                
                action_result = {
                    {"item_effect", "steal"},
                    {"coin_type", "gold"},
                    {"target_entity_id", target_entity_id},
                    {"amount_stolen", stolen},
                    {"thief_total_gold", inv->goldCoins},
                    {"victim_remaining_gold", target_inv->goldCoins}
                };
                e.steal_gold_active = false;
                std::cout << "[C++] Entity " << entity_id << " stole " << stolen 
                          << " gold" << std::endl;
            }
            
            // ========== NormalPipe ==========
            // TODO: Implementare logica completa con mappa coordinate della tua classe
            else if (e.normal_pipe_active) {
                used_item = "normal_pipe";
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(-4, 4);
                int warp_offset = dis(gen);
                
                int old_position = e.position;
                e.position += warp_offset;
                if (e.position < 0) e.position = 0;
                
                action_result = {
                    {"item_effect", "warp"},
                    {"warp_type", "normal_pipe"},
                    {"old_position", old_position},
                    {"new_position", e.position},
                    {"warp_distance", warp_offset}
                };
                e.normal_pipe_active = false;
                std::cout << "[C++] Entity " << entity_id << " used NORMAL PIPE" << std::endl;
            }
            
            // ========== GoldPipe ==========
            // TODO: Implementare logica completa con scelta del bot su coordinate[13][13]
            else if (e.gold_pipe_active) {
                used_item = "gold_pipe";
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis_x(0, 12);
                std::uniform_int_distribution<> dis_y(0, 12);
                
                int bot_choice_x = dis_x(gen);
                int bot_choice_y = dis_y(gen);
                
                int old_position = e.position;
                e.position = bot_choice_x * 13 + bot_choice_y;
                
                action_result = {
                    {"item_effect", "warp"},
                    {"warp_type", "gold_pipe"},
                    {"old_position", old_position},
                    {"new_position", e.position},
                    {"bot_choice_coordinates", {
                        {"x", bot_choice_x},
                        {"y", bot_choice_y}
                    }}
                };
                e.gold_pipe_active = false;
                std::cout << "[C++] Entity " << entity_id << " used GOLD PIPE" << std::endl;
            }
            
            else {
                json error = {
                    {"status", "error"},
                    {"message", "No item is active"},
                    {"action", "use_item"}
                };
                return json_to_cstring(error);
            }
            
            json response = {
                {"status", "success"},
                {"action", "use_item"},
                {"entity_id", entity_id},
                {"item_type", used_item},
                {"result", action_result}
            };
            
            return json_to_cstring(response);
        } catch (const std::exception& e) {
            json error = {
                {"status", "error"},
                {"message", e.what()},
                {"action", "use_item"}
            };
            return json_to_cstring(error);
        }
    }

    // ==================== INVENTORY ACTIONS ====================
    
    /**
     * Ottieni inventario completo
     */
    const json get_inventory(int entity_id) {
        try {
            if (game_entities.find(entity_id) == game_entities.end()) {
                json error = {
                    {"status", "error"},
                    {"message", "Entity not found"},
                    {"action", "get_inventory"}
                };
                return json_to_cstring(error);
            }
            
            GameEntity& e = game_entities[entity_id];
            Inventory* inv = e.get_inventory();
            
            if (!inv) {
                json error = {
                    {"status", "error"},
                    {"message", "No inventory"},
                    {"action", "get_inventory"}
                };
                return json_to_cstring(error);
            }
            
            json response = {
                {"status", "success"},
                {"action", "get_inventory"},
                {"entity_id", entity_id},
                {"silver_coins", inv->silverCoins},
                {"gold_coins", inv->goldCoins},
                {"items", inventory_to_json(inv->Utilities)},
                {"max_items", 3}
            };
            
            return json_to_cstring(response);
        } catch (const std::exception& e) {
            json error = {
                {"status", "error"},
                {"message", e.what()},
                {"action", "get_inventory"}
            };
            return json_to_cstring(error);
        }
    }

    // ==================== COIN ACTIONS ====================
    
    /**
     * Aggiungi monete
     */
    const json add_coins(int entity_id, int silver_amount, int gold_amount) {
        try {
            if (game_entities.find(entity_id) == game_entities.end()) {
                json error = {
                    {"status", "error"},
                    {"message", "Entity not found"},
                    {"action", "add_coins"}
                };
                return json_to_cstring(error);
            }
            
            GameEntity& e = game_entities[entity_id];
            Inventory* inv = e.get_inventory();
            
            if (!inv) {
                json error = {
                    {"status", "error"},
                    {"message", "No inventory"},
                    {"action", "add_coins"}
                };
                return json_to_cstring(error);
            }
            
            inv->silverCoins += silver_amount;
            inv->goldCoins += gold_amount;
            
            json response = {
                {"status", "success"},
                {"action", "add_coins"},
                {"entity_id", entity_id},
                {"silver_added", silver_amount},
                {"gold_added", gold_amount},
                {"total_silver", inv->silverCoins},
                {"total_gold", inv->goldCoins}
            };
            
            std::cout << "[C++] Entity " << entity_id << " gained " << silver_amount 
                      << " silver and " << gold_amount << " gold" << std::endl;
            
            return json_to_cstring(response);
        } catch (const std::exception& e) {
            json error = {
                {"status", "error"},
                {"message", e.what()},
                {"action", "add_coins"}
            };
            return json_to_cstring(error);
        }
    }

    /**
     * Rimuovi monete
     */
    const json remove_coins(int entity_id, int silver_amount, int gold_amount) {
        try {
            if (game_entities.find(entity_id) == game_entities.end()) {
                json error = {
                    {"status", "error"},
                    {"message", "Entity not found"},
                    {"action", "remove_coins"}
                };
                return json_to_cstring(error);
            }
            
            GameEntity& e = game_entities[entity_id];
            Inventory* inv = e.get_inventory();
            
            if (!inv) {
                json error = {
                    {"status", "error"},
                    {"message", "No inventory"},
                    {"action", "remove_coins"}
                };
                return json_to_cstring(error);
            }
            
            if (inv->silverCoins < silver_amount || inv->goldCoins < gold_amount) {
                json error = {
                    {"status", "error"},
                    {"message", "Not enough coins"},
                    {"action", "remove_coins"}
                };
                return json_to_cstring(error);
            }
            
            inv->silverCoins -= silver_amount;
            inv->goldCoins -= gold_amount;
            
            json response = {
                {"status", "success"},
                {"action", "remove_coins"},
                {"entity_id", entity_id},
                {"silver_removed", silver_amount},
                {"gold_removed", gold_amount},
                {"total_silver", inv->silverCoins},
                {"total_gold", inv->goldCoins}
            };
            
            std::cout << "[C++] Entity " << entity_id << " spent " << silver_amount 
                      << " silver and " << gold_amount << " gold" << std::endl;
            
            return json_to_cstring(response);
        } catch (const std::exception& e) {
            json error = {
                {"status", "error"},
                {"message", e.what()},
                {"action", "remove_coins"}
            };
            return json_to_cstring(error);
        }
    }

    // ==================== GAME STATE ====================
    
    /**
     * Ottieni lo stato di tutte le entità (per il web)
     */
    const json get_game_state() {
        try {
            json all_entities = json::array();
            
            for (auto& [id, entity] : game_entities) {
                Inventory* inv = entity.get_inventory();
                if (!inv) continue;
                
                json e = {
                    {"entity_id", entity.id},
                    {"name", entity.name},
                    {"is_bot", entity.is_bot},
                    {"position", entity.position},
                    {"silver_coins", inv->silverCoins},
                    {"gold_coins", inv->goldCoins},
                    {"inventory", inventory_to_json(inv->Utilities)},
                    {"active_items", {
                        {"go_ahead", entity.go_ahead_active},
                        {"go_back", entity.go_back_active},
                        {"steal_silver", entity.steal_silver_active},
                        {"steal_gold", entity.steal_gold_active},
                        {"normal_pipe", entity.normal_pipe_active},
                        {"gold_pipe", entity.gold_pipe_active}
                    }}
                };
                
                if (entity.is_bot) {
                    e["difficulty"] = entity.difficulty;
                    e["percentage"] = entity.percentage;
                    e["MoreDice"] = entity.MoreDice;
                    e["UseSpecialDie"] = entity.UseSpecialDie;
                }
                
                all_entities.push_back(e);
            }
            
            json response = {
                {"status", "success"},
                {"action", "get_game_state"},
                {"entities", all_entities},
                {"total_entities", game_entities.size()}
            };
            
            return json_to_cstring(response);
        } catch (const std::exception& e) {
            json error = {
                {"status", "error"},
                {"message", e.what()},
                {"action", "get_game_state"}
            };
            return json_to_cstring(error);
        }
    }

    // ==================== DEBUG ====================
    
    const json test_bridge() {
        json test = {
            {"status", "bridge_working"},
            {"message", "C++ Game Core is active"},
            {"version", "3.0"},
            {"features", {
                "real_player_class",
                "real_bot_class",
                "all_dices_with_trowdice",
                "all_items_with_useitem",
                "warp_pipes_todo",
                "real_inventory",
                "multiplayer_ready"
            }},
            {"timestamp", "2026-06-08"}
        };
        return json_to_cstring(test);
    }
 
} // extern "C"
