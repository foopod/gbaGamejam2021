#include "fe_sky_scene.h"

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

//fe code
#include "fe_level.h"
#include "fe_player.h"
#include "fe_scene.h"
#include "fe_npc.h"
#include "fe_npc_type.h"

//assets
#include "bn_sprite_items_cat.h"
#include "bn_affine_bg_items_map.h"
namespace fe
{
    Scene Sky::execute(bn::fixed_point spawn)
    {
        // player sprite
        bn::sprite_ptr cat_sprite = bn::sprite_items::cat.create_sprite(spawn.x(), spawn.y());
        cat_sprite.set_bg_priority(1);
        bn::camera_ptr camera = bn::camera_ptr::create(spawn.x(), spawn.y());

        // map
        bn::affine_bg_ptr map = bn::affine_bg_items::map.create_bg(512, 512);
        fe::Level level = fe::Level(map);
        map.set_horizontal_scale(2);
        map.set_vertical_scale(2);

        // camera
        cat_sprite.set_camera(camera);
        map.set_camera(camera);

        // player
        fe::Player player = fe::Player(spawn, cat_sprite, camera);
        while(true)
        {
            

            //elevator.update_position();
            player.update_position(map,level);
            player.apply_animation_state();
            // BN_LOG(bn::to_string<32>(player.pos().x())+" " + bn::to_string<32>(player.pos().y()));
            
            if(bn::keypad::up_pressed())
            {
                if(player.pos().x() < 80 && player.pos().x() > 50){
                    if(player.pos().y() < 980 && player.pos().y() > 920){
                        return Scene::SKY_DUNGEON;
                    }
                }

                if(player.pos().x() < 660 && player.pos().x() > 640){
                    if(player.pos().y() < 650 && player.pos().y() > 630){
                        return Scene::SKY_HOUSE;
                    }
                }
            }
            
            
            bn::core::update();
        }
    }
}