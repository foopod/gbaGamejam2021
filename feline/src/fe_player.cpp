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

#include "fe_hitbox.h"
#include "fe_player.h"
#include "fe_elevator.h"

namespace fe
{
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

    bool check_collisions_map(bn::fixed_point pos, fe::Hitbox hitbox, bn::affine_bg_ptr& map)
    {
        int l = pos.x().integer() - hitbox.width() / 2 + hitbox.x();
        int r = pos.x().integer() + hitbox.width() / 2 + hitbox.x();
        int u = pos.y().integer() - hitbox.height() / 2 + hitbox.y();
        int d = pos.y().integer() + hitbox.height() / 2 + hitbox.y();

        if((get_map_cell(l, u, map) > 0 && get_map_cell(l, u, map) < 17) ||
        (get_map_cell(l, d, map) > 0 && get_map_cell(l, d, map) < 17) ||
        (get_map_cell(r, u, map) > 0 && get_map_cell(r, u, map) < 17) ||
        (get_map_cell(r, d, map) > 0 && get_map_cell(r, d, map) < 17)){
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
    constexpr const bn::fixed gravity = 0.3;
    constexpr const bn::fixed jump_power = 3.5;
    constexpr const bn::fixed acc = 0.3;
    constexpr const bn::fixed max_dy = 3;
    constexpr const bn::fixed friction = 0.85;

    Player::Player(bn::fixed_point pos, bn::sprite_ptr sprite, bn::camera_ptr camera) :
        _pos(pos), _sprite(sprite), _camera(camera), _hitbox_fall(0,4,4,0), _hitbox_left(-2,0,2,4), _hitbox_right(2,0,2,4)
    {}

    bn::fixed_point Player::pos()
    {
        return _pos;
    }

    void Player::jump()
    {
        if(_grounded){
            _dy-= jump_power;
            _grounded = false;
        }
    }

    void Player::collide_with_objects(bn::affine_bg_ptr map, fe::Elevator elevator){
        // if falling
        if(_dy > 0){
            _falling = true;
            _grounded = false;
            _jumping = false;
            
            // clamp max fall speed
            if(_dy > max_dy){
                _dy = max_dy;
            }

            if(check_collisions_elevator(_pos, _hitbox_fall, elevator, true))
            {
                _grounded = true;
                _falling = false;
                _dy = 0;
                _pos.set_y(elevator.top_y() -4);
            }
            else if(check_collisions_elevator(_pos, _hitbox_fall, elevator, false))
            {
                _grounded = true;
                _falling = false;
                _dy = 0;
                _pos.set_y(elevator.bottom_y() - 4);
            }
            else if(check_collisions_map(_pos, _hitbox_fall, map))
            {
                _grounded = true;
                _falling = false;
                _dy = 0;
                _pos.set_y(_pos.y() - modulo(_pos.y() + 4,8));
                if(check_collisions_map(_pos, fe::Hitbox(0,2,2,1), map)){
                    _pos.set_y(_pos.y() - 8);
                }
                //todo if they pressed jump a few milliseconds before hitting the ground then jump now
            }
            else
            {
                //falling
            }
        } 
        else if(_dy < 0) // jumping
        {
            _jumping = true;
            //todo collide upward maybe
        }

        if(_dx > 0) // moving right
        {
            if(check_collisions_map(_pos, _hitbox_right, map)){
                if(_grounded){
                    _dx = 0;
                }  
            }
        } 
        else if (_dx < 0) // moving left
        {
            if(check_collisions_map(_pos, _hitbox_left, map)){
                if(_grounded){
                    _dx = 0;
                }
            }
        }
    }

    void Player::move_right(){
        _sprite.set_horizontal_flip(false);
        _dx+= acc;
        _running = true;
    }

    void Player::move_left(){
        _sprite.set_horizontal_flip(true);
        _dx-= acc;
        _running = true;
    }

    void Player::apply_animation_state(){
        if(_jumping){
            _action = bn::create_sprite_animate_action_forever(
                            _sprite, 6, bn::sprite_items::cat.tiles_item(), 9,9,9,9,9,9,9,9,9);
            _already_running = false;
        } else if(_falling){
            _action = bn::create_sprite_animate_action_forever(
                            _sprite, 6, bn::sprite_items::cat.tiles_item(), 10,10,10,10,10,10,10,10,10);
            _already_running = false;
        } else if(_sliding){
            _action = bn::create_sprite_animate_action_forever(
                            _sprite, 6, bn::sprite_items::cat.tiles_item(), 8,8,8,8,8,8,8,8,8);
            _already_running = false;
        } else if(_running){
            if(!_already_running){
                _action = bn::create_sprite_animate_action_forever(
                        _sprite, 6, bn::sprite_items::cat.tiles_item(), 1, 2, 3, 4, 5, 6, 7, 8, 9);
                _already_running = true;
            }
        } else {
            //idle
            _action = bn::create_sprite_animate_action_forever(
                    _sprite, 6, bn::sprite_items::cat.tiles_item(), 0,0,0,0,0,0,0,0,0);
            _already_running = false;
        }

        // dashing
        if(_dy < -3.5)
        {   
            _action = bn::create_sprite_animate_action_forever(
                    _sprite, 6, bn::sprite_items::cat.tiles_item(), 12,12,12,12,12,12,12,12,12);    
        }
        else if(bn::abs(_dx) > 2)
        {
            _action = bn::create_sprite_animate_action_forever(
                    _sprite, 6, bn::sprite_items::cat.tiles_item(), 11,11,11,11,11,11,11,11,11);
        }
        
        _action.update();
    }

    void Player::update_position(bn::affine_bg_ptr map, fe::Elevator elevator){
        // apply friction
        _dx = _dx * friction;

        //apply gravity
        _dy+= gravity;

        // take input
        if(bn::keypad::left_held())
        {
            move_left();
        } 
        else if(bn::keypad::right_held())
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
        if(bn::keypad::b_pressed())
        {
            if(bn::keypad::up_held())
            {
                _dy = -5;
            }
            else if(_sprite.horizontal_flip())
            {
                _dx = -4;
                _dy = -0.5;
            }
            else
            {
                _dx = 4;
                _dy = -0.5;
            }
        } 

        // collide
        collide_with_objects(map, elevator);
        
        // update position
        _pos.set_x(_pos.x() + _dx);
        _pos.set_y(_pos.y() + _dy);

        // lock player position to map limits x
        if(_pos.x() > 508){
            _pos.set_x(508);
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
        } else if (_pos.x() > 412-20){
            _camera.set_x(412-20);
        }
        else 
        {
            _camera.set_x(_pos.x());
        }

        // update camera y
        if(_pos.y() < 432){
            _camera.set_y(_pos.y());
        } else {
            _camera.set_y(432);
        }
        
    }
}
