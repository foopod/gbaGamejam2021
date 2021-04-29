#ifndef FE_ELEVATOR_H
#define FE_ELEVATOR_H

#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"

namespace fe
{
    class Elevator
    {
        private:
            bn::fixed_point _pos;
            bn::sprite_ptr _top_sprite;
            bn::sprite_ptr _bottom_sprite;
            int _dist;
            int _start_y;
            

        public:
            Elevator(bn::fixed_point pos, bn::sprite_ptr top_sprite, bn::sprite_ptr bottom_sprite, int dist);

            [[nodiscard]] bn::fixed_point pos();
            [[nodiscard]] int top_y();
            [[nodiscard]] int bottom_y();

            void update_position();
    };
}

#endif