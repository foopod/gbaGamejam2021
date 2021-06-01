#include "fe_scene_sky.h"

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

#include "bn_sprite_text_generator.h"
#include "variable_8x8_sprite_font.h"

namespace fe
{
    Scene Sky::execute(Player player, bn::fixed_point spawn_location)
    {
        bn::camera_ptr camera = bn::camera_ptr::create(spawn_location.x(), spawn_location.y());

        bn::sprite_text_generator text_generator(variable_8x8_sprite_font);

        //NPC
        NPC penguin = NPC(bn::fixed_point(850, 984), camera, NPC_TYPE::PENGUIN, text_generator);

        // map
        bn::affine_bg_ptr map = bn::affine_bg_items::map.create_bg(512, 512);
        fe::Level level = fe::Level(map);
        map.set_horizontal_scale(2);
        map.set_vertical_scale(2);

        // camera
        map.set_camera(camera);
        
        // bn::fixed max_cpu_usage;
        // int counter = 1;

        bn::vector<Enemy, 32> enemies = {};

        // player
        player.spawn(spawn_location, camera, map, enemies);
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

            if(penguin.check_trigger(player.pos()))
            {
                if(bn::keypad::a_pressed()){
                    player.set_listening(true);
                    penguin.talk();
                }else if(!penguin.is_talking()){
                    player.set_listening(false);
                }
            }
            penguin.update();

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

                if(player.pos().x() < 220 && player.pos().x() > 200){
                    if(player.pos().y() < 730 && player.pos().y() > 720){
                        return Scene::SKY_HOUSE;
                    }
                }
            }
            
            
            bn::core::update();
        }
    }
}