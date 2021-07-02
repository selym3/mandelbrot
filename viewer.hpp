#ifndef __VIEWER_HPP__
#define __VIEWER_HPP__

#include <SFML/Graphics.hpp>
#include <vector>
#include <thread>

#include "camera.hpp"
#include "pixels.hpp"
#include "mandelbrot.hpp"
#include "mcntrl.hpp"

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

    Camera<mb::number> _camera;
    using vec2 = Vec2<mb::number>;

    MouseControl mcntrl;

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
        // scale_window();
        // scale_pixels();
        // std::cout << "Reset screen: No op!\n";
    }

private:

    sf::Color get_color( const vec2& coords )
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
                auto coords = _camera.pixel_to_world(vec2::from(pixel));
                // std::cout << coords << "\n";
                _pixels.set_color(pixel, get_color(coords));
            }
        }
    }

    void calculate_set_multi(std::size_t threads)
    {
        if (threads == 0) return calculate_set();
        
        auto multi_supplier = [&](int who)
        {
            const auto size = _window.getSize();
            for (; who < size.x * size.y; who += threads)
            {
                const sf::Vector2u pixel { who % size.x, who / size.x };
                const auto coord = _camera.pixel_to_world(vec2::from(pixel));
                _pixels.set_color(pixel, get_color(coord));
            }
        };

        std::vector<std::thread> pool;
        for (int who = 0; who < threads; ++who)
            pool.emplace_back(multi_supplier, who);

        for (auto& thread : pool)
            thread.join();
    }

private:

    void draw()
    {
        _window.clear(sf::Color::Black);
        _window.draw(_pixels);
        _window.display();
    }

private:

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
        else if (e.type == sf::Event::MouseWheelScrolled)
        {
            float scalar = e.mouseWheelScroll.delta > 0 ? 1.01f : 0.99f;
            zoom_about(scalar, getMousePosition());
        }
        else if (e.type == sf::Event::KeyPressed)
        {
            if (e.key.code == sf::Keyboard::Escape)
            {
                _window.close();
                return;
            }

            // test code
            else if (e.key.code == sf::Keyboard::Up)
            {
                ++mb::MAX_ITERS;
            }
            else if (e.key.code == sf::Keyboard::Down)
            {
                --mb::MAX_ITERS;
            }
        }
    }

    template <typename Vt = sf::Vector2i>
    Vt getMousePosition() const
    { return static_cast<Vt>(sf::Mouse::getPosition(_window)); }

    void zoom_about(float scalar, const sf::Vector2i& pixel)
    {
        auto view = _window.getView();
        const auto _p = vec2::from(pixel);

        auto before = _camera.pixel_to_world(_p);
        _camera *= scalar;
        _camera += (before - _camera.pixel_to_world(_p));
    }

    sf::Vector2i start = { 0, 0 };
    void update_view()
    {
        if (mcntrl.isPressed(0))
            start = getMousePosition();
        
        if (mcntrl.isHeld(0))
        {
            // Calculate offset based on last mouse position
            const auto mouse = getMousePosition();
            auto worldset = _window.mapPixelToCoords(mouse) - _window.mapPixelToCoords(start);

            // Apply offset
            auto view = _window.getView();
            view.move(-worldset);
            _window.setView(view);
            
            // Reset mouse position
            start = mouse;
        }
    }

public:

    Viewer(std::size_t width, std::size_t height) :
        _window { 
            sf::VideoMode(width, height), 
            "Mandelbrot Viewer",
            sf::Style::Default
        },
        _pixels { _window.getSize() },
        _camera { 0, vec2::from(_window.getSize()), vec2(-2,-2), vec2(+2,+2) }
    {
        center();
        reset_screen();
    }

    bool is_running() const
    { return _window.isOpen(); }

    void update()
    {
        sf::Event e;
        while (_window.pollEvent(e))
        {
            handle_event(e);
        }

        if (!mcntrl.isDown(1))
            calculate_set_multi(0);

        draw();

        update_view();
        mcntrl.update();
    }

};

#endif