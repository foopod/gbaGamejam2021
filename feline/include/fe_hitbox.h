#ifndef FE_HITBOX_H
#define FE_HITBOX_H

#include "bn_fixed.h"
#include "bn_fixed_point.h"

namespace fe
{
    class Hitbox
    {
        private:
            bn::fixed_point _pos;
            bn::fixed _width;
            bn::fixed _height;

        public:
            Hitbox();
            Hitbox(bn::fixed x, bn::fixed y, bn::fixed width, bn::fixed height);

            bn::fixed x();
            bn::fixed y();
            bn::fixed width();
            bn::fixed height();
            bn::fixed_point pos();


            void set_x(bn::fixed x);
            void set_y(bn::fixed y);
            void set_width(bn::fixed width);
            void set_height(bn::fixed height);
    };
}

#endif