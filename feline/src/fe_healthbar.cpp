#include "fe_healthbar.h"
#include "bn_fixed.h"
#include "bn_sprite_ptr.h"

#include "bn_sprite_items_healthbar.h"

namespace fe
{
    const constexpr int y = -70;
    const constexpr int inc = 8;
    const constexpr int x = -120;
    
    Healthbar::Healthbar() : 
        _sprites({
            bn::sprite_items::healthbar.create_sprite(x, y, 0),
            bn::sprite_items::healthbar.create_sprite(x + inc, y, 1),
            bn::sprite_items::healthbar.create_sprite(x + inc *2, y, 2),
            bn::sprite_items::healthbar.create_sprite(x + inc *3, y, 3),
            bn::sprite_items::healthbar.create_sprite(x + inc *4, y, 4),
            bn::sprite_items::healthbar.create_sprite(x + inc *5, y, 5),
            bn::sprite_items::healthbar.create_sprite(x + inc *6, y, 6),
            bn::sprite_items::healthbar.create_sprite(x + inc *7, y, 7),
            bn::sprite_items::healthbar.create_sprite(x + inc *8, y, 8),
            bn::sprite_items::healthbar.create_sprite(x + inc *9, y, 9),
            bn::sprite_items::healthbar.create_sprite(x + inc *10, y, 10)
        })
    {
        _hp = 9;
        for(int i = 0; i < 11; ++i){
            _sprites[i].set_bg_priority(0);
        }
    }

    int Healthbar::hp(){
        return _hp;
    }

    void Healthbar::set_hp(int hp){
        _hp = hp;
        for(int i = 0; i < 9; ++i){
            if(i < hp){
                _sprites[i+1].set_visible(true);
            } else{
                _sprites[i+1].set_visible(false);
            }
        }
        _sprites[10].set_x(x + inc*(hp+1));
    }

    void Healthbar::set_visible(bool is_visible){
        _is_visible = is_visible;
    }

}