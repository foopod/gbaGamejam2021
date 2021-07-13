#include "fe_scene_butano.h"

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
#include "bn_sprite_items_butano.h"
#include "bn_sprite_items_explosion.h"
#include "bn_affine_bg_items_gbajam_splash.h"
#include "bn_regular_bg_items_butano_background.h"
#include "bn_regular_bg_items_butano_colour.h"

#include "bn_music_items.h"

#include "bn_sprite_text_generator.h"
#include "variable_8x8_sprite_font.h"

namespace fe
{

    Scene Butano::execute()
    {
        // map
        // bn::affine_bg_ptr map = bn::affine_bg_items::title.create_bg(0, 0);
        //reset player

        bn::sprite_ptr butano1 = bn::sprite_items::butano.create_sprite(-80,24,1);
        bn::sprite_ptr butano2 = bn::sprite_items::butano.create_sprite(-80,-24,0);
        butano1.set_bg_priority(1);
        butano2.set_bg_priority(1);
        butano1.set_scale(0.75);
        butano2.set_scale(0.75);

        bn::sprite_ptr explosion = bn::sprite_items::explosion.create_sprite(-60,30,0);
        explosion.set_visible(false);
        explosion.set_bg_priority(2);
        explosion.set_scale(2);
        bn::optional<bn::sprite_animate_action<10>> _action;

        bn::sprite_ptr explosion2 = bn::sprite_items::explosion.create_sprite(-100,0,0);
        explosion2.set_visible(false);
        explosion2.set_bg_priority(2);
        explosion2.set_scale(2);
        bn::optional<bn::sprite_animate_action<10>> _action2;

        bn::sprite_ptr explosion3 = bn::sprite_items::explosion.create_sprite(-77,-30,0);
        explosion3.set_visible(false);
        explosion3.set_bg_priority(2);
        explosion3.set_scale(2);
        bn::optional<bn::sprite_animate_action<10>> _action3;
        

        // bn::regular_bg_ptr map = bn::regular_bg_items::butano_background.create_bg(0 , 0);
        // map.set_priority(2);
        bn::regular_bg_ptr map2 = bn::regular_bg_items::butano_colour.create_bg(0 , 10);
        map2.set_priority(3);
        // map2.set_visible(false);
        // bn::affine_bg_ptr map = bn::affine_bg_items::gbajam_splash.create_bg(0 , 0);
        // map.set_scale(2);
        
        
        // map.value().set_scale(2);
        bn::sprite_text_generator text_generator(variable_8x8_sprite_font);
        bn::vector<bn::sprite_ptr, 32> _text_sprites;

        bn::string<32> middle_text("BUTANO ");
        middle_text.append(bn::to_string<4>(bn::version::major()));
        middle_text.append('.');
        middle_text.append(bn::to_string<4>(bn::version::minor()));
        middle_text.append('.');
        middle_text.append(bn::to_string<4>(bn::version::patch()));

        


        // text_generator.generate(0, 30, "Cat's only have nine lives.", text_sprites);


        // player sprite
        // bn::sprite_ptr rip = bn::sprite_items::rip.create_sprite(0,-10);
        // bn::camera_ptr camera = bn::camera_ptr::create(0,0);

        // rip.set_camera(camera);
        // map.set_camera(camera);


        // text_generator.set_center_alignment();
        text_generator.generate(-40, -16, "Made with", _text_sprites);
        text_generator.generate(-40, 0, middle_text, _text_sprites);
        text_generator.generate(-40, 16, "github.com/GValiente/butano", _text_sprites);

        for(bn::sprite_ptr& text_sprite : _text_sprites)
        {
            text_sprite.set_blending_enabled(true);
            text_sprite.set_bg_priority(0);
            text_sprite.put_above();
        }
        int timer = 0;
        // camera

        while(true)
        {
            // map.set_x(map.x() -1);
            // map.set_y(map.y() +1);
            if(timer > 180){
                
                return Scene::TITLE;
            }

            if(timer>30 && !_action.has_value()){
                explosion.set_visible(true);
                _action = bn::create_sprite_animate_action_once(
                        explosion, 2, bn::sprite_items::explosion.tiles_item(), 
                        0,1,2,3,4,5,6,7,8,9);
            }

            if(timer>60 && !_action2.has_value()){
                explosion2.set_visible(true);
                _action2 = bn::create_sprite_animate_action_once(
                        explosion2, 2, bn::sprite_items::explosion.tiles_item(), 
                        0,1,2,3,4,5,6,7,8,9);
            }

            if(timer>45 && !_action3.has_value()){
                explosion3.set_visible(true);
                _action3 = bn::create_sprite_animate_action_once(
                        explosion3, 2, bn::sprite_items::explosion.tiles_item(), 
                        0,1,2,3,4,5,6,7,8,9);
            }

            if(_action.has_value() && !_action.value().done()){
                _action.value().update();
            }

            if(_action2.has_value() && !_action2.value().done()){
                _action2.value().update();
            }

            if(_action3.has_value() && !_action3.value().done()){
                _action3.value().update();
            }
            ++timer;
            bn::core::update();
        }

        
    }
}