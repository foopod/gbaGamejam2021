#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_affine_bg_ptr.h"
#include "bn_affine_bg_map_ptr.h"
#include "bn_string.h"
#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_size.h"
#include "bn_optional.h"
#include "bn_span.h"
#include "bn_log.h"

#include "bn_sprite_items_cat.h"
#include "bn_sprite_items_text_bg.h"

#include "fe_hitbox.h"
#include "fe_player.h"
#include "fe_elevator.h"
#include "fe_level.h"

namespace fe
{
    enum directions{up, down, left, right};

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

    bool contains_cell(int tile, bn::vector<int, 32> tiles)
    {
        for(int index = 0; index < tiles.size(); ++index)
        {
            if(tiles.at(index) == tile)
            {
                return true;
            }
        }
        return false;
    }

    bool check_collisions_map(bn::fixed_point pos, directions direction, Hitbox hitbox,bn::affine_bg_ptr& map, fe::Level level)
    {
        int l = pos.x().integer()/2 - hitbox.width() / 2 + hitbox.x();
        int r = pos.x().integer()/2 + hitbox.width() / 2 + hitbox.x();
        int u = pos.y().integer()/2 - hitbox.height() / 2 + hitbox.y();
        int d = pos.y().integer()/2 + hitbox.height() / 2 + hitbox.y();
        
        bn::vector<int, 32> tiles;
        if(direction == down){
            tiles = level.floor_tiles();
        } else if(direction == left || direction == right){
            tiles = level.wall_tiles();
        } else if(direction == up){
            tiles = level.ceil_tiles();
        }

        if(contains_cell(get_map_cell(l, u, map), tiles) ||
        contains_cell(get_map_cell(l, d, map), tiles) ||
        contains_cell(get_map_cell(r, u, map), tiles) ||
        contains_cell(get_map_cell(l, d, map), tiles)){
            return true;
        } else {
            return false;
        }
    }

    bool check_collisions_elevator(bn::fixed_point pos, fe::Hitbox hitbox, fe::Elevator elevator, bool is_top)
    {
        int l = pos.x().integer() - hitbox.width() / 2 + hitbox.x();
        int r = pos.x().integer() + hitbox.width() / 2 + hitbox.x();
        int u = pos.y().integer() - hitbox.height() / 2 + hitbox.y() + 2;
        int d = pos.y().integer() + hitbox.height() / 2 + hitbox.y();
        
        // align left and right     
        if((r > (elevator.pos().x().integer() -16)  && r < (elevator.pos().x().integer()+16))
            || (l < (elevator.pos().x().integer()+16) && l > (elevator.pos().x().integer() -16)))
        { 
            if(is_top) //collide top of balloon
            {
                if((u > elevator.top_y() && u < elevator.top_y() + 8)
                    || (d > elevator.top_y() && d < elevator.top_y() + 8))
                {
                    return true;
                }
            } else {//collide bottom of balloon
                if((u > elevator.bottom_y() && u < elevator.bottom_y() + 8)
                    || (d > elevator.bottom_y() && d < elevator.bottom_y() + 8)) // collide bottom
                {
                    return true;
                }
            }
            
        }
        return false;

    }

    //constants
    constexpr const bn::fixed gravity = 0.5;
    constexpr const bn::fixed jump_power = 7;
    constexpr const bn::fixed acc = 0.4;
    constexpr const bn::fixed max_dy = 6;
    constexpr const bn::fixed friction = 0.85;

    Player::Player(bn::fixed_point pos, bn::sprite_ptr sprite, bn::camera_ptr camera) :
        _pos(pos), _sprite(sprite), _camera(camera), _hitbox_fall(0,4,4,0), _hitbox_left(-2,0,2,4), _hitbox_right(2,0,2,4), _hitbox_jump(0,2,4,2),
        _text_bg1(bn::sprite_items::text_bg.create_sprite(0, 0)),_text_bg2(bn::sprite_items::text_bg.create_sprite(0, 0))
    {
        _sprite.set_horizontal_scale(2);
        _sprite.set_vertical_scale(2);
        _sprite.put_above();

        _text_bg1.set_scale(2);
        _text_bg1.set_bg_priority(0);
        _text_bg1.put_above();
        _text_bg1.set_camera(_camera);
        _text_bg2.set_scale(2);
        _text_bg2.set_bg_priority(0);
        _text_bg2.put_above();
        _text_bg2.set_camera(_camera);
    }

    bn::fixed_point Player::pos()
    {
        return _pos;
    }

    void Player::set_listening(bool is_listening)
    {
        _listening = is_listening;
        _text_bg1.set_visible(_listening);
        _text_bg2.set_visible(_listening);
        _text_bg1.set_position(_camera.x()+64+8, _camera.y() + 40+24);
        _text_bg2.set_position(_camera.x()-64+8, _camera.y() + 40+24);
    }

    void Player::jump()
    {
        if(_grounded && !_listening){
            _dy-= jump_power;
            _grounded = false;
        }
    }

