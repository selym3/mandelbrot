#ifndef __PIXELS_HPP__
#define __PIXELS_HPP__

#include <SFML/Graphics.hpp>
#include <vector>

class Pixels : public sf::Drawable
{
    /////////////////
    // BUFFER DATA //
    /////////////////

    std::vector<sf::Uint8> _data;

    std::size_t _get_size() const 
    { return _width * _height * 4; }

    std::size_t _get_index(const std::size_t x, const std::size_t y) const
    { return (( y * _width ) + x) * 4; }

public:
    ///////////////////
    // CREATE BUFFER //
    ///////////////////

    const std::size_t _width, _height;

    Pixels(std::size_t width, std::size_t height) :
        _width { width }, _height { height }
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

    // DOES NOT SUPPORT ANY RENDER STATES 
    void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        sf::Image image;
        image.create(_width, _height, &_data[0]);

        sf::Texture texture;
        texture.loadFromImage(image);

        sf::Sprite sprite;
        sprite.setTexture(texture, true);

        target.draw(sprite);
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
    
    void set_color(const sf::Vector2u& pixel, const sf::Color& color)
    { set_color(pixel.x, pixel.y, color); }

    void set_color(std::size_t x, std::size_t y, sf::Uint32 rgba)
    { set_color(x, y, sf::Color(rgba)); }

    void set_color(const sf::Vector2u& pixel, sf::Uint32 rgba)
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

    sf::Color get_color(const sf::Vector2u& pixel) const
    { return get_color(pixel.x, pixel.y); }

};

#endif