#include <iostream>
#include "pixels.hpp"
#include "constants.hpp"

std::size_t mandelbrot(const mb::complex& c)
{
    mb::complex z = 0;

    std::size_t iter = 0;
    for (; iter < mb::MAX_ITERS; ++iter)
    {
        z = (z * z) + c;

        if (std::abs(z) > mb::BOUNDS)
            break;
    }

    return iter;
}

sf::Color get_color(const sf::Vector2u& pixel)
{
    auto c = mb::complex(
        (pixel.x / static_cast<mb::number>(window::WIDTH)) * (mb::PLANE_WIDTH * 2) - (mb::PLANE_WIDTH),
        (pixel.y / static_cast<mb::number>(window::HEIGHT)) * (mb::PLANE_HEIGHT * 2) - (mb::PLANE_HEIGHT)
    );

    std::size_t iterations = mandelbrot(c);
    mb::number density = iterations / static_cast<mb::number>(mb::MAX_ITERS);
    std::size_t tmp = static_cast<std::size_t>(density * 255);

    return sf::Color(tmp,tmp,tmp,tmp);
}

int main()
{
    sf::RenderWindow window { 
        sf::VideoMode {
            window::WIDTH, 
            window::HEIGHT
        },
        "Mandelbrot Viewer",
        sf::Style::None, 
    };

    window.setPosition({ 1920/2, 1080/2 });

    Pixels pixels { 
        window.getSize() 
    }; 

    bool first = true;
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

        window.clear(sf::Color::Black);
        
        if (first)
            first = false, window.display();
        else
            pixels.execute(get_color, 16);
        
        window.draw(pixels);
        window.display();

    }
}