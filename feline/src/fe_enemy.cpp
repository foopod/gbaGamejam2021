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

namespace fe
{

    Enemy::Enemy(int x, int y, bn::camera_ptr camera, ENEMY_TYPE type, int hp) :
        _pos(x, y), _camera(camera), _type(type), _hp(hp)
    {
        
        _dir = 1;
        
        if(_type == ENEMY_TYPE::BAT)
        {
            _sprite = bn::sprite_items::bat_sprite.create_sprite(_pos.x(), _pos.y());
            _sprite.value().set_camera(_camera);
            _sprite.value().set_bg_priority(1);
            _action = bn::create_sprite_animate_action_forever(
                            _sprite.value(), 4, bn::sprite_items::bat_sprite.tiles_item(), 0,1);
        } else if (_type == ENEMY_TYPE::SLIME){
            _sprite = bn::sprite_items::slime_sprite.create_sprite(_pos.x(), _pos.y());
            _sprite.value().set_camera(_camera);
            _sprite.value().set_bg_priority(1);
            _action = bn::create_sprite_animate_action_forever(
                            _sprite.value(), 20, bn::sprite_items::slime_sprite.tiles_item(), 0,1);
        }
    }

    bool Enemy::damage_from_left(int damage){
        _hp -= damage;
        _sprite.value().set_visible(false);
        return true;
    }

    bool Enemy::damage_from_right(int damage){
        _hp -= damage;
        _sprite.value().set_visible(false);
        return true;
    }
    
    bool Enemy::is_hit(Hitbox attack)
    {
        return check_collisions_bb(attack, _pos.x(), _pos.y(), 16, 16);
    }

    bn::fixed_point Enemy::pos(){
        return _pos;
    }

    int Enemy::hp(){
        return _hp;
    }

    void Enemy::update(){
        if(_type == ENEMY_TYPE::BAT){
            if(_pos.x() < 200){
                _dir = 1;
                _sprite.value().set_horizontal_flip(false);
            } else if(_pos.x() > 400){
                _dir = -1;
                _sprite.value().set_horizontal_flip(true);
            }
            _pos.set_x(_pos.x() + _dir);
        } else if(_type == ENEMY_TYPE::SLIME){
            if(_pos.x() < 370){
                _dir = 1;
                _sprite.value().set_horizontal_flip(true);
            } else if(_pos.x() > 510){
                _dir = -1;
                _sprite.value().set_horizontal_flip(false);
            }
            bn::fixed m = 0;
            if(_action.value().current_index() == 1){
                m = 0.6;
            }
            _pos.set_x(_pos.x() + _dir*m);
        }
        _sprite.value().set_position(_pos);
        _action.value().update();
    }
}