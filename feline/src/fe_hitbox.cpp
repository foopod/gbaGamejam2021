#include "fe_hitbox.h"

namespace fe
{
    Hitbox::Hitbox(bn::fixed x, bn::fixed y, bn::fixed width, bn::fixed height) : 
        _pos(x, y), _width(width), _height(height)
    {}

    bn::fixed_point Hitbox::pos()
    {
        return _pos;
    }

    bn::fixed Hitbox::x()
    {
        return _pos.x();
    }

    bn::fixed Hitbox::y()
    {
        return _pos.y();
    }

    bn::fixed Hitbox::width()
    {
        return _width;
    }

    bn::fixed Hitbox::height()
    {
        return _height;
    }

    void Hitbox::set_x(bn::fixed x)
    {
        _pos.set_x(x);
    }

    void Hitbox::set_y(bn::fixed y)
    {
        _pos.set_y(y);
    }

    void Hitbox::set_width(bn::fixed width)
    {
        _width = width;
    }

    void Hitbox::set_height(bn::fixed height)
    {
        _height = height;
    }
}