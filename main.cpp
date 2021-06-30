#include "pixels.hpp"

int main()
{
    sf::RenderWindow window { 
        sf::VideoMode{600, 600},
        "Test Pixels",
        sf::Style::None, 
    };

    Pixels pixels { 
        window.getSize() 
    }; 

    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            { 
                window.close();
            }
        }

        window.clear();
        window.draw(pixels);
        window.display();
    }
}