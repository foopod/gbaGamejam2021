#ifndef FE_PLAYER_H
#define FE_PLAYER_H

#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_affine_bg_ptr.h"

#include "fe_level.h"
#include "fe_hitbox.h"

#include "bn_sprite_items_cat.h"

namespace fe
{
    class Player
    {
        private:
            bn::fixed _dx;
            bn::fixed _dy;
            bn::fixed_point _pos;
            bn::sprite_ptr _sprite;
            bn::sprite_ptr _text_bg1;
            bn::sprite_ptr _text_bg2;
            bn::camera_ptr _camera;
            bool _jumping = false;
            bool _falling = false;
            bool _running = false;
            bool _listening = false;
            bool _grounded = false;
            bool _sliding = false;
            bool _already_running = false;
            fe::Hitbox _hitbox_fall;
            fe::Hitbox _hitbox_left;
            fe::Hitbox _hitbox_right;
            fe::Hitbox _hitbox_jump;
            bn::sprite_animate_action<9> _action = bn::create_sprite_animate_action_forever(
                        _sprite, 6, bn::sprite_items::cat.tiles_item(), 0,0,0,0,0,0,0,0,0);

        public:
            Player(bn::fixed_point pos, bn::sprite_ptr sprite, bn::camera_ptr camera);
            
            [[nodiscard]] bn::fixed_point pos();

            void jump();
            void collide_with_objects(bn::affine_bg_ptr map, fe::Level level);
            void move_right();
            void move_left();
            void set_listening(bool is_listening);
            void apply_animation_state();
            void update_position(bn::affine_bg_ptr map, fe::Level level);
    };
}

#endif