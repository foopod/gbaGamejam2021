#include "fe_dungeon_scene.h"

//butano
#include "bn_core.h"
#include "bn_log.h"
#include "bn_string.h"
#include "bn_keypad.h"
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
#include "fe_enemy.h"
#include "fe_enemy_type.h"

//assets
#include "bn_sprite_items_debug.h"
#include "bn_affine_bg_items_dungeon.h"
#include "bn_regular_bg_items_dungeon_bg.h"
#include "bn_regular_bg_items_vines.h"

namespace fe
{
    Scene Dungeon::execute(Player player, bn::fixed_point spawn_location)
    {
        bn::sprite_ptr debug1 = bn::sprite_items::debug.create_sprite(0,0);
        debug1.set_bg_priority(0);
        bn::sprite_ptr debug2 = bn::sprite_items::debug.create_sprite(0,0);
        debug2.set_bg_priority(0);
        bn::sprite_ptr debug3 = bn::sprite_items::debug.create_sprite(0,0);
        debug3.set_bg_priority(0);

        bn::camera_ptr camera = bn::camera_ptr::create(spawn_location.x(), spawn_location.y());

        //NPC
        NPC golem = NPC(bn::fixed_point(640, 960), camera, NPC_TYPE::GOLEM);

        //Enemies
        bn::vector<Enemy,32> enemies = {};
        Enemy bat = Enemy(200, 952, camera, ENEMY_TYPE::BAT, 1);
        enemies.push_back(bat);
        Enemy slime = Enemy(490, 824, camera, ENEMY_TYPE::SLIME, 2);
        enemies.push_back(slime);

        // map
        bn::regular_bg_ptr map_bg = bn::regular_bg_items::dungeon_bg.create_bg(512, 512);
        bn::affine_bg_ptr map = bn::affine_bg_items::dungeon.create_bg(512, 512);
        bn::regular_bg_ptr vines = bn::regular_bg_items::vines.create_bg(512, 512);
        map_bg.set_priority(2);
        map.set_priority(1);
        vines.set_priority(0);
        fe::Level level = fe::Level(map);
        map.set_horizontal_scale(2);
        map.set_vertical_scale(2);

        map.set_camera(camera);
        map_bg.set_camera(camera);
        vines.set_camera(camera);
        debug1.set_camera(camera);
        debug2.set_camera(camera);
        debug3.set_camera(camera);

        // bn::fixed max_cpu_usage;
        // int counter = 1;

        player.spawn(spawn_location, camera, map, &enemies);
        player.set_can_wallrun(true);

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

            if(golem.near_player(player.pos()))
            {
                if(bn::keypad::a_pressed()){
                    player.set_listening(true);
                    golem.talk();
                }else if(!golem.is_talking()){
                    player.set_listening(false);
                }
            }
            golem.update();
            bat.update();
            slime.update();

            player.update_position(map,level);
            player.apply_animation_state();
            vines.set_position(bn::fixed_point((512-player.pos().x())/10,(512-player.pos().y())/10));
            // BN_LOG(bn::to_string<32>(player.pos().x())+" " + bn::to_string<32>(player.pos().y()));

            //door && npc check
            if(bn::keypad::up_pressed())
            {
                if(player.pos().x() < 120 && player.pos().x() > 100){
                    if(player.pos().y() < 960 && player.pos().y() > 920){
                        return Scene::DUNGEON_SKY;
                    }
                }

            }

            debug1.set_position(player.pos());
            if(player.is_right()){
                debug1.set_x(player.pos().x() + 8);
            } else {
                debug1.set_x(player.pos().x() - 8);
            }
            debug2.set_position(bat.pos());
            debug3.set_position(slime.pos());

            bn::core::update();
        }
    }
}