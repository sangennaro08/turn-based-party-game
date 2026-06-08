#include <iostream>
#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

int main()
{
    std::string input_line;
    
    // Leggi da stdin una riga JSON
    while (std::getline(std::cin, input_line))
    {
        try {
            // Parsa il JSON ricevuto da Python
            json request = json::parse(input_line);
            
            // Estrai i dati
            std::string action = request["action"];
            int player_id = request["player_id"];
            
            // Qui puoi aggiungere la tua logica di gioco
            // per ora restituiamo una risposta semplice
            
            json response;
            response["status"] = "ok";
            response["action"] = action;
            response["player_id"] = player_id;
            response["result"] = "elaborato da C++";
            response["timestamp"] = "2026-06-08";
            
            // Scrivi su stdout il JSON
            std::cout << response.dump() << std::endl;
            std::cout.flush();
        }
        catch (const std::exception& e) {
            json error_response;
            error_response["status"] = "error";
            error_response["message"] = e.what();
            std::cout << error_response.dump() << std::endl;
            std::cout.flush();
        }
    }
    
    return 0;
}
