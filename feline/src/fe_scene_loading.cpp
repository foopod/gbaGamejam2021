#include "fe_scene_loading.h"

//butano
#include "bn_core.h"
#include "bn_log.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_affine_bg_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_random.h"

//fe code
#include "fe_scene.h"

//assets
#include "bn_sprite_items_cat_sprite.h"
#include "bn_affine_bg_items_loading_bg.h"

namespace fe
{
    void Loading::execute()
    {
        bn::fixed_point init_pos = bn::fixed_point(0, 0);

        // player sprite
        bn::sprite_ptr cat_sprite1 = bn::sprite_items::cat_sprite.create_sprite(init_pos.x(), init_pos.y()-50);
        bn::sprite_ptr cat_sprite2 = bn::sprite_items::cat_sprite.create_sprite(init_pos.x(), init_pos.y()-25);
        bn::sprite_ptr cat_sprite3 = bn::sprite_items::cat_sprite.create_sprite(init_pos.x(), init_pos.y()+25);
        bn::sprite_ptr cat_sprite4 = bn::sprite_items::cat_sprite.create_sprite(init_pos.x(), init_pos.y()+50);
        bn::sprite_ptr cat_sprite5 = bn::sprite_items::cat_sprite.create_sprite(init_pos.x(), init_pos.y());
        
        bn::sprite_animate_action<10> action1 = bn::create_sprite_animate_action_forever(
                        cat_sprite1, 2, bn::sprite_items::cat_sprite.tiles_item(), 8, 9,10,11, 2, 3, 4, 5, 6,7);
        bn::sprite_animate_action<10> action2 = bn::create_sprite_animate_action_forever(
                        cat_sprite2, 2, bn::sprite_items::cat_sprite.tiles_item(), 8, 9,10,11, 2, 3, 4, 5, 6,7);
        bn::sprite_animate_action<10> action3 = bn::create_sprite_animate_action_forever(
                        cat_sprite3, 2, bn::sprite_items::cat_sprite.tiles_item(), 8, 9,10,11, 2, 3, 4, 5, 6,7);
        bn::sprite_animate_action<10> action4 = bn::create_sprite_animate_action_forever(
                        cat_sprite4, 2, bn::sprite_items::cat_sprite.tiles_item(), 8, 9,10,11, 2, 3, 4, 5, 6,7);
        bn::sprite_animate_action<10> action5 = bn::create_sprite_animate_action_forever(
                        cat_sprite5, 2, bn::sprite_items::cat_sprite.tiles_item(), 8, 9,10,11, 2, 3, 4, 5, 6,7);

        bn::camera_ptr camera = bn::camera_ptr::create(init_pos.x()+100, init_pos.y());

        // map
        bn::affine_bg_ptr map = bn::affine_bg_items::loading_bg.create_bg(512, 512);
        // map.set_horizontal_scale(2);

        // camera
        cat_sprite1.set_camera(camera);
        cat_sprite2.set_camera(camera);
        cat_sprite3.set_camera(camera);
        cat_sprite4.set_camera(camera);
        cat_sprite5.set_camera(camera);
        map.set_camera(camera);

        for(int i = 0; i < 120; ++i)
        {
            cat_sprite1.set_x(cat_sprite1.x() + 1.3);
            cat_sprite2.set_x(cat_sprite2.x() + 2);
            cat_sprite3.set_x(cat_sprite3.x() + 1.5);
            cat_sprite4.set_x(cat_sprite4.x() + 2.2);
            cat_sprite5.set_x(cat_sprite5.x() + 1.7);

            action1.update();
            action2.update();
            action3.update();
            action4.update();
            action5.update();
            bn::core::update();
        }
    }
}