#include "fe_scene_death.h"

//butano
#include "bn_core.h"
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
#include "bn_affine_bg_items_title.h"

#include "bn_music_items.h"

#include "bn_sprite_text_generator.h"
#include "variable_8x8_sprite_font.h"

namespace fe
{

    Death::Death(Player& player)
    : _player( & player ){}

    Scene Death::execute()
    {
        // map
        // bn::affine_bg_ptr map = bn::affine_bg_items::title.create_bg(0, 0);
        //reset player
        _player->set_can_teleport(false);
        _player->set_hp(9);

        bn::music_items::mystic.play();

        bn::sprite_text_generator text_generator(variable_8x8_sprite_font);
        bn::vector<bn::sprite_ptr, 32> text_sprites;

        bn::vector<bn::sprite_ptr, 32> text_sprites2;
        text_generator.set_bg_priority(0);
        text_generator.set_center_alignment();


        text_generator.generate(0, 30, "Cat's only have nine lives.", text_sprites);


        // player sprite
        bn::sprite_ptr rip = bn::sprite_items::rip.create_sprite(0,-10);
        // bn::camera_ptr camera = bn::camera_ptr::create(0,0);

        // rip.set_camera(camera);
        // map.set_camera(camera);


        int timer = 0;
        bool text_made = false;
        // camera

        while(timer < 180 || !bn::keypad::any_pressed())
        {
            // cat_sprite.set_x(cat_sprite.x() + 1.3);
            ++timer;

            if(timer > 180 && !text_made){
                text_generator.generate(0, 70, "press any key", text_sprites2);
                text_made = true;
            }
            bn::core::update();
        }

        return Scene::TITLE;
    }
}