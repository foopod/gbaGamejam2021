#include "fe_scene_dungeon.h"

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
#include "fe_data.h"
#include "fe_story_save.h"

//assets
// #include "bn_music_items.h"
#include "bn_sprite_items_debug.h"
#include "bn_affine_bg_items_dungeon_2x.h"
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

        // bn::music_items::house.play();
        
        // map
        bn::regular_bg_ptr map_bg = bn::regular_bg_items::dungeon_bg.create_bg(512, 512);
        bn::affine_bg_ptr map = bn::affine_bg_items::dungeon_2x.create_bg(512, 512);
        bn::regular_bg_ptr vines = bn::regular_bg_items::vines.create_bg(0, 0);
        map_bg.set_priority(2);
        map.set_priority(1);
        vines.set_priority(0);
        Level level = Level(map);

        // map.set_horizontal_scale(2);
        // map.set_vertical_scale(2);

        map.set_camera(camera);
        map_bg.set_camera(camera);
        vines.set_camera(camera);

        // bn::fixed max_cpu_usage;
        // int counter = 1;

        //Enemies
        bn::vector<Enemy, 32> enemies = {};
        enemies.push_back(Enemy(315, 168, camera, map, ENEMY_TYPE::SLIME, 2));
        enemies.push_back(Enemy(434, 150, camera, map, ENEMY_TYPE::BAT, 1));
        enemies.push_back(Enemy(750, 480, camera, map, ENEMY_TYPE::SLIME, 2));
        enemies.push_back(Enemy(711, 224, camera, map, ENEMY_TYPE::SLIME, 2));
        enemies.push_back(Enemy(710, 348, camera, map, ENEMY_TYPE::BAT, 1));

        //player
        player.spawn(spawn_location, camera, map, enemies);
        player.set_can_wallrun(true);

        //NPC
        

        NPC golem = NPC(bn::fixed_point(155, 704), camera, NPC_TYPE::GOLEM, text_generator);
        Tooltip explain_attack = Tooltip(bn::fixed_point(243, 160),"Press 'B' to Attack", text_generator);
        Tooltip explain_wallrun = Tooltip(bn::fixed_point(454, 256),"Hold 'Up' to Wallrun", text_generator);

        StorySave first_plaque = StorySave(bn::fixed_point(323, 232), STORY_TYPE::BEGINNING, camera, text_generator);
        StorySave second_plaque = StorySave(bn::fixed_point(913, 488), STORY_TYPE::BEGINNING, camera, text_generator);
        StorySave third_plaque = StorySave(bn::fixed_point(746, 592), STORY_TYPE::BEGINNING, camera, text_generator);
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
            // 

            if(golem.check_trigger(player.pos()))
            {
                if(bn::keypad::up_pressed()){
                    player.set_listening(true);
                    golem.talk();
                }else if(!golem.is_talking()){
                    player.set_listening(false);
                }
            }
            else if(explain_attack.check_trigger(player.pos())){
                player.set_listening(true);
            }
            else if(explain_wallrun.check_trigger(player.pos())){
                player.set_listening(true);
            }
            else {
                player.set_listening(false);
            }

            first_plaque.update();
            second_plaque.update();
            third_plaque.update();
            golem.update();
            explain_attack.update();
            explain_wallrun.update();

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
            // BN_LOG(player.pos().x());
            // BN_LOG(player.pos().y());

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