    void Player::collide_with_objects(bn::affine_bg_ptr map, fe::Level level){
        // if falling
        if(_dy > 0){
            _falling = true;
            _grounded = false;
            _jumping = false;
            
            // clamp max fall speed
            if(_dy > max_dy){
                _dy = max_dy;
            }

            // if(check_collisions_elevator(_pos, _hitbox_fall, elevator, true))
            // {
            //     _grounded = true;
            //     _falling = false;
            //     _dy = 0;
            //     _pos.set_y(elevator.top_y() -4);
            // }
            // else if(check_collisions_elevator(_pos, _hitbox_fall, elevator, false))
            // {
            //     _grounded = true;
            //     _falling = false;
            //     _dy = 0;
            //     _pos.set_y(elevator.bottom_y() - 4);
            // }
            // else 
            if(check_collisions_map(_pos, down, _hitbox_fall, map, level))
            {
                _grounded = true;
                _falling = false;
                _dy = 0;
                _pos.set_y(_pos.y() - modulo(_pos.y() + 8,16));
                //todo if they pressed jump a few milliseconds before hitting the ground then jump now
            }
        } 
        else if(_dy < 0) // jumping
        {
            _jumping = true;
            if(check_collisions_map(_pos, up, _hitbox_jump, map, level))
            {
                _dy = 0;
            }
            //todo collide upward maybe
        }

        if(_dx > 0) // moving right
        {
            if(check_collisions_map(_pos, right,_hitbox_right, map, level)){
                _dx = 0;
            }
        } 
        else if (_dx < 0) // moving left
        {
            if(check_collisions_map(_pos, left, _hitbox_left, map, level)){
                _dx = 0;
            }
        }
    }

    void Player::move_right(){
        _sprite.set_horizontal_flip(false);
        _dx+= acc;
        _running = true;
        _sliding = false;
    }

    void Player::move_left(){
        _sprite.set_horizontal_flip(true);
        _dx-= acc;
        _running = true;
        _sliding = false;
    }

    void Player::apply_animation_state(){
        if(_jumping){
            _action = bn::create_sprite_animate_action_once(
                            _sprite, 6, bn::sprite_items::cat.tiles_item(), 9,9,9,9,9,9,9,9,9);
        } else if(_falling){
            _action = bn::create_sprite_animate_action_once(
                            _sprite, 6, bn::sprite_items::cat.tiles_item(), 10,10,10,10,10,10,10,10,10);
        } else if(_sliding){
            _action = bn::create_sprite_animate_action_once(
                            _sprite, 6, bn::sprite_items::cat.tiles_item(), 8,8,8,8,8,8,8,8,8);
        } else if(_running){
            if(_action.graphics_indexes().front() != 1){
                _action = bn::create_sprite_animate_action_forever(
                        _sprite, 4, bn::sprite_items::cat.tiles_item(), 1, 2, 3, 4, 5, 6, 7, 8, 9);
            }
        } else {
            //idle
            _action = bn::create_sprite_animate_action_once(
                    _sprite, 6, bn::sprite_items::cat.tiles_item(), 0,0,0,0,0,0,0,0,0);
        }

        // dashing
        // if(_dy < -8)
        // {   
        //     _action = bn::create_sprite_animate_action_forever(
        //             _sprite, 6, bn::sprite_items::cat.tiles_item(), 12,12,12,12,12,12,12,12,12);    
        // }
        // else if(bn::abs(_dx) > 3)
        // {
        //     _action = bn::create_sprite_animate_action_forever(
        //             _sprite, 6, bn::sprite_items::cat.tiles_item(), 11,11,11,11,11,11,11,11,11);
        // }
        //BN_LOG("running: " + bn::to_string<32>(_running));
        
        _action.update();
    }

    void Player::update_position(bn::affine_bg_ptr map, fe::Level level){
        // apply friction
        _dx = _dx * friction;

        //apply gravity
        _dy+= gravity;

        // take input
        if(bn::keypad::left_held() && !_listening)
        {
            move_left();
        } 
        else if(bn::keypad::right_held() && !_listening)
        {
            move_right();
        }
        else if(_running) //slide to a stop
        {
            if(!_falling & !_jumping){
                _sliding = true;
                _running = false;
            }
        } 
        else if (_sliding) //stop sliding
        {
            if (bn::abs(_dx) < 0.1 || _running){
                _sliding = false;
            }
        } 

        // jump
        if(bn::keypad::a_pressed())
        {
            jump();
        } 

        // dash
        // if(bn::keypad::b_pressed())
        // {
        //     if(bn::keypad::up_held())
        //     {
        //         _dy = -9;
        //     }
        //     else if(_sprite.horizontal_flip())
        //     {
        //         _dx = -15;
        //         _dy = -1;
        //     }
        //     else
        //     {
        //         _dx = 15;
        //         _dy = -1;
        //     }
        // } 

        // collide
        collide_with_objects(map, level);
        
        // update position
        _pos.set_x(_pos.x() + _dx);
        _pos.set_y(_pos.y() + _dy);

        // lock player position to map limits x
        if(_pos.x() > 1020){
            _pos.set_x(1020);
        } else if(_pos.x() < 4){
            _pos.set_x(4);
        }

        // update sprite position
        _sprite.set_x(_pos.x());
        _sprite.set_y(_pos.y());

        // update camera x
        if(_pos.x() < 120)
       {
            _camera.set_x(120);
        } else if (_pos.x() > 924-20){
            _camera.set_x(924-20);
        }
        else 
        {
            _camera.set_x(_pos.x());
        }

        if(_pos.y() < 944){
            _camera.set_y(_pos.y());
        } else {
            _camera.set_y(944);
        }

    }
}
