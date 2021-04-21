#include "bn_core.h"
#include "bn_log.h"
#include "bn_sprite_builder.h"
#include "bn_sprite_ptr.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_regular_bg_ptr.h"
#include "bn_fixed_point.h"
#include "bn_regular_bg_item.h"
#include "bn_affine_bg_ptr.h"
#include "bn_affine_bg_item.h"
#include "bn_affine_bg_tiles_ptr.h"
#include "bn_affine_bg_map_ptr.h"
#include "bn_camera_actions.h"
#include "bn_sprite_animate_actions.h"

#include "bn_sprite_items_cat.h"
#include "bn_affine_bg_items_map.h"
#include "bn_affine_bg_items_map1.h"
#include "bn_regular_bg_items_map2.h"

#include "bn_string_view.h"
#include "bn_vector.h"
#include "bn_sprite_text_generator.h"
#include "variable_8x16_sprite_font.h"

namespace
{
    bn::fixed modulo(bn::fixed a, bn::fixed m)
    {

        return a - m * ((a/m).right_shift_integer());
    }

    bn::fixed get_map_index(bn::fixed x, bn::fixed y, bn::fixed map_size)
    {

        return modulo((y.safe_division(8).right_shift_integer() * map_size/8 + x/8), map_size*8);
    }

    bn::fixed_point apply_gravity(bn::fixed_point pos)
    {
        return bn::fixed_point(pos.x(), pos.y() +1);
    }

    void check_collisions(bn::sprite_ptr& sprite, bn::affine_bg_ptr& map)
    {
        bn::fixed current_cell = get_map_index(sprite.x(), sprite.y(), map.dimensions().width());
        bool is_hit = map.map().cells_ref().value().at(current_cell.integer()) != 0;
        if(is_hit){
            sprite.set_y(sprite.y() -1);
        }
    }
}

int main()
{
    bn::core::init();

    bn::fixed_point init_pos = bn::fixed_point(60, 432);


    // current cell player is on
    bn::fixed current_cell = 0;

    // player sprite
    bn::sprite_ptr cat_sprite = bn::sprite_items::cat.create_sprite(init_pos.x(), init_pos.y());
    bn::sprite_animate_action<9> action = bn::create_sprite_animate_action_forever(
                    cat_sprite, 6, bn::sprite_items::cat.tiles_item(), 1, 2, 3, 4, 5, 6, 7, 8, 9);
    
    // map
    bn::affine_bg_ptr map_bg = bn::affine_bg_items::map.create_bg(256, 256);

    bn::camera_ptr camera = bn::camera_ptr::create(init_pos.x(), init_pos.y());

    // text set up
    bn::sprite_text_generator text_generator(variable_8x16_sprite_font);
    text_generator.set_center_alignment();
    bn::vector<bn::sprite_ptr, 32> text_sprites;
    
    while(true)
    {
        cat_sprite.set_position(apply_gravity(cat_sprite.position()));
        check_collisions(cat_sprite, map_bg);

    
        if(bn::keypad::left_held())
        {
            cat_sprite.set_horizontal_flip(true);
            cat_sprite.set_x(cat_sprite.x() - 1);
            camera.set_x(camera.x() - 1);
            
        }
        else if(bn::keypad::right_held())
        {
            cat_sprite.set_horizontal_flip(false);
            cat_sprite.set_x(cat_sprite.x() + 1);
            camera.set_x(camera.x() + 1);
        }

        if(bn::keypad::up_held())
        {
            cat_sprite.set_y(cat_sprite.y() - 1);
            //camera.set_y(camera.y() - 1);
        }
        else if(bn::keypad::down_held())
        {
            cat_sprite.set_y(cat_sprite.y() + 1);
            //camera.set_y(camera.y() + 1);
        }

        //display current tile
        current_cell = get_map_index(cat_sprite.x(), cat_sprite.y(), 512);
        text_sprites.clear();
        text_generator.generate(0, -40, bn::to_string<32>(map_bg.map().cells_ref().value().at(current_cell.integer())), text_sprites);

        cat_sprite.set_camera(camera);
        map_bg.set_camera(camera);
        
        action.update();
        bn::core::update();
    }
}
