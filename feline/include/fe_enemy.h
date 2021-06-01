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
#include "bn_affine_bg_ptr.h"
#include "bn_span.h"
#include "bn_affine_bg_map_cell.h"

#include "fe_enemy_type.h"
#include "fe_hitbox.h"
#include "fe_level.h"

namespace fe
{
    class Enemy
    {
        private:
            bn::fixed_point _pos;
            bn::fixed _dy = 0;
            bn::fixed _dx = 0;
            bn::camera_ptr _camera;
            ENEMY_TYPE _type;
            int _dir;
            int _hp;
            int _direction_timer = 0;
            bool _invulnerable = false;
            bool _dead = false;
            bool _grounded = false;
            int _inv_timer = 0;
            bool _stunned = false;
            bn::optional<bn::sprite_ptr> _sprite;
            bn::optional<bn::sprite_animate_action<4>> _action;

            bn::span<const bn::affine_bg_map_cell> _map_cells;
            bn::affine_bg_ptr _map;
            Level _level;

            bool _take_damage(int damage);
            bool _will_hit_wall();
            bool _will_fall();

        public:
            Enemy(int x, int y, bn::camera_ptr camera, bn::affine_bg_ptr map, ENEMY_TYPE type, int hp);
            void update();
            bn::fixed_point pos();
            bool is_hit(Hitbox attack);
            void set_visible(bool visibility);
            bool damage_from_left(int damage);
            bool damage_from_right(int damage);
            int hp();
    };
}

#endif