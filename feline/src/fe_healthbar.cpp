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
            bn::sprite_items::healthbar.create_sprite(x + inc *4, y, 12),
        })
    {
        _hp = 9;
        for(int i = 0; i < 5; ++i){
            _sprites[i].set_bg_priority(0);
        }
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
    }

}