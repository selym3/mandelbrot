#ifndef __PIXELS_HPP__
#define __PIXELS_HPP__

#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include <thread>

class Pixels : public sf::Drawable, public sf::Transformable
{
    /////////////////
    // BUFFER DATA //
    /////////////////

    std::vector<sf::Uint8> _data;

    std::size_t _get_size() const 
    { return width * height * 4; }

    std::size_t _get_index(const std::size_t x, const std::size_t y) const
    { return (( y * width ) + x) * 4; }

public:
    ///////////////////
    // CREATE BUFFER //
    ///////////////////

    const std::size_t width, height;

    Pixels(std::size_t width, std::size_t height) :
        width { width }, height { height }
    {
        _data.reserve(_get_size());
    }

    Pixels(const sf::Vector2u& size) : 
        Pixels(size.x, size.y)
    {
    }

    Pixels(const sf::VideoMode& vm) :
        Pixels(vm.width, vm.height)
    {
    }

    /////////////////
    // DRAW PIXELS //
    /////////////////

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override 
    {
        sf::Image image;
        image.create(width, height, &_data[0]);

        sf::Texture texture;
        texture.loadFromImage(image);

        sf::Sprite sprite;
        sprite.setTexture(texture, true);

        states.transform *= getTransform();
        target.draw(sprite, states);
    }

    //////////////////
    // WRITE PIXELS //
    //////////////////

    void set_color(std::size_t x, std::size_t y, const sf::Color& color)
    {
        auto index = _get_index(x, y);

        _data[index + 0] = color.r;
        _data[index + 1] = color.g;
        _data[index + 2] = color.b;
        _data[index + 3] = color.a;
    }
    
    template <typename T>
    void set_color(const sf::Vector2<T>& pixel, const sf::Color& color)
    { set_color(pixel.x, pixel.y, color); }

    void set_color(std::size_t x, std::size_t y, sf::Uint32 rgba)
    { set_color(x, y, sf::Color(rgba)); }

    template <typename T>
    void set_color(const sf::Vector2<T>& pixel, sf::Uint32 rgba)
    { set_color(pixel.x, pixel.y, rgba); }

    /////////////////
    // READ PIXELS //
    /////////////////

    sf::Color get_color(std::size_t x, std::size_t y) const
    {
        auto index = _get_index(x, y);
        
        return sf::Color(
            _data[index + 0],
            _data[index + 1],
            _data[index + 2],
            _data[index + 3]
        );
    }

    template <typename T>
    sf::Color get_color(const sf::Vector2<T>& pixel) const
    { return get_color(pixel.x, pixel.y); }

    //////////////////////
    // PIXEL OPERATIONS //
    //////////////////////

    using color_supplier = std::function<sf::Color(const sf::Vector2u& pixel)>;

    void execute(const color_supplier& supplier, std::size_t threads)
    {
        if (threads == 0) 
            return execute(supplier);
        
        auto multi_supplier = [&](auto who)
        {
            for (; who < width * height; who += threads)
            {
                auto pixel = sf::Vector2u( who % width, who / width);
                set_color(pixel, supplier(pixel));
            }
        };

        std::vector<std::thread> pool;
        for (int who = 0; who < threads; ++who)
            pool.emplace_back(multi_supplier, who);

        for (auto& thread : pool)
            thread.join();
    }

    void execute(const color_supplier& supplier)
    {
        sf::Vector2u pixel = { 0, 0 };
        for (; pixel.y < height; ++pixel.y)
            for (pixel.x = 0; pixel.x < width; ++pixel.x)
                set_color(pixel, supplier(pixel));
    }

    ///////
    // test code
    /////// 

    // (not well designed)

    void execute(const color_supplier& supplier, std::size_t threads, sf::RenderWindow& target, sf::RenderStates states = sf::RenderStates::Default)
    {
        if (threads == 0)
            return execute(supplier, target, states);
        
        auto multi_supplier = [&](auto who)
        {
            for (; who < width * height; who += threads)
            {
                auto pixel = sf::Vector2u( who % width, who / width);
                set_color(pixel, supplier(pixel));
            }
        };

        std::vector<std::thread> pool;
        for (int who = 0; who < threads; ++who)
            pool.emplace_back(multi_supplier, who);

        for (auto& thread : pool)
        {
            while (thread.joinable())
            {
                target.draw(*this, states);
                target.display();

                sf::sleep(sf::milliseconds(100));
            }
            thread.join();
        }
        
    }

    void execute(const color_supplier& supplier, sf::RenderWindow& target, sf::RenderStates states = sf::RenderStates::Default)
    {
        sf::Vector2u pixel = { 0, 0 };
        for (; pixel.y < height; ++pixel.y)
        {
            for (pixel.x = 0; pixel.x < width; ++pixel.x)
                set_color(pixel, supplier(pixel));

            target.draw(*this, states);
            target.display();
        }
    }

};

#endif