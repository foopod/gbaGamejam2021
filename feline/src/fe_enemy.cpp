#include "fe_enemy.h"
#include "fe_enemy_type.h"
#include "fe_hitbox.h"
#include "fe_collision.h"

#include "bn_optional.h"
#include "bn_math.h"
#include "bn_log.h"
#include "bn_display.h"
#include "bn_keypad.h"
#include "bn_sprite_items_bat_sprite.h"
#include "bn_sprite_items_slime_sprite.h"
#include "bn_affine_bg_map_ptr.h"


namespace fe
{
    enum directions{up, down, left, right};

    [[nodiscard]] int _get_map_cell(bn::fixed x, bn::fixed y, bn::affine_bg_ptr& map, bn::span<const bn::affine_bg_map_cell> cells)
    {
        int map_size = map.dimensions().width();
        int cell =  modulo((y.safe_division(8).right_shift_integer() * map_size/8 + x/8), map_size*8).integer();
        return cells.at(cell);
    }

    [[nodiscard]] bool _contains_cell(int tile, bn::vector<int, 32> tiles)
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

    [[nodiscard]] bool _check_collisions_map(bn::fixed_point pos, Hitbox hitbox, directions direction, bn::affine_bg_ptr& map, fe::Level level, bn::span<const bn::affine_bg_map_cell> cells)
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

