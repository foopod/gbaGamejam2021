#include "bn_core.h"
#include "bn_log.h"
#include "bn_sprite_builder.h"
#include "bn_sprite_ptr.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_regular_bg_ptr.h"
#include "bn_fixed_point.h"
#include "bn_regular_bg_item.h"
#include "bn_affine_bg_ptr.h"
#include "bn_affine_bg_item.h"
#include "bn_affine_bg_tiles_ptr.h"
#include "bn_affine_bg_map_ptr.h"
#include "bn_camera_actions.h"
#include "bn_sprite_animate_actions.h"

#include "bn_sprite_items_cat.h"
#include "bn_affine_bg_items_map.h"
#include "bn_affine_bg_items_map1.h"
#include "bn_regular_bg_items_map2.h"
#include "bn_music_items.h"

#include "bn_string_view.h"
#include "bn_vector.h"
#include "bn_sprite_text_generator.h"
#include "variable_8x16_sprite_font.h"

namespace
{
    //constants
    constexpr const bn::fixed gravity = 0.2;
    constexpr const bn::fixed jump_power = 5;
    constexpr const bn::fixed acc = 0.2;
    constexpr const bn::fixed max_dx = 2;
    constexpr const bn::fixed max_dy = 2;
    constexpr const bn::fixed friction = 0.9;

     // text set up
    bn::sprite_text_generator text_generator(variable_8x16_sprite_font);
    //text_generator.set_center_alignment();
    bn::vector<bn::sprite_ptr, 32> text_sprites;

    class Hitbox

    {
        private:
            int _x;
            int _y;
            int _width;
            int _height;
        public:
            Hitbox(int x, int y, int width, int height)
                : _x(x), _y(y), _width(width), _height(height)
            {}
            int x(){
                return _x;
            }
            int y(){
                return _y;
            }
            int width(){
                return _width;
            }
            int height(){
                return _height;
            }

    };

    bn::fixed modulo(bn::fixed a, bn::fixed m)
    {
        return a - m * ((a/m).right_shift_integer());
    }

    int get_map_cell(bn::fixed x, bn::fixed y, bn::affine_bg_ptr& map)
    {
        int map_size = map.dimensions().width();
        int cell =  modulo((y.safe_division(8).right_shift_integer() * map_size/8 + x/8), map_size*8).integer();
        return map.map().cells_ref().value().at(cell);
    }

    bool check_fall_collisions(bn::fixed_point pos, Hitbox hitbox, bn::affine_bg_ptr& map)
    {
        int l = pos.x().integer() - hitbox.width() / 2 + hitbox.x();
        int r = pos.x().integer() + hitbox.width() / 2 + hitbox.x();
        int u = pos.y().integer() - hitbox.height() / 2 + hitbox.y();
        int d = pos.y().integer() + hitbox.height() / 2 + hitbox.y();

        //display current tile
        text_sprites.clear();
        text_generator.generate(0, -40, 
        bn::to_string<32>(get_map_cell(l, u, map)) + " " +
        bn::to_string<32>(get_map_cell(l, d, map)) + " " +
        bn::to_string<32>(get_map_cell(r, u, map)) + " " +
        bn::to_string<32>(get_map_cell(r, d, map)), text_sprites);

        if((get_map_cell(l, u, map) > 0 && get_map_cell(l, u, map) < 13) ||
        (get_map_cell(l, d, map) > 0 && get_map_cell(l, d, map) < 13) ||
        (get_map_cell(r, u, map) > 0 && get_map_cell(r, u, map) < 13) ||
        (get_map_cell(r, d, map) > 0 && get_map_cell(r, d, map) < 13)){
            return true;
        } else {
            return false;
        }
    }
    
    class Player
    {
        public:
            Player(bn::fixed_point pos, bn::sprite_ptr sprite, bn::camera_ptr camera)
            : _pos(pos), _sprite(sprite), _camera(camera), _hitbox(0,3,4,2)
            {
                
            }

            bn::fixed_point pos(){
                return _pos;
            }

