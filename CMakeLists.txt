cmake_minimum_required(VERSION 4.2)
project(Wirtualna_kamera)

set(CMAKE_CXX_STANDARD 23)

find_package(SFML CONFIG REQUIRED)

add_executable(Wirtualna_kamera src/main.cpp)

target_link_libraries(Wirtualna_kamera PRIVATE SFML::Graphics SFML::Window SFML::System)