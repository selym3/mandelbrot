/*
clang++ -O3 -lsfml-window -lsfml-graphics -lsfml-system -Wall -pthread main.cpp -o main
./main
*/

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

sf::Color get_color(const sf::Vector2f& coords)
{
    // auto c = mb::complex(
    //     (((coords.x / static_cast<mb::number>(window::WIDTH)) * (mb::PLANE_WIDTH * 2)) - (mb::PLANE_WIDTH)),
    //     (window::HEIGHT/static_cast<mb::number>(window::WIDTH)) * ((coords.y / static_cast<mb::number>(window::HEIGHT)) * (mb::PLANE_HEIGHT * 2) - (mb::PLANE_HEIGHT))
    // );

    // cast into mb::number
    auto c = mb::complex(
        static_cast<mb::number>(coords.x), 
        static_cast<mb::number>(coords.y)
    );

    std::size_t iterations = mandelbrot(c);

    mb::number density = iterations / static_cast<mb::number>(mb::MAX_ITERS);
    std::size_t tmp = static_cast<std::size_t>(density * 255);

    return sf::Color(tmp,tmp,tmp,tmp);
}

void center(sf::RenderWindow& window)
{
    auto _vm = sf::VideoMode::getDesktopMode();
    auto vm = sf::Vector2i(_vm.width, _vm.height);
    vm.x /= 2, vm.y /= 2;

    auto sz = window.getSize();
    vm.x -= sz.x/2, vm.y -= sz.y/2;

    window.setPosition(vm);
}

int main()
{
    sf::RenderWindow window { 
        sf::VideoMode {
            window::WIDTH, 
            window::HEIGHT
        },
        "Mandelbrot Viewer",
        sf::Style::Default, 
    };

    center(window);

    Pixels pixels { 
        window.getSize() 
    }; 

    auto view = window.getView();
    view.setCenter({ 0, 0 });
    view.setSize({ 4, 2 });
    window.setView(view);

    pixels.setScale({ 4 / 600.0, 2 / 300.0 });
    pixels.setPosition({ -2, -1 });


    float zoom = 1.0;

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
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Space)
                {
                    auto m = sf::Mouse::getPosition(window);
                    auto mc = window.mapPixelToCoords(m);

                    std::cout << "( " << mc.x << " , " << mc.y << " )\n";
                }
                else if (event.key.code == sf::Keyboard::Y)
                {
                    ++mb::MAX_ITERS;
                }
                else if (event.key.code == sf::Keyboard::H)
                {
                    --mb::MAX_ITERS;
                }

                auto view = window.getView();

                // Zoom

                float factor = (event.key.code == sf::Keyboard::Q) * 1.1 +
                    (event.key.code == sf::Keyboard::E) * 0.9;

                if (factor != 0)
                {
                    zoom *= factor;
                    view.zoom(factor);
                }

                // Offset

                sf::Vector2f offset = sf::Vector2f(
                    // ((event.key.code == sf::Keyboard::Left) - (event.key.code == sf::Keyboard::Right)),
                    // ((event.key.code == sf::Keyboard::Up) - (event.key.code == sf::Keyboard::Down))
                    4 / 600.0 * ((event.key.code == sf::Keyboard::Left) - (event.key.code == sf::Keyboard::Right)),
                    2 / 300.0 * ((event.key.code == sf::Keyboard::Up) - (event.key.code == sf::Keyboard::Down))
                );

                //offset = static_cast<sf::Vector2f>(window.mapPixelToCoords(static_cast<sf::Vector2i>(offset)));

                view.move(offset);

                // Apply changes
                window.setView(view);
            }
        }

        window.clear(sf::Color::Black);
        
        sf::Vector2i pixel = { 0, 0 };
        for (; pixel.y < pixels.height; ++pixel.y)
        {
            for (pixel.x = 0; pixel.x < pixels.width; ++pixel.x)
            { 
                auto coords = window.mapPixelToCoords(pixel);
                pixels.set_color(pixel, get_color(coords));
            }
        }

        window.draw(pixels);
        window.display();

    }
}