            Hitbox hitbox(){
                return _hitbox;
            }

            void jump(){
                _dy-= jump_power;
            }

            void fall(bn::affine_bg_ptr map){
                if(check_fall_collisions(_pos, _hitbox, map)){
                    if(_dy > 0)
                    {
                        _dy = 0;
                        if(!_grounded){
                            _pos.set_y(_pos.y() - modulo(_pos.y() + 4,8));
                            _grounded = true;
                        }
                    }
                } else {
                    _dy+= gravity;
                    _grounded = false;
                }
                
            }

            void move_right(){
                _sprite.set_horizontal_flip(false);
                _dx+= acc;
            }

            void move_left(){
                _sprite.set_horizontal_flip(true);
                _dx-= acc;
            }

            void update_position(bn::affine_bg_ptr map){
                // take input
                if(bn::keypad::left_held())
                {
                    move_left();
                } 
                else if(bn::keypad::right_held())
                {
                    move_right();
                }

                // fall (and eventually check falling colliders)
                fall(map);
                
                // jump
                if(bn::keypad::up_pressed())
                {
                    jump();
                } 

                // apply friction
                _dx = _dx * friction;
                _dy = _dy * friction;

                //apply limits
                if( _dx > max_dx){
                    _dx = max_dx;
                }
                if( _dy > max_dy){
                    _dy = max_dy;
                }
                
                // update position
                _pos.set_x(_pos.x() + _dx);
                _pos.set_y(_pos.y() + _dy);

                // update sprite
                _sprite.set_x(_pos.x());
                _sprite.set_y(_pos.y());

                // update camera
                _camera.set_x(_pos.x());
            }

        private: 
            bn::fixed _dx;
            bn::fixed _dy;
            bn::fixed_point _pos;
            bn::sprite_ptr _sprite;
            bn::camera_ptr _camera;
            bool _grounded;
            Hitbox _hitbox;
    };
}

int main()
{
    bn::core::init();

    bn::fixed_point init_pos = bn::fixed_point(60, 432);

    // current cell player is on
    bn::fixed current_cell = 0;

    // player sprite
    bn::sprite_ptr cat_sprite = bn::sprite_items::cat.create_sprite(init_pos.x(), init_pos.y());
    bn::sprite_animate_action<9> action = bn::create_sprite_animate_action_forever(
                    cat_sprite, 6, bn::sprite_items::cat.tiles_item(), 1, 2, 3, 4, 5, 6, 7, 8, 9);
    
    // map
    bn::affine_bg_ptr map_bg = bn::affine_bg_items::map.create_bg(256, 256);

    bn::camera_ptr camera = bn::camera_ptr::create(init_pos.x(), init_pos.y());

    cat_sprite.set_camera(camera);
    map_bg.set_camera(camera);
    Player player = Player(init_pos, cat_sprite, camera);

   
    bn::music_items::song.play(0.5);
    
    while(true)
    {
        //cat_sprite.set_position(apply_gravity(cat_sprite.position()));
        //calc_position()
        //check_collisions(cat_sprite, map_bg);

    /**
        if(bn::keypad::left_held())
        {
            cat_sprite.set_horizontal_flip(true);
            cat_sprite.set_x(cat_sprite.x() - 1);
            camera.set_x(camera.x() - 1);
            
        }
        else if(bn::keypad::right_held())
        {
            cat_sprite.set_horizontal_flip(false);
            cat_sprite.set_x(cat_sprite.x() + 1);
            camera.set_x(camera.x() + 1);
        }

        if(bn::keypad::up_held())
        {
            cat_sprite.set_y(cat_sprite.y() - 1);
            //camera.set_y(camera.y() - 1);
        }
        else if(bn::keypad::down_held())
        {
            cat_sprite.set_y(cat_sprite.y() + 1);
            //camera.set_y(camera.y() + 1);
        }
*/
        player.update_position(map_bg);
        
        //cat_sprite.set_camera(camera);
        //map_bg.set_camera(camera);
        
        action.update();
        bn::core::update();
    }
}
