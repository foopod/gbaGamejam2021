#include "fe_healthbar.h"
#include "bn_fixed.h"
#include "bn_sprite_ptr.h"

#include "bn_sprite_items_healthbar.h"
#include "bn_sprite_items_weapon_claw.h"

namespace fe
{
    const constexpr int y = -70;
    const constexpr int inc = 8;
    const constexpr int x = -100;
    
    Healthbar::Healthbar() : 
        _sprites({
            bn::sprite_items::healthbar.create_sprite(x, y, 0),
            bn::sprite_items::healthbar.create_sprite(x + inc, y, 1),
            bn::sprite_items::healthbar.create_sprite(x + inc *2, y, 2),
            bn::sprite_items::healthbar.create_sprite(x + inc *3, y, 3),
            bn::sprite_items::healthbar.create_sprite(x + inc *4, y, 12),
        }),
        _weapon(WEAPON_TYPE::CLAW),
        _weapon_sprite(bn::sprite_items::weapon_claw.create_sprite(x-6, y,0)),
        _action(bn::create_sprite_animate_action_once(
                        _weapon_sprite, 1, bn::sprite_items::weapon_claw.tiles_item(), 1,1,1,1,1,1,1,1,1,1))
    {
        _hp = 9;
        for(int i = 0; i < 5; ++i){
            _sprites[i].set_bg_priority(0);
        }
        _weapon_sprite.set_bg_priority(0);
    }

    int Healthbar::hp(){
        return _hp;
    }

    void Healthbar::set_hp(int hp){
        _hp = hp;
        _sprites[3].set_item(bn::sprite_items::healthbar, 12-hp);
    }

    void Healthbar::set_visible(bool is_visible){
        for(int i = 0; i < 5; ++i){
            _sprites[i].set_visible(is_visible);
        }
        _weapon_sprite.set_visible(is_visible);
    }

    void Healthbar::activate_glow(){
        _is_glowing = true;
        _action = bn::create_sprite_animate_action_once(
                        _weapon_sprite, 15, bn::sprite_items::weapon_claw.tiles_item(), 0,0,8,7,6,5,4,3,2,1);
    }

    void Healthbar::update(){
        if(!_action.value().done()){
            _action.value().update();
        }
    }

    bool Healthbar::is_glow_ready(){
        return _action.value().done();
    }
}   