        if(_contains_cell(_get_map_cell(l, u, map, cells), tiles) ||
        _contains_cell(_get_map_cell(l, d, map, cells), tiles) ||
        _contains_cell(_get_map_cell(r, u, map, cells), tiles) ||
        _contains_cell(_get_map_cell(l, d, map, cells), tiles)){
            return true;
        } else {
            return false;
        }
    }

    constexpr const bn::fixed gravity = 0.2;
    constexpr const bn::fixed wall_run_speed =0.25;
    constexpr const bn::fixed jump_power = 4;
    constexpr const bn::fixed acc = 0.1;
    constexpr const bn::fixed max_dy = 6;
    constexpr const bn::fixed friction = 0.85;

    Enemy::Enemy(int x, int y, bn::camera_ptr camera, bn::affine_bg_ptr map, ENEMY_TYPE type, int hp) :
        _pos(x, y), _camera(camera), _type(type), _hp(hp), _map(map), _level(Level(map))
    {
        _map_cells = map.map().cells_ref().value();
        _dir = 1;
        
        if(_type == ENEMY_TYPE::BAT)
        {
            _sprite = bn::sprite_items::bat_sprite.create_sprite(_pos.x(), _pos.y());
            _sprite.value().set_camera(_camera);
            _sprite.value().set_bg_priority(1);
            _action = bn::create_sprite_animate_action_forever(
                            _sprite.value(), 4, bn::sprite_items::bat_sprite.tiles_item(), 0,1,0,1);
        } else if (_type == ENEMY_TYPE::SLIME){
            _sprite = bn::sprite_items::slime_sprite.create_sprite(_pos.x(), _pos.y());
            _sprite.value().set_camera(_camera);
            _sprite.value().set_bg_priority(1);
            _action = bn::create_sprite_animate_action_forever(
                             _sprite.value(), 20, bn::sprite_items::slime_sprite.tiles_item(), 0,1,0,1);
        }
    }

    void Enemy::set_visible(bool visiblity){
        _sprite.value().set_visible(visiblity);
    }

    bool Enemy::damage_from_left(int damage){
        _dy-=0.4;
        _dx-=1;
        _grounded = false;
        return _take_damage(damage);
    }

    bool Enemy::damage_from_right(int damage){
        _dy-=0.4;
        _dx+=1;
        _grounded = false;
        return _take_damage(damage);
    }

    bool Enemy::_take_damage(int damage){
        if(!_invulnerable)
        {
            _hp -= damage;
            _invulnerable = true;
            if(_hp <= 0){
                if(_type == ENEMY_TYPE::SLIME){
                    _action = bn::create_sprite_animate_action_once(
                        _sprite.value(), 5, bn::sprite_items::slime_sprite.tiles_item(), 2,3,3,3);
                }
                else if (_type == ENEMY_TYPE::BAT)
                {
                    _action = bn::create_sprite_animate_action_once(
                        _sprite.value(), 5, bn::sprite_items::bat_sprite.tiles_item(), 2,3,3,3);
                }
                
                return true;
            }
        }
        return false;
    }
    
    bool Enemy::is_hit(Hitbox attack)
    {
        if(!_dead){
            return check_collisions_bb(attack, _pos.x(), _pos.y(), 8, 8);
        } else {
            return false;
        }
    }

    bool Enemy::_will_hit_wall()
    {   
        
        if(_dx < 0){ // left
            if(!_check_collisions_map(_pos, Hitbox(-4,8,4,8), directions::down, _map, _level, _map_cells)){
                return true;
            }
            if(_check_collisions_map(_pos, Hitbox(-4, 4, 8, 8), directions::right, _map, _level, _map_cells)){
                return true;
            }
        } else { //right
            if(!_check_collisions_map(_pos, Hitbox(4,8,4,8), directions::down, _map, _level, _map_cells)){
                return true;
            }
            if(_check_collisions_map(_pos, Hitbox(4, 4, 8, 8), directions::right, _map, _level, _map_cells)){
                return true;
            }
        }
        return false;
    }

    bool Enemy::_will_fall(){
        if(_dx < 0){ // left
            if(!_check_collisions_map(_pos, Hitbox(-4,8,4,8), directions::down, _map, _level, _map_cells)){
                return true;
            }
        } else { //right
            if(!_check_collisions_map(_pos, Hitbox(4,8,4,8), directions::down, _map, _level, _map_cells)){
                return true;
            }
        }
        return false;
    }

    bn::fixed_point Enemy::pos(){
        return _pos;
    }

    int Enemy::hp(){
        return _hp;
    }

    void Enemy::update(){
        if(!_dead)
        {   
            if(!_sprite.value().visible())
            {
                _sprite.value().set_visible(true);
            }

            //dead check
            if(_action.value().done()){
                _sprite.value().set_visible(false);
                _dead = true;
            }

            if(_invulnerable){
                ++_inv_timer;
                if(_inv_timer > 20){
                    _inv_timer = 0;
                    _invulnerable = false;
                }
            }

            if(_direction_timer > 200){
                // do nothing
            } else{
                ++_direction_timer;
            }

            //apply gravity
            if(_type != ENEMY_TYPE::BAT)
            {
                _dy += gravity;
            }

            if(_type == ENEMY_TYPE::SLIME){
                if(!_invulnerable && _grounded && _direction_timer > 60){
                    if(_will_fall() || _will_hit_wall()){
                        _dx = 0;
                        _dir = -_dir;
                        _direction_timer = 0;
                        _sprite.value().set_horizontal_flip(!_sprite.value().horizontal_flip());
                    }
                } 
                if((_action.value().current_index() == 1 || _action.value().current_index() == 3)  && !_invulnerable && _grounded){
                    _dx += _dir*acc;
                }
            } else if(_type == ENEMY_TYPE::BAT){
                if(_direction_timer > 60){
                    if(_will_hit_wall()){
                        _dx = 0;
                        _dir = -_dir;
                        _direction_timer = 0;
                        _sprite.value().set_horizontal_flip(!_sprite.value().horizontal_flip());
                    }
                } 
                if(!_invulnerable){
                    _dx += _dir*acc;
                }
            }
            
            _dx = _dx * friction;

            if(_dy > 0){
                if(_check_collisions_map(_pos, Hitbox(0,8,8,0), directions::down, _map, _level, _map_cells)){
                    _dy = 0;
                    // BN_LOG(bn::to_string<32>(_pos.x())+" " + bn::to_string<32>(_pos.y()));
                    _pos.set_y(_pos.y() - modulo(_pos.y(),8));
                    _grounded = true;
                } else {
                    _grounded = false;
                }
            }

            if(bn::abs(_dx) > 0){
                if(_check_collisions_map(_pos, Hitbox(0, 0, 4, 8), directions::left, _map, _level, _map_cells) ||
                _check_collisions_map(_pos, Hitbox(0, 0, 4, 8), directions::left, _map, _level, _map_cells)){
                    _dx = -_dx;
                    _direction_timer = 0;
                }
            }

            //max
            if(_dy > max_dy){
                _dy = max_dy;
            }

            _pos.set_x(_pos.x() + _dx);
            _pos.set_y(_pos.y() + _dy);

            _sprite.value().set_position(_pos);
            if(!_action.value().done()){
                _action.value().update();
            }        
        }
    }
}