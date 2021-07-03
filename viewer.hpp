#ifndef __VIEWER_HPP__
#define __VIEWER_HPP__

#include <SFML/Graphics.hpp>
#include <vector>
#include <thread>

#include "pixels.hpp"
#include "mandelbrot.hpp"
#include "mcntrl.hpp"

template <typename T>
std::ostream& operator<<(std::ostream& os, const sf::Vector2<T>& rhs)
{
    return os << "( " << rhs.x << " , " << rhs.y << " )";
}

mb::camera make_camera(const sf::RenderTarget& target, const mb::vec2& bottom, const mb::vec2& top)
{
    return mb::camera(
        mb::vec2(0, 0), mb::vec2::from(target.getSize()),
        bottom, top
    );
}

class Viewer
{
private:

    sf::RenderWindow _window;
    Pixels _pixels;

    mb::camera _camera;
    MouseControl _mcntrl;

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

    void scale_camera()
    {
        _camera = get_camera();
        mb::vec2 size { mb::vec2::from(_window.getSize()) };
        std::cout << "Screen size is: " << size << "\n";

        if (size.x > size.y)
            size /= size.y;
        else
            size /= size.x;
        
        std::cout << "Ratio is: " << size << "\n";
        _camera *= size;
        std::cout << "Camera is: " << _camera << "\n";
        std::cout << "\n";
    }

    void scale_pixels()
    {
        const auto size = _window.getSize();
        bool did_resize = _pixels.set_size(size);

        // std::cout << "Pixel buffer size: " << _pixels.get_size() << "\n";

        if (did_resize)
            std::cout << "Resized pixel buffer\n";
        else
            std::cout << "Didn't resize pixel buffer\n";
    }

    void scale()
    {
        scale_camera();
        scale_pixels();
    }

    void debug_scale()
    {
        std::cout << "\n---\n";
        std::cout << "Window size: " << _window.getSize() << "\n";
        std::cout << "Pixels size: " << _pixels.get_size() << "\n";
        std::cout << "Camera data: " << _camera << "\n";
        std::cout << _camera._bottom << " -> " << _camera.world_to_pixel(_camera._bottom) << "\n";
        std::cout << _camera._top << " -> " << _camera.world_to_pixel(_camera._top) << "\n";
        std::cout << "\n---\n";
    }

    mb::camera get_camera()
    {
        const mb::vec2 bottom = mb::vec2(-2, -2),
            top = mb::vec2(+2, +2);

        // std::cout << bottom << "\n";
        // std::cout << top << "\n";
        // auto _c = make_camera(_window, bottom, top);
        // std::cout << _c << "\n";

        return make_camera(_window, bottom, top);
    }


private:

    sf::Color get_color( const mb::vec2& coords )
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
                auto coords = _camera.pixel_to_world(mb::vec2::from(pixel));
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
                const auto coord = _camera.pixel_to_world(mb::vec2::from(pixel));
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
            scale();
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
            else if (e.key.code == sf::Keyboard::Space)
            {
                debug_scale();
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

    mb::vec2 start = { 0, 0 };
    void update_view()
    {
        if (_mcntrl.isPressed(0))
            start = mb::vec2::from(getMousePosition());
        
        if (_mcntrl.isHeld(0))
        {
            // Calculate offset based on last mouse position
            const auto mouse = mb::vec2::from(getMousePosition());
            auto worldset = _camera.pixel_to_world(mouse) - _camera.pixel_to_world(start);

            // Apply offset
            _camera -= worldset;
            
            // Reset mouse position
            start = mouse;
        }
    }

    void zoom_about(float scalar, const sf::Vector2i& pixel)
    {
        const auto _p = mb::vec2::from(pixel);

        auto before = _camera.pixel_to_world(_p);
        _camera *= scalar;
        _camera += (before - _camera.pixel_to_world(_p));
    }

public:

    Viewer(std::size_t width, std::size_t height) :
        _window { 
            sf::VideoMode(width, height), 
            "Mandelbrot Viewer",
            sf::Style::Default,
            // sf::Style::None
        },
        _pixels { _window.getSize() },
        // _camera { mb::vec2(0), mb::vec2::from(_window.getSize()), mb::vec2(-2,-2), mb::vec2(+2,+2) }
        _camera { get_camera() }
    {
        center();
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

        // if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        // calculate_set_multi(0);
        calculate_set();
        draw();

        update_view();
        _mcntrl.update();
    }

};

#endif