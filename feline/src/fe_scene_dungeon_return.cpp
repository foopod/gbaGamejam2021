#include "fe_scene_dungeon_return.h"

//butano
#include "bn_core.h"
#include "bn_log.h"
#include "bn_string.h"
#include "bn_keypad.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_palettes.h"
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
#include "bn_blending_actions.h"
#include "bn_sprite_actions.h"


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
#include "bn_affine_bg_items_dungeon_other.h"
#include "bn_regular_bg_items_dungeon_bg.h"
#include "bn_regular_bg_items_vines.h"

#include "bn_sprite_items_child.h"
#include "bn_sprite_items_glow.h"

#include "bn_sprite_text_generator.h"
#include "variable_8x8_sprite_font.h"

#include "bn_music_items.h"
#include "bn_music_actions.h"

namespace fe
{
    DungeonReturn::DungeonReturn(Player& player)
    : _player(&player){}

    Scene DungeonReturn::execute(bn::fixed_point spawn_location)
    {
        bn::camera_ptr camera = bn::camera_ptr::create(spawn_location.x(), spawn_location.y());

        bn::sprite_text_generator text_generator(variable_8x8_sprite_font);

        int kill_timer = 0;

        bn::music_items::mellowdy.play();
        bn::music::set_volume(0.2);
        
        // map
        bn::affine_bg_ptr map = bn::affine_bg_items::dungeon_other.create_bg(512, 512);
        bn::regular_bg_ptr map_bg = bn::regular_bg_items::dungeon_bg.create_bg(512, 512);
        bn::optional<bn::regular_bg_ptr> vines = bn::regular_bg_items::vines.create_bg(0, 0);
        map_bg.set_priority(2);
        map.set_priority(1);
        vines.value().set_priority(0);

        Level level = Level(map);

        // map.set_horizontal_scale(2);
        // map.set_vertical_scale(2);

        map.set_camera(camera);
        map_bg.set_camera(camera);
        vines.value().set_camera(camera);

        // bn::fixed max_cpu_usage;
        // int counter = 1;

        //Enemies
        bn::vector<Enemy, 16> enemies = {};
        // enemies.push_back(Enemy(408, 872, camera, map, ENEMY_TYPE::WALL,2 100));
        enemies.push_back(Enemy(734, 808, camera, map, ENEMY_TYPE::SLIMEO, 3));
        enemies.push_back(Enemy(734, 680, camera, map, ENEMY_TYPE::SLIMEO, 3));
        enemies.push_back(Enemy(254, 568, camera, map, ENEMY_TYPE::SLIMEO, 3));
        enemies.push_back(Enemy(244, 440, camera, map, ENEMY_TYPE::SLIMEO, 3));
        enemies.push_back(Enemy(660, 408, camera, map, ENEMY_TYPE::SLIMEO, 3));
        enemies.push_back(Enemy(750, 408, camera, map, ENEMY_TYPE::SLIMEO, 3));
        enemies.push_back(Enemy(400, 608, camera, map, ENEMY_TYPE::SLIMEO, 3));
        enemies.push_back(Enemy(106, 536, camera, map, ENEMY_TYPE::SLIMEO, 3));
        enemies.push_back(Enemy(451, 408, camera, map, ENEMY_TYPE::SLIMEO, 3));
        enemies.push_back(Enemy(852, 336, camera, map, ENEMY_TYPE::SLIMEO, 3));
        enemies.push_back(Enemy(440, 240, camera, map, ENEMY_TYPE::SLIMEO, 3));

        //player
        _player->spawn(spawn_location, camera, map, enemies);
        _player->set_healthbar_visibility(true);
        _player->set_can_teleport(true);

        // //NPC
        NPC tablet = NPC(bn::fixed_point(300, 872), camera, NPC_TYPE::TABLET, text_generator);
        NPC cage = NPC(bn::fixed_point(254, 208), camera, NPC_TYPE::CAGE, text_generator);
        Tooltip explain_teleport = Tooltip(bn::fixed_point(420, 880),"Tap 'L' to Teleport", text_generator);
        
        // bn::vector<StorySave, 4> saves = {};
        // saves.push_back(StorySave(bn::fixed_point(323, 232), STORY_TYPE::FIRST, camera, text_generator));
        // saves.push_back(StorySave(bn::fixed_point(913, 488), STORY_TYPE::SECOND, camera, text_generator));
        // saves.push_back(StorySave(bn::fixed_point(746, 592), STORY_TYPE::THIRD, camera, text_generator));
        // saves.push_back(StorySave(bn::fixed_point(210, 840), STORY_TYPE::CRACKED, camera, text_generator));
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

            if(tablet.check_trigger(_player->pos()))
            {
                if(bn::keypad::up_pressed()){
                    _player->set_listening(true);
                    tablet.talk();
                }else if(!tablet.is_talking()){
                    _player->set_listening(false);
                }
            }
            else if(cage.check_trigger(_player->pos()))
            {
                if(bn::keypad::up_pressed()){
                    _player->set_listening(true);
                    cage.talk();
                }else if(!cage.is_talking()){
                    _player->set_listening(false);
                }
            }
            else if(explain_teleport.check_trigger(_player->pos())){
                _player->set_listening(true);
                explain_teleport.update();
            }
            else {
                _player->set_listening(false);
            }
            // else if(explain_wallrun.check_trigger(player.pos())){
            //     player.set_listening(true);
            //     explain_wallrun.update();
            // }
            // else {
            //     player.set_listening(false);
            // }

            // for(StorySave& save : saves){
                
            //     if(save.check_trigger(player.pos()) && bn::keypad::up_pressed()){
            //         vines.reset();
            //         player.set_healthbar_visibility(false);
            //         save.execute_scene();
            //         vines = bn::regular_bg_items::vines.create_bg(0, 0);
            //         player.set_healthbar_visibility(true);
            //         vines.value().set_priority(0);
            //         vines.value().set_camera(camera);
            //     }
            //     if(bn::abs(save.pos().x() - camera.x()) < 200 && bn::abs(save.pos().y() - camera.y()) < 100){
            //         save.update();
            //     }
            // }
            tablet.update();
            cage.update();

            for(Enemy& enemy : enemies){
                if(bn::abs(enemy.pos().x() - camera.x()) < 200 && bn::abs(enemy.pos().y() - camera.y()) < 100){
                    enemy.update(_player->pos());
                } else {
                    enemy.set_visible(false);
                }
            }

            _player->update_position(map,level);
            _player->apply_animation_state();

            //  BN_LOG(bn::to_string<32>(_player->pos().x())+" " + bn::to_string<32>(_player->pos().y()));
            vines.value().set_position(bn::fixed_point((_player->pos().x()-100)/10,(_player->pos().y())/10));
            
            if(cage.finished_talking()){
                _player->set_listening(false);
                _player->delete_data();
                return Scene::RETURN_SKY;
            }

            if(_player->hp() < 1){
                _player->delete_data();
                return Scene::DEATH;
            }
            
            // BN_PROFILER_RESET();
            bn::core::update();
            // bn::profiler::show();
            
        }
    }
}