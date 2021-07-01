#ifndef __VIEWER_HPP__
#define __VIEWER_HPP__

#include <SFML/Graphics.hpp>
#include "pixels.hpp"
#include "mandelbrot.hpp"

template <typename T>
std::ostream& operator<<(std::ostream& os, const sf::Vector2<T>& rhs)
{
    return os << "( " << rhs.x << " , " << rhs.y << " )";
}

class Viewer
{
private:

    sf::RenderWindow _window;
    Pixels _pixels;

private:

    void center()
    {
        auto _vm = sf::VideoMode::getDesktopMode();
        auto vm = sf::Vector2i(_vm.width, _vm.height);
        vm.x /= 2, vm.y /= 2;

        auto sz = _window.getSize();
        vm.x -= sz.x/2, vm.y -= sz.y/2;

        _window.setPosition(vm);
    }

    // sf::Vector2f uv;
    float start_size = 4.0f;
    float zoom = start_size;

    void scale_window()
    {
        // Find pixel size of window
        auto size = _window.getSize();

        // Create a copy of the window's view to modify it
        auto view = _window.getView();
        view.setCenter({ 0, 0 });

        // Always have the smallest dimension be the size of the
        // zoom
        if (size.x > size.y)
        {
            float a = size.x / static_cast<float>(size.y);
            view.setSize({ a * zoom, zoom });
        }
        else
        {
            float a = size.y / static_cast<float>(size.x);
            view.setSize({ zoom, a * zoom });
        }

        // Apply the changes to the window
        _window.setView(view);
    }

    void scale_pixels()
    {

        // Find the pixel size of the window (to scale the pixel buffer by)
        const auto size = _window.getSize();
        auto did_resize = _pixels.set_size(size);
        // std::cout << "Buffer " << (did_resize ? "" : "not" ) << " resized\n";

        // Find the camera space of the window (to scale the pixel rendering by)
        const auto view = _window.getView();
        const auto vsize = view.getSize();

        auto uv = sf::Vector2f(
            vsize.x / size.x,
            vsize.y / size.y
        );
        _pixels.setScale(uv);

        auto top_left = -vsize / 2.f;
        _pixels.setPosition(top_left);
    }

    void reset_screen()
    {
        scale_window();
        scale_pixels();
    }


public:

    Viewer(std::size_t width, std::size_t height) :
        _window { 
            sf::VideoMode(width, height), 
            "Mandelbrot Viewer",
            sf::Style::Default
        },
        _pixels { _window.getSize() }
    {
        center();
        reset_screen();
    }

    bool is_running() const
    { return _window.isOpen(); }
    
    void handle_event(sf::Event& e)
    {
        if (e.type == sf::Event::Closed)
        {
            _window.close();
        }
        else if (e.type == sf::Event::Resized)
        {
            reset_screen();
        }
    }

    void draw()
    {
        _window.clear(sf::Color::Black);
        _window.draw(_pixels);
        _window.display();
    }

    void update()
    {
        sf::Event e;
        while (_window.pollEvent(e))
        {
            handle_event(e);
        }

        calculate_set();
        draw();
    }

    sf::Color get_color( const sf::Vector2f& coords )
    {
        // Complex number to iterate over 
        mb::complex c { coords.x, coords.y };
    
        // Calculate iterations 
        std::size_t iters = mb::mandelbrot(c);

        // Convert iterations into a color
        sf::Uint8 p = sf::Uint8(255) * (iters / static_cast<float>(mb::MAX_ITERS));
        return sf::Color(p, p, p, 255);
    }

    void calculate_set()
    {
        sf::Vector2i pixel = { 0, 0 };
        for (; pixel.y < _pixels.get_height(); ++pixel.y)
        {
            for (pixel.x = 0; pixel.x < _pixels.get_width(); ++pixel.x)
            { 
                auto coords = _window.mapPixelToCoords(pixel);
                _pixels.set_color(pixel, get_color(coords));
            }
        }
    }

};

#endif