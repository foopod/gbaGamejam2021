#ifndef FE_PLAYER_H
#define FE_PLAYER_H

#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_affine_bg_ptr.h"
#include "bn_span.h"
#include "bn_affine_bg_map_cell.h"

#include "fe_level.h"
#include "fe_hitbox.h"
#include "fe_enemy.h"
#include "fe_data.h"
#include "fe_healthbar.h"

#include "bn_sprite_items_cat_sprite.h"

namespace fe
{
    class Player
    {
        private:
            bn::sprite_ptr _sprite;
            bn::fixed _dx;
            bn::fixed _dy;
            bn::fixed_point _pos;
            bn::camera_ptr _camera;
            bn::sprite_ptr _text_bg1;
            bn::sprite_ptr _text_bg2;
            fe::Healthbar _healthbar;
            
            bool _can_wallrun = false;

            //used for state management
            bool _jumping = false;
            bool _falling = false;
            bool _running = false;
            bool _listening = false;
            bool _grounded = false;
            bool _sliding = false;
            bool _wall_running = false;
            bool _wall_jumped = false;
            bool _already_running = false;
            bool _attacking = false;

            bool _invulnerable = false;
            int _inv_timer = 0;

            bn::span<const bn::affine_bg_map_cell> _map_cells;
            bn::affine_bg_ptr _map;
            bn::vector<Enemy,32>* _enemies;

            fe::Hitbox _hitbox_fall = Hitbox(0,8,8,0);
            fe::Hitbox _hitbox_left = Hitbox(-4,0,4,8);
            fe::Hitbox _hitbox_right = Hitbox(4,0,4,8);
            fe::Hitbox _hitbox_jump = Hitbox(0,4,8,4);
            bn::sprite_animate_action<10> _action = bn::create_sprite_animate_action_forever(
                        _sprite, 6, bn::sprite_items::cat_sprite.tiles_item(), 0,1,0,1,0,1,0,1,0,1);
            void _update_camera(int lerp);
            

        public:
            Player(bn::sprite_ptr sprite);

            Data data;
            
            [[nodiscard]] bn::fixed_point pos();

            void jump();
            void attack();
            void collide_with_objects(bn::affine_bg_ptr map, fe::Level level);
            void collide_with_enemies();
            void move_right();
            void move_left();
            void check_attack();
            void set_listening(bool is_listening);
            bool is_listening();
            void set_can_wallrun(bool can_wallrun);
            void apply_animation_state();
            void update_position(bn::affine_bg_ptr map, fe::Level level);

            void spawn(bn::fixed_point pos, bn::camera_ptr camera, bn::affine_bg_ptr map, bn::vector<Enemy,32>& enemies);
            void reset();

            bool is_right();
    };
}

#endif