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

#include "bn_sprite_items_cat_sprite.h"
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

    int get_map_cell(bn::fixed x, bn::fixed y, bn::affine_bg_ptr& map, bn::span<const bn::affine_bg_map_cell> cells)
    {
        int map_size = map.dimensions().width();
        int cell =  modulo((y.safe_division(8).right_shift_integer() * map_size/8 + x/8), map_size*8).integer();
        return cells.at(cell);
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

    bool check_collisions_map(bn::fixed_point pos, directions direction, Hitbox hitbox,bn::affine_bg_ptr& map, fe::Level level, bn::span<const bn::affine_bg_map_cell> cells)
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

        if(contains_cell(get_map_cell(l, u, map, cells), tiles) ||
        contains_cell(get_map_cell(l, d, map, cells), tiles) ||
        contains_cell(get_map_cell(r, u, map, cells), tiles) ||
        contains_cell(get_map_cell(l, d, map, cells), tiles)){
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
    constexpr const bn::fixed wall_run_speed =0.25;
    constexpr const bn::fixed jump_power = 7;
    constexpr const bn::fixed acc = 0.4;
    constexpr const bn::fixed max_dy = 6;
    constexpr const bn::fixed friction = 0.85;

    Player::Player(bn::fixed_point pos, bn::sprite_ptr sprite, bn::camera_ptr camera, bn::span<const bn::affine_bg_map_cell> map_cells) :
        _pos(pos), _sprite(sprite), _camera(camera), _hitbox_fall(0,4,4,0), _hitbox_left(-2,0,2,4), _hitbox_right(2,0,2,4), _hitbox_jump(0,2,4,2),
        _text_bg1(bn::sprite_items::text_bg.create_sprite(0, 0)),_text_bg2(bn::sprite_items::text_bg.create_sprite(0, 0)),
        _map_cells(map_cells)
    {
        _sprite.put_above();

        _text_bg1.set_scale(2);
        _text_bg1.set_bg_priority(0);
        _text_bg1.put_above();
        _text_bg1.set_camera(_camera);
        _text_bg2.set_scale(2);
        _text_bg2.set_bg_priority(0);
        _text_bg2.put_above();
        _text_bg2.set_camera(_camera);

        _update_camera(1);
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
    }

    void Player::jump()
    {
        if(_grounded && !_listening){
            _dy-= jump_power;
            _grounded = false;
        } else if (_wall_running){
            _dy-= jump_power;
        }
    }

    void Player::collide_with_objects(bn::affine_bg_ptr map, fe::Level level){
        // if falling
        if(_dy > 0){
            if(!_wall_running){
                _falling = true;
            } else {
                _falling = false;
            }
            _grounded = false;
            _jumping = false;
            
            // clamp max fall speed
            if(_dy > max_dy){
                _dy = max_dy;
            }

            if(check_collisions_map(_pos, down, _hitbox_fall, map, level, _map_cells))
            {
                _grounded = true;
                _wall_running = false;
                _falling = false;
                _dy = 0;
                _pos.set_y(_pos.y() - modulo(_pos.y() + 8,16));
                //todo if they pressed jump a few milliseconds before hitting the ground then jump now
            }
        } 
        else if(_dy < 0) // jumping
        {
            if(!_wall_running){
                _jumping = true;
            } else {
                _jumping = false;
            }
            
            if(check_collisions_map(_pos, up, _hitbox_jump, map, level, _map_cells))
            {
                _dy = 0;
                _wall_running = false;
            }
        }

        if(_dx > 0) // moving right
        {
            if(check_collisions_map(_pos, right,_hitbox_right, map, level, _map_cells)){
                _dx = 0;
                
            }
        } 
        else if (_dx < 0) // moving left
        {
            if(check_collisions_map(_pos, left, _hitbox_left, map, level, _map_cells)){
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
        _sprite.set_vertical_scale(1);
        if(_jumping){
            _action = bn::create_sprite_animate_action_forever(
                            _sprite, 6, bn::sprite_items::cat_sprite.tiles_item(), 12,12,12,12,12,12,12,12,12,12);
        } else if(_wall_running){
            if(_action.graphics_indexes().front() != 8){
                _action = bn::create_sprite_animate_action_forever(
                            _sprite, 6, bn::sprite_items::cat_sprite.tiles_item(), 8, 9,10,11, 2, 3, 4, 5, 6,7);
            }
            _sprite.set_vertical_scale(0.9);
        } else if(_falling){
            _action = bn::create_sprite_animate_action_forever(
                            _sprite, 6, bn::sprite_items::cat_sprite.tiles_item(), 13,13,13,13,13,13,13,13,13,13);
        } else if(_sliding){
            _action = bn::create_sprite_animate_action_forever(
                            _sprite, 6, bn::sprite_items::cat_sprite.tiles_item(), 6,6,6,6,6,6,6,6,6,6);
        } else if(_running){
            if(_action.graphics_indexes().front() != 8){
                _action = bn::create_sprite_animate_action_forever(
                        _sprite, 2.5, bn::sprite_items::cat_sprite.tiles_item(), 8, 9,10,11, 2, 3, 4, 5, 6,7);
            }
        } else {
            //idle
            if(_action.graphics_indexes().front() != 0){
                _action = bn::create_sprite_animate_action_forever(
                        _sprite, 30, bn::sprite_items::cat_sprite.tiles_item(), 0,1,0,1,0,1,0,1,0,1);
            }
        }

        _action.update();
    }

    void Player::_update_camera(int lerp){
        // update camera
        if(_pos.x() < 122+30)
        {
            _camera.set_x(_camera.x()+ (122-_camera.x()) /lerp);
        } else if (_pos.x() > 922-30){
            _camera.set_x(_camera.x()+ (922-20-_camera.x()) /lerp);
        }
        else
        {
            if(_sprite.horizontal_flip()){
                _camera.set_x(_camera.x()+ (_pos.x() - 30-_camera.x() + _dx*8) /lerp);
            } else {
                _camera.set_x(_camera.x()+ (_pos.x() +30 -_camera.x() + _dx*8) /lerp);
            }            
        }

        if(_pos.y() < 942){
            _camera.set_y(_camera.y()+ (_pos.y()-10-_camera.y()) /lerp);
        } else {
            _camera.set_y(_camera.y()+(942-_camera.y()) /lerp);
        }
    }

    void Player::update_position(bn::affine_bg_ptr map, fe::Level level){
        _update_camera(10);

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
        
        if(bn::keypad::up_held()){
            _wall_running = true;
            if(_dy > 0){
                _dy = _dy/2;
            }
            
        } else {
            _wall_running = false;
        }
        
        if(_listening){
            _text_bg1.set_position(_camera.x()+64+8, _camera.y() + 40+24);
            _text_bg2.set_position(_camera.x()-64+8, _camera.y() + 40+24);
        }

        // jump
        if(bn::keypad::a_pressed())
        {
            jump();
        } 

        // collide
        collide_with_objects(map, level);

        // update position
        _pos.set_x(_pos.x() + _dx);
        _pos.set_y(_pos.y() + _dy);

        // lock player position to map limits x
        if(_pos.x() > 1016){
            _pos.set_x(1016);
        } else if(_pos.x() < 4){
            _pos.set_x(4);
        }

        // update sprite position
        _sprite.set_x(_pos.x());
        _sprite.set_y(_pos.y());

    }
}
