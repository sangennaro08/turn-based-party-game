cmake_minimum_required(VERSION 3.16)

project(Bridge)

set(CMAKE_CXX_STANDARD 17)

find_package(Drogon CONFIG REQUIRED)

add_executable(Bridge Bridge.cpp)

target_link_libraries(Bridge PRIVATE Drogon::Drogon)