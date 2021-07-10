#include "fe_scene_title.h"

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

//assets
#include "bn_sprite_items_cat_sprite.h"
#include "bn_sprite_items_title_cat.h"
#include "bn_sprite_items_title_start_1.h"
#include "bn_sprite_items_title_start_2.h"
#include "bn_sprite_items_title_start_3.h"
#include "bn_sprite_items_title_1.h"
#include "bn_sprite_items_title_2.h"
#include "bn_affine_bg_items_title.h"

#include "bn_music_items.h"

namespace fe
{
    bn::fixed loop(bn::fixed number, bn::fixed increment){
        number += increment;
        if(number > 256){
            return 0;
        } else {
            return number;
        }
    }

    Scene Title::execute()
    {
        // map
        bn::affine_bg_ptr map = bn::affine_bg_items::title.create_bg(0, 0);

        bn::fixed_point init_pos = bn::fixed_point(-70, 44);
        bn::music_items::mystic.play();

        // player sprite
        bn::sprite_ptr cat_sprite = bn::sprite_items::cat_sprite.create_sprite(init_pos.x(), init_pos.y());
        bn::sprite_ptr platform = bn::sprite_items::title_cat.create_sprite(init_pos.x(), init_pos.y());
        platform.set_horizontal_scale(2);
        platform.set_vertical_scale(2);

        cat_sprite.set_horizontal_scale(2);
        cat_sprite.set_vertical_scale(2);

        bn::sprite_ptr start1 = bn::sprite_items::title_start_1.create_sprite(-16,30);
        bn::sprite_ptr start2 = bn::sprite_items::title_start_2.create_sprite(0,30);
        bn::sprite_ptr start3 = bn::sprite_items::title_start_3.create_sprite(16,30);
        
        bn::sprite_ptr title1 = bn::sprite_items::title_1.create_sprite(-32,-30);
        bn::sprite_ptr title2 = bn::sprite_items::title_2.create_sprite(32,-30);
       

        bn::sprite_animate_action<10> _action = bn::create_sprite_animate_action_forever(
                        cat_sprite, 30, bn::sprite_items::cat_sprite.tiles_item(), 0,1,0,1,0,1,0,1,0,1);
        
        bn::camera_ptr camera = bn::camera_ptr::create(0,0);

        

        bn::array<bn::fixed, bn::display::height()> horizontal_deltas;
        bn::affine_bg_pivot_position_hbe_ptr horizontal_hbe =
                bn::affine_bg_pivot_position_hbe_ptr::create_horizontal(map, horizontal_deltas);

        // camera
        platform.set_camera(camera);
        cat_sprite.set_camera(camera);
        map.set_camera(camera);

        bn::fixed layer_1 = 0;
        bn::fixed layer_2 = 0;
        bn::fixed layer_3 = 0;
        bn::fixed layer_4 = 0;

        bn::fixed start_y = 20;
        bn::fixed start_amp = 0;

        int timer = 0;

        while(!(bn::keypad::any_pressed() && timer > 120))
        {
            // cat_sprite.set_x(cat_sprite.x() + 1.3);

            ++timer;
            layer_1 = loop(layer_1, 0.3);
            layer_2 = loop(layer_2, 0.2);
            layer_3 = loop(layer_3, 0.1);
            layer_4 = loop(layer_4, 0.5);

            start_amp += 6;
            if(start_amp >= 360){
                start_amp = 0;
            }

            start1.set_y(start_y + bn::degrees_lut_sin(start_amp*1)*4);
            start2.set_y(start_y + bn::degrees_lut_sin(start_amp*1)*4);
            start3.set_y(start_y + bn::degrees_lut_sin(start_amp*1)*4);


            for(int index = 0, limit = 160; index < limit; ++index)
            {
                if( index > 140){
                    horizontal_deltas[index] = layer_1;
                } 
                else if(index > 127)
                {
                    horizontal_deltas[index] = layer_2;
                }
                else if(index > 118)
                {
                    horizontal_deltas[index] = layer_3;
                }
                else if(index > 0)
                {
                    horizontal_deltas[index] = layer_4;
                }
                else {
                    horizontal_deltas[index] = 0;
                }
            }

            horizontal_hbe.reload_deltas_ref();

            _action.update();
            
            bn::core::update();
        }

        return Scene::HOUSE_START;
    }
}