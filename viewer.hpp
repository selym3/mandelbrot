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

class Viewer
{
private:
    /////////////////
    // VIEWER DATA //
    /////////////////

    sf::RenderWindow _window;
    Pixels _pixels;

    mb::camera _camera;
    MouseControl _mcntrl;

private:
    /////////////////////// 
    // VIEW MODE CONTROL //
    ///////////////////////

    enum ViewingMode
    {
        MouseControl = 0,
        KeyboardControl,
        Cinematic,

        // Keep Last
        ModeCount
    };

    ViewingMode _vmode;

    ViewingMode next_vmode() const
    {
        return static_cast<ViewingMode>(
            (_vmode + 1) % 
            ViewingMode::ModeCount
        );
    }

    // main stuff

    void update_view()
    {
        switch (_vmode)
        {
        case ViewingMode::MouseControl:
            update_view_mouse();
            break;
        case ViewingMode::Cinematic:
            zoom_about(0.9, getMousePosition());
            break;
        case ViewingMode::KeyboardControl:
            update_view_keyboard();
            break;
        default:
            break;
        };
    }
    
    void handle_event_vmode(sf::Event& e)
    {
        switch (_vmode)
        {
        case ViewingMode::MouseControl:
        {   
            if (e.type == sf::Event::MouseWheelScrolled)     
            {
                float scalar = e.mouseWheelScroll.delta > 0 ? 1.1f : 0.9f;
                zoom_about(scalar, getMousePosition());
            }
            break;
        }
        default:
            break;
        }
    }

    // helpers 

    mb::vec2 start = { 0, 0 };
    void update_view_mouse()
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

    void update_view_keyboard()
    {
        using KB = sf::Keyboard;

        // zoom
        if (KB::isKeyPressed(KB::Q))
            _camera *= 0.9;
        else if (KB::isKeyPressed(KB::E))
            _camera *= 1.1;


        // translation
        auto shift = mb::vec2(
            (KB::isKeyPressed(KB::D) - KB::isKeyPressed(KB::A)),
            (KB::isKeyPressed(KB::S) - KB::isKeyPressed(KB::W))
        );

        shift *= 5 * (_camera.pixel_to_world({1,1}) - _camera.pixel_to_world({0,0}));

        _camera += shift;
    }
    
    template <typename Vt = sf::Vector2i>
    Vt getMousePosition() const
    { return static_cast<Vt>(sf::Mouse::getPosition(_window)); }

    void zoom_about(float scalar, const sf::Vector2i& pixel)
    {
        const auto _p = mb::vec2::from(pixel);

        auto before = _camera.pixel_to_world(_p);
        _camera *= scalar;
        _camera += (before - _camera.pixel_to_world(_p));
    }

private:
    ////////////////////
    // SCREEN SCALING //
    ////////////////////

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

        if (size.x > size.y)
            size /= size.y;
        else
            size /= size.x;
        
        _camera *= size;
    }

    void scale()
    {
        scale_camera();
    }

    mb::camera get_camera()
    {
        const static mb::vec2 zero = 
            mb::vec2(0, 0);

        const static mb::vec2 bottom = mb::vec2(-2, -2),
            top = mb::vec2(+2, +2);

        return mb::camera(
            zero, mb::vec2::from(_pixels.get_size()),
            bottom, top
        );
    }


private:
    ////////////////////////
    // COLOR CALCULATIONS //
    ////////////////////////

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
        
        auto multi_supplier = [&](std::size_t who)
        {
            const auto size = _pixels.get_size();
            for (; who < size.x * size.y; who += threads)
            {
                const sf::Vector2<std::size_t> pixel { who % size.x, who / size.x };
                const auto coord = _camera.pixel_to_world(mb::vec2::from(pixel));
                _pixels.set_color(pixel, get_color(coord));
            }
        };

        std::vector<std::thread> pool;
        for (std::size_t who = 0; who < threads; ++who)
            pool.emplace_back(multi_supplier, who);

        for (auto& thread : pool)
            thread.join();
    }

private:
    //////////////////
    // DRAW ROUTINE //
    //////////////////

    void draw()
    {
        _window.clear(sf::Color::Black);
        _window.draw(_pixels);
        _window.display();
    }

private:
    ///////////////////
    // EVENT HANDLER //
    ///////////////////

    void handle_event(sf::Event& e)
    {
        handle_event_vmode(e);

        if (e.type == sf::Event::Closed)
        {
            _window.close();
            return;
        }

        else if (e.type == sf::Event::Resized)
        {
            scale();
        }

        else if (e.type == sf::Event::KeyPressed)
        {
            if (e.key.code == sf::Keyboard::Escape)
            {
                _window.close();
                return;
            }

            // test code
            else if (e.key.code == sf::Keyboard::Z)
            {
                scale();
            }
            else if (e.key.code == sf::Keyboard::X)
            {
                _vmode = next_vmode();
            }

            // test code
            else if (e.key.code == sf::Keyboard::Equal)
            {
                ++mb::MAX_ITERS;
            }
            else if (e.key.code == sf::Keyboard::Hyphen)
            {
                --mb::MAX_ITERS;
            }
        }
    }

public:
    /////////////////
    // CONSTRUCTOR //
    /////////////////

    Viewer(std::size_t width, std::size_t height) :
        _window { 
            sf::VideoMode(width, height), 
            "Mandelbrot Viewer",
            sf::Style::Default,
        },
        _pixels { _window.getSize() },
        _camera { get_camera() },
        _vmode { ViewingMode::MouseControl }
    {
        center();
    }

public:
    ///////////////
    // MAIN LOOP //
    ///////////////
    
    bool is_running() const
    { return _window.isOpen(); }

    void update()
    {
        sf::Event e;
        while (_window.pollEvent(e))
        {
            handle_event(e);
        }

        calculate_set_multi(mb::THREADS);
        draw();

        update_view();
        _mcntrl.update();
    }

};

#endif