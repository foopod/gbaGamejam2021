#ifndef BF_ENEMY_H
#define BF_ENEMY_H

#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_optional.h"
#include "bn_span.h"
#include "bn_string.h"
#include "bn_string_view.h"
#include "bn_sprite_animate_actions.h"

#include "fe_enemy_type.h"
#include "fe_hitbox.h"

namespace fe
{
    class Enemy
    {
        private:
            bn::fixed_point _pos;
            bn::camera_ptr _camera;
            ENEMY_TYPE _type;
            int _dir;
            int _hp;
            bool _stunned = false;
            bn::optional<bn::sprite_ptr> _sprite;
            bn::optional<bn::sprite_animate_action<2>> _action;

        public:
            Enemy(int x, int y, bn::camera_ptr camera, ENEMY_TYPE type, int hp);
            void update();
            bn::fixed_point pos();
            bool is_hit(Hitbox attack);
            bool damage_from_left(int damage);
            bool damage_from_right(int damage);
            int hp();
    };
}

#endif