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
#include "fe_extras.h"
#include "fe_enemy.h"
#include "fe_enemy_type.h"

#include "bn_affine_bg_items_house.h"

namespace fe
{
    enum directions{up, down, left, right};

    [[nodiscard]] int get_map_cell(bn::fixed x, bn::fixed y, bn::affine_bg_ptr& map, bn::span<const bn::affine_bg_map_cell> cells)
    {
        int map_size = map.dimensions().width();
        int cell =  modulo((y.safe_division(8).right_shift_integer() * map_size/8 + x/8), map_size*8).integer();
        return cells.at(cell);
    }

    [[nodiscard]] bool contains_cell(int tile, bn::vector<int, 32> tiles)
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

    [[nodiscard]] bool check_collisions_map(bn::fixed_point pos, directions direction, Hitbox hitbox,bn::affine_bg_ptr& map, fe::Level level, bn::span<const bn::affine_bg_map_cell> cells)
    {
        bn::fixed l = pos.x() - hitbox.width() / 2 + hitbox.x();
        bn::fixed r = pos.x() + hitbox.width() / 2 + hitbox.x();
        bn::fixed u = pos.y() - hitbox.height() / 2 + hitbox.y();
        bn::fixed d = pos.y() + hitbox.height() / 2 + hitbox.y();
        
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

    //constants
    constexpr const bn::fixed gravity = 0.2;
    constexpr const bn::fixed wall_run_speed =0.25;
    constexpr const bn::fixed jump_power = 4;
    constexpr const bn::fixed acc = 0.4;
    constexpr const bn::fixed max_dy = 6;
    constexpr const bn::fixed friction = 0.85;

    Player::Player(bn::sprite_ptr sprite) :
        _sprite(sprite),
        _camera(bn::camera_ptr::create(0,0)),
        _map(bn::affine_bg_items::house.create_bg(0, 0)),
        _text_bg1(bn::sprite_items::text_bg.create_sprite(0, 0)),
        _text_bg2(bn::sprite_items::text_bg.create_sprite(0, 0)),
        _healthbar(fe::Healthbar())
    {
        _map.set_visible(false); // why can't I leave something uninitialised
        _sprite.put_above();

        _text_bg1.set_scale(2);
        _text_bg1.set_bg_priority(0);
        _text_bg1.put_above();
        _text_bg2.set_scale(2);
        _text_bg2.set_bg_priority(0);
        _text_bg2.put_above();
    }

    void Player::spawn(bn::fixed_point pos, bn::camera_ptr camera, bn::affine_bg_ptr map, bn::vector<Enemy,32>& enemies){
        _pos = pos;
        _camera = camera;
        _map = map;
        _map_cells = map.map().cells_ref().value();
        _enemies = &enemies;
        _map.set_visible(true);
        _sprite.set_visible(true);

        reset();
    }

    void Player::reset(){
        _sprite.set_camera(_camera);
        _sprite.set_bg_priority(1);
        _sprite.put_above();
        _text_bg1.set_camera(_camera);
        _text_bg2.set_camera(_camera);
        _update_camera(1);
        _dy = 0;
        _dy = 0;
        _jumping = false;
        _falling = false;
        _running = false;
        _listening = false;
        _grounded = false;
        _sliding = false;
        _wall_running = false;
        _wall_jumped = false;
        _already_running = false;
        _attacking = false;

        _can_wallrun = false;
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

    void Player::set_can_wallrun(bool can_wallrun)
    {
        _can_wallrun = can_wallrun;
    }

    bool Player::is_listening()
    {
        return _listening;
    }

    void Player::jump()
    {
        if(_grounded && !_listening){
            _dy-= jump_power;
            _grounded = false;
        } else if (_wall_running && !_wall_jumped){
            _dy-= jump_power;
            _wall_jumped = true;
        }
    }

    void Player::attack()
    {
        _attacking = true;
    }

    bool Player::is_right()
    {
        return !_sprite.horizontal_flip();
    }

    void Player::check_attack(){
        if(_attacking){
            Hitbox attack_hitbox = Hitbox(_pos.x(),_pos.y(), 20, 20);
            if(_sprite.horizontal_flip()){
                attack_hitbox.set_x(_pos.x() - 8);
            } else {
                attack_hitbox.set_x(_pos.x() + 8);
            }
            
            for(int i = 0; i < _enemies->size(); i++)
            {
                if(_enemies->at(i).is_hit(attack_hitbox))
                {
                    if(_sprite.horizontal_flip()){
                        _enemies->at(i).damage_from_left(1);
                    } else {
                        _enemies->at(i).damage_from_right(1);
                    }
                    
                }
            }
        }
        
    }

    void Player::collide_with_enemies(){
        Hitbox damage_hitbox = Hitbox(_pos.x(),_pos.y()+2, 8, 8);
        if(!_invulnerable){
            for(int i = 0; i < _enemies->size(); i++)
            {
                if(_enemies->at(i).is_hit(damage_hitbox))
                {
                    _invulnerable = true;
                    _healthbar.set_hp(_healthbar.hp() - 1);
                    _dy -= 0.3;
                    if(_sprite.horizontal_flip()){
                        _dx += 5;
                    } else {
                        _dx -= 5;
                    }
                }
            }
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
                _wall_jumped = false;
                _wall_running = false;
                _falling = false;
                _dy = 0;
                _pos.set_y(_pos.y() - modulo(_pos.y(),8));
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

        if(_attacking && _action.done()){
            _attacking = false;
        }
        _sprite.set_vertical_scale(1);
        if(_attacking){
            if(_action.graphics_indexes().front() != 14){
                _action = bn::create_sprite_animate_action_once(
                            _sprite, 1, bn::sprite_items::cat_sprite.tiles_item(), 14,14,14,14,14,14,14,14,15,15);
            }
        } else if(_jumping){
            _action = bn::create_sprite_animate_action_forever(
                            _sprite, 6, bn::sprite_items::cat_sprite.tiles_item(), 12,12,12,12,12,12,12,12,12,12);
        } else if(_wall_running){
            if(_action.graphics_indexes().front() != 8){
                _action = bn::create_sprite_animate_action_forever(
                            _sprite, 2.5, bn::sprite_items::cat_sprite.tiles_item(), 8, 9,10,11, 2, 3, 4, 5, 6,7);
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
            if(_dy > 0 && bn::abs(_dx) > 1 && !_wall_jumped && _can_wallrun){
                _wall_running = true;
                _dy = _dy / 2;
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

        // attack
        if(bn::keypad::b_pressed())
        {
            attack();
        } 

        check_attack();

        // collide
        collide_with_objects(map, level);

        // collide with enemies
        collide_with_enemies();
        
        // ouch
        if(_invulnerable){
            ++_inv_timer;
            if(modulo(_inv_timer/5, 2) == 0){
                _sprite.set_visible(true);
            } else {
                _sprite.set_visible(false);
            }
            if(_inv_timer > 120){
                _invulnerable = false;
                _inv_timer = 0;
                _sprite.set_visible(true);
            }
        }

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
