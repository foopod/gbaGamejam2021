#include "fe_hitbox.h"

namespace fe
{
    Hitbox::Hitbox(int x, int y, int width, int height) : 
        _x(x), _y(y), _width(width), _height(height)
    {}

    int Hitbox::x()
    {
        return _x;
    }

    int Hitbox::y()
    {
        return _y;
    }

    int Hitbox::width()
    {
        return _width;
    }

    int Hitbox::height()
    {
        return _height;
    }
}