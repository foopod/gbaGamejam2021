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
#include "bn_math.h"
#include "bn_affine_bg_map_cell.h"


//fe code
#include "fe_level.h"
#include "fe_player.h"
#include "fe_scene.h"
#include "fe_npc.h"
#include "fe_tooltip.h"
#include "fe_npc_type.h"
#include "fe_enemy.h"
#include "fe_enemy_type.h"

//assets
#include "bn_music_items.h"
#include "bn_sprite_items_debug.h"
#include "bn_affine_bg_items_dungeon.h"
#include "bn_regular_bg_items_dungeon_bg.h"
#include "bn_regular_bg_items_vines.h"

#include "bn_sprite_text_generator.h"
#include "variable_8x8_sprite_font.h"

namespace fe
{
    Scene Dungeon::execute(Player player, bn::fixed_point spawn_location)
    {
        bn::camera_ptr camera = bn::camera_ptr::create(spawn_location.x(), spawn_location.y());

        bn::sprite_text_generator text_generator(variable_8x8_sprite_font);

        bn::music_items::house.play();

        //NPC
        NPC golem = NPC(bn::fixed_point(550, 944), camera, NPC_TYPE::GOLEM, text_generator);
        Tooltip explain_attack = Tooltip(bn::fixed_point(147, 952),"Press 'B' to Attack", text_generator);
        

        // map
        bn::regular_bg_ptr map_bg = bn::regular_bg_items::dungeon_bg.create_bg(512, 512);
        bn::affine_bg_ptr map = bn::affine_bg_items::dungeon.create_bg(512, 512);
        bn::regular_bg_ptr vines = bn::regular_bg_items::vines.create_bg(0, 0);
        map_bg.set_priority(2);
        map.set_priority(1);
        vines.set_priority(0);
        Level level = Level(map);
        map.set_horizontal_scale(2);
        map.set_vertical_scale(2);

        map.set_camera(camera);
        map_bg.set_camera(camera);
        vines.set_camera(camera);

        // bn::fixed max_cpu_usage;
        // int counter = 1;

        //Enemies
        bn::vector<Enemy, 32> enemies = {};
        enemies.push_back(Enemy(200, 952, camera, map, ENEMY_TYPE::BAT, 1));
        enemies.push_back(Enemy(112, 984, camera, map, ENEMY_TYPE::SLIME, 2));
        enemies.push_back(Enemy(490, 822, camera, map, ENEMY_TYPE::SLIME, 2));
        enemies.push_back(Enemy(305, 568, camera, map, ENEMY_TYPE::SLIME, 2));
        enemies.push_back(Enemy(700, 568, camera, map, ENEMY_TYPE::SLIME, 2));
        enemies.push_back(Enemy(439, 439, camera, map, ENEMY_TYPE::SLIME, 2));

        //player
        player.spawn(spawn_location, camera, map, enemies);
        player.set_can_wallrun(true);

        // int index = 1;

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

            if(golem.check_trigger(player.pos()))
            {
                if(bn::keypad::a_pressed()){
                    player.set_listening(true);
                    golem.talk();
                }else if(!golem.is_talking()){
                    player.set_listening(false);
                }
            } else if(explain_attack.check_trigger(player.pos())){
                player.set_listening(true);
            } else {
                player.set_listening(false);
            }

            golem.update();
            explain_attack.update();
            

            for(Enemy& enemy : enemies){
                if(bn::abs(enemy.pos().x() - camera.x()) < 240 && bn::abs(enemy.pos().y() - camera.y()) < 160){
                    enemy.update();
                } else {
                    enemy.set_visible(false);
                }
            }

            player.update_position(map,level);
            player.apply_animation_state();

            vines.set_position(bn::fixed_point((player.pos().x()-500)/10,(player.pos().y())/10));
            // BN_LOG(bn::to_string<32>(player.pos().x())+" " + bn::to_string<32>(player.pos().y()));

            //door
            if(bn::keypad::up_pressed() && !player.is_listening())
            {
                if(player.pos().x() < 120 && player.pos().x() > 100){
                    if(player.pos().y() < 960 && player.pos().y() > 920){
                        return Scene::DUNGEON_SKY;
                    }
                }

            }
            // BN_PROFILER_RESET();
            bn::core::update();
            // bn::profiler::show();
            
        }
    }
}