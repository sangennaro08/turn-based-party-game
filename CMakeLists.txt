cmake_minimum_required(VERSION 3.20...4.4.2)

# Nome del Progetto CMake
project(TurnBasedPartyGame CXX)

# Imposta C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Attiva il modulo per scaricare le librerie esterne
include(FetchContent)

# ==============================================================================
# 1. SCARICAMENTO LIBRERIE ESTERNE (JSON e Pybind11)
# ==============================================================================

# Nlohmann JSON
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# Pybind11
FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG        v2.13.6
)
FetchContent_MakeAvailable(pybind11)

# ==============================================================================
# 2. FILE SORGENTI DEL PROGETTO
# ==============================================================================

# Inseriamo esplicitamente il file principale di Pybind11 e cerchiamo gli altri nel backend
file(GLOB_RECURSE BACKEND_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/backend/*.cpp")

set(SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/Bridge-BE-FE/Comms-backend/Comunication.cpp"
    ${BACKEND_SOURCES}
)

# ==============================================================================
# 3. CREAZIONE DEL MODULO PYTHON (Comunication)
# ==============================================================================

pybind11_add_module(Comunication ${SOURCES})

# FORZA il nome pulito senza suffissi strani
set_target_properties(Comunication PROPERTIES SUFFIX ".pyd")

# ==============================================================================
# 4. INCLUSIONE CARTELLE HEADERS (.hpp)
# ==============================================================================

target_include_directories(Comunication PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/backend
    ${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/backend/bots
    ${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/backend/Inventory
    ${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/backend/minigames
    ${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/backend/Player
    ${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/backend/Party_Utility
    ${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/backend/Party_Utility/Dices
    ${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/backend/Party_Utility/Dices/Type_of_dices
    ${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/backend/Party_Utility/GOahead
    ${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/backend/Party_Utility/ReturnBack
    ${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/backend/Party_Utility/StoleCoins
    ${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/backend/Party_Utility/Warp_Pipe
    ${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/Bridge-BE-FE
    ${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/Bridge-BE-FE/Comms-backend
)

# Forza la destinazione finale direttamente nella cartella Server
set_target_properties(Comunication PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/Bridge-BE-FE/Server"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/Bridge-BE-FE/Server"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/Bridge-BE-FE/Server"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/progetto_PARTY/Bridge-BE-FE/Server"
)

# ==============================================================================
# 5. COLLEGAMENTO LIBRERIE
# ==============================================================================
target_link_libraries(Comunication PRIVATE nlohmann_json::nlohmann_json)