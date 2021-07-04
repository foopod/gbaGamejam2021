#ifndef FE_HEALTHBAR_H
#define FE_HEALTHBAR_H

#include "bn_sprite_ptr.h"
#include "bn_optional.h"
#include "bn_sprite_animate_actions.h"

namespace fe
{
    enum class WEAPON_TYPE
    {
        CLAW
    };

    class Healthbar
    {
        private:
            bn::sprite_ptr _sprites[5];
            int _hp;
            bool _is_visible;
            WEAPON_TYPE _weapon;
            bn::sprite_ptr _weapon_sprite;
            bool _is_glowing = false;

            bn::optional<bn::sprite_animate_action<10>> _action;

        public:
            Healthbar();
            
            int hp();
            void set_hp(int hp);
            void set_visible(bool is_visible);
            void activate_glow();
            bool is_glow_active();
            void update();
            bool is_glow_ready();
    };
}

#endif