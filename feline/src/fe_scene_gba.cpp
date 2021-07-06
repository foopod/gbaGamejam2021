#include "fe_scene_gba.h"

//butano
#include "bn_core.h"
#include "bn_version.h"
#include "bn_log.h"
#include "bn_math.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_display.h"
#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_affine_bg_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_affine_bg_pivot_position_hbe_ptr.h"
#include "bn_affine_bg_attributes_hbe_ptr.h"

//fe code
#include "fe_scene.h"
#include "fe_player.h"

//assets
#include "bn_sprite_items_rip.h"
#include "bn_affine_bg_items_gbajam_splash.h"

#include "bn_music_items.h"

#include "bn_sprite_text_generator.h"
#include "variable_8x8_sprite_font.h"

namespace fe
{

    Scene GBA::execute()
    {

        bn::affine_bg_ptr map = bn::affine_bg_items::gbajam_splash.create_bg(0 , 0);
        // map.value().set_scale(2);
        
        bn::sprite_text_generator text_generator(variable_8x8_sprite_font);
        bn::vector<bn::sprite_ptr, 32> _text_sprites;

        bn::string<32> middle_text("BUTANO ");
        middle_text.append(bn::to_string<4>(bn::version::major()));
        middle_text.append('.');
        middle_text.append(bn::to_string<4>(bn::version::minor()));
        middle_text.append('.');
        middle_text.append(bn::to_string<4>(bn::version::patch()));

        for(bn::sprite_ptr& text_sprite : _text_sprites)
        {
            text_sprite.set_blending_enabled(true);
        }

        int timer = 0;
        // camera

        while(true)
        {
            if(timer > 180){
                // map.reset();
                return Scene::BUTANO;
            }
            
            ++timer;
            bn::core::update();
        }

        
    }
}