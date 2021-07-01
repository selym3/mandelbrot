/*
clang++  main.cpp -lsfml-window -lsfml-graphics -lsfml-system -o main && ./main
*/

#include <iostream>
#include <chrono>

#include "viewer.hpp"

int main()
{
    Viewer app {
        640, 480
    };

    const sf::Int32 delay { static_cast<sf::Int32>(1000.0 / 1.0) }; 

    std::cout << "Began execution...\n";
    while (app.is_running())
    {
        auto start = std::chrono::system_clock::now();
        app.update();
        auto end = std::chrono::system_clock::now();
        
        double took = static_cast<std::chrono::duration<double>>(end-start).count();
        std::cout << "Took: " <<  took << " | FPS: " << (1.0 / took) << "\n";

        sf::sleep(sf::milliseconds(delay));
    }
    std::cout << "...Ended execution\n";
}