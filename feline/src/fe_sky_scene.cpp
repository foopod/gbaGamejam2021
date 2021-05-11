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
#include "bn_affine_bg_map_ptr.h"
#include "bn_optional.h"
#include "bn_span.h"
#include "bn_affine_bg_map_cell.h"

//fe code
#include "fe_level.h"
#include "fe_player.h"
#include "fe_scene.h"
#include "fe_npc.h"
#include "fe_npc_type.h"

//assets
#include "bn_sprite_items_cat_sprite.h"
#include "bn_affine_bg_items_map.h"
namespace fe
{
    Scene Sky::execute(bn::fixed_point spawn)
    {
        // player sprite
        bn::sprite_ptr cat_sprite = bn::sprite_items::cat_sprite.create_sprite(spawn.x(), spawn.y());
        cat_sprite.set_bg_priority(1);
        bn::camera_ptr camera = bn::camera_ptr::create(spawn.x(), spawn.y());

        // map
        bn::affine_bg_ptr map = bn::affine_bg_items::map.create_bg(512, 512);
        fe::Level level = fe::Level(map);
        map.set_horizontal_scale(2);
        map.set_vertical_scale(2);
        bn::span<const bn::affine_bg_map_cell> cells = map.map().cells_ref().value();

        // camera
        cat_sprite.set_camera(camera);
        map.set_camera(camera);
        
        // bn::fixed max_cpu_usage;
        // int counter = 1;

        // player
        fe::Player player = fe::Player(spawn, cat_sprite, camera, cells);
        while(true)
        {
            
            // max_cpu_usage = bn::max(max_cpu_usage, bn::core::last_cpu_usage());
            // --counter;
            // if(! counter)
            // {
            //     BN_LOG("cpu:" + bn::to_string<32>((max_cpu_usage * 100).right_shift_integer()));
            //     max_cpu_usage = 0;
            //     counter = 60;
            // }

            //elevator.update_position();
            player.update_position(map, level);
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