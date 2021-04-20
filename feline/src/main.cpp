#include "bn_core.h"
#include "bn_log.h"
#include "bn_sprite_builder.h"
#include "bn_sprite_ptr.h"
#include "bn_keypad.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_affine_bg_ptr.h"
#include "bn_affine_bg_item.h"
#include "bn_affine_bg_tiles_ptr.h"
#include "bn_camera_actions.h"
#include "bn_sprite_animate_actions.h"

#include "bn_sprite_items_cat.h"
#include "bn_affine_bg_items_map.h"
#include "bn_affine_bg_items_map1.h"
#include "bn_regular_bg_items_map2.h"


int main()
{
    bn::core::init();

    bn::sprite_ptr cat_sprite = bn::sprite_items::cat.create_sprite(0, 0);
    bn::sprite_animate_action<9> action = bn::create_sprite_animate_action_forever(
                    cat_sprite, 6, bn::sprite_items::cat.tiles_item(), 1, 2, 3, 4, 5, 6, 7, 8, 9);
    
    bn::affine_bg_ptr map_bg = bn::affine_bg_items::map1.create_bg(0, -50);

    bn::camera_ptr camera = bn::camera_ptr::create(0, 0);

    while(true)
    {
        

        if(bn::keypad::left_held())
        {
            BN_LOG(bn::affine_bg_items::map1.tiles_item().tiles_ref().size());
            BN_LOG(map_bg.tiles().tiles_count());
            BN_LOG(map_bg.map().cells_ref()[0]);


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

        // Camera doesn't move up and down
        if(bn::keypad::up_held())
        {
            cat_sprite.set_y(cat_sprite.y() - 1);
        }
        else if(bn::keypad::down_held())
        {
            cat_sprite.set_y(cat_sprite.y() + 1);
        }
        
        cat_sprite.set_camera(camera);
        map_bg.set_camera(camera);
        //map2_bg.set_camera(camera);
        action.update();
        bn::core::update();
    }
}
