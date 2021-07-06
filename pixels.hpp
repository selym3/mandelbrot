#ifndef __PIXELS_HPP__
#define __PIXELS_HPP__

#include <SFML/Graphics.hpp>
#include <vector>

class Pixels : public sf::Drawable, public sf::Transformable
{
    /////////////////
    // BUFFER DATA //
    /////////////////

    std::size_t _width, _height;
    std::vector<sf::Uint8> _data;

    std::size_t _get_size() const 
    { return _width * _height * 4; }

    std::size_t _get_index(const std::size_t x, const std::size_t y) const
    { return (( y * _width ) + x) * 4; }

public:
    ///////////////////
    // CREATE BUFFER //
    ///////////////////

    Pixels(std::size_t _width, std::size_t _height) : 
        _width { 0 }, _height { 0 } // <-- set_size will handle this
    {
        set_size(_width, _height);
    }

    Pixels(const sf::Vector2u& size) : 
        Pixels(size.x, size.y)
    {
    }

    Pixels(const sf::VideoMode& vm) :
        Pixels(vm.width, vm.height)
    {
    }

    ////////////////////
    // READ META DATA //
    ////////////////////

    std::size_t get_width() const
    { return _width; }

    std::size_t get_height() const
    { return _height; }

    sf::Vector2<std::size_t> get_size() const
    { return { get_width(), get_height() }; }

    /////////////////////
    // WRITE META DATA //
    /////////////////////

    template <typename T>
    bool set_size(const sf::Vector2<T>& size)
    {
        const std::size_t old_size = _get_size();

        _width = size.x, _height = size.y;
        const std::size_t new_size = _get_size();

        if (new_size > old_size)
        {
            _data.reserve(new_size);
            return true;
        }

        return false;
    }

    bool set_size(std::size_t width, std::size_t height)
    { return set_size(sf::Vector2<std::size_t>{ width, height }); }

    bool set_width(std::size_t width) 
    { return set_size(sf::Vector2<std::size_t>{ width, get_height() }); }

    bool set_height(std::size_t height) 
    { return set_size(sf::Vector2<std::size_t>{ get_width(), height }); }

    /////////////////
    // DRAW PIXELS //
    /////////////////

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override 
    {
        sf::Texture texture;
        texture.create(_width, _height);
        texture.update(_data.data());

        sf::Sprite sprite(texture);

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

};

#endif