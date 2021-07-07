#include "fe_scene_dungeon.h"

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
#include "bn_affine_bg_items_dungeon_2x.h"
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
    Dungeon::Dungeon(Player& player)
    : _player(&player){}

    Scene Dungeon::execute(bn::fixed_point spawn_location)
    {
        // spawn_location = bn::fixed_point(760, 900);

        bn::music_items::mellowdy.play();
        bn::music::set_volume(0.2);

        bn::camera_ptr camera = bn::camera_ptr::create(spawn_location.x(), spawn_location.y());

        bn::sprite_text_generator text_generator(variable_8x8_sprite_font);

        bn::sprite_ptr child = bn::sprite_items::child.create_sprite(845, 936);
        child.set_bg_priority(1);
        child.put_above();
        child.set_camera(camera);
        bn::sprite_ptr glow = bn::sprite_items::glow.create_sprite(845, 947);

        glow.set_camera(camera);
        glow.set_bg_priority(0);
        glow.set_scale(0.1);
        bn::optional<bn::sprite_scale_to_action> scale_action;
        bn::optional<bn::blending_fade_alpha_to_action> intensity_action;
        glow.put_above();

        int kill_timer = 0;

        // bn::music_items::house.play();
        
        // map
        bn::affine_bg_ptr map = bn::affine_bg_items::dungeon_2x.create_bg(512, 512);
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
        enemies.push_back(Enemy(290, 169, camera, map, ENEMY_TYPE::SLIME, 2));
        enemies.push_back(Enemy(434, 150, camera, map, ENEMY_TYPE::BAT, 1));
        enemies.push_back(Enemy(750, 480, camera, map, ENEMY_TYPE::SLIME, 2));
        enemies.push_back(Enemy(711, 224, camera, map, ENEMY_TYPE::SLIME, 2));
        enemies.push_back(Enemy(710, 348, camera, map, ENEMY_TYPE::BAT, 1));
        enemies.push_back(Enemy(412, 440, camera, map, ENEMY_TYPE::SLIME, 2));
        enemies.push_back(Enemy(922, 720, camera, map, ENEMY_TYPE::BAT, 1));
        enemies.push_back(Enemy(337, 792, camera, map, ENEMY_TYPE::SLIME, 2));
        // enemies.push_back(Enemy(885, 936, camera, map, ENEMY_TYPE::BOSS, 10));

        //_player
        _player->spawn(spawn_location, camera, map, enemies);
        _player->set_healthbar_visibility(true);

        //NPC
        NPC golem = NPC(bn::fixed_point(155, 704), camera, NPC_TYPE::GOLEM, text_generator);
        Tooltip explain_attack = Tooltip(bn::fixed_point(243, 160),"Press 'B' to Attack", text_generator);

        bn::vector<StorySave, 4> saves = {};
        saves.push_back(StorySave(bn::fixed_point(323, 232), STORY_TYPE::FIRST, camera, text_generator));
        saves.push_back(StorySave(bn::fixed_point(913, 488), STORY_TYPE::SECOND, camera, text_generator));
        saves.push_back(StorySave(bn::fixed_point(746, 592), STORY_TYPE::THIRD, camera, text_generator));
        saves.push_back(StorySave(bn::fixed_point(210, 840), STORY_TYPE::CRACKED, camera, text_generator));
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

            
            if(_player->hp() <= 0){
                _player->delete_data();
                return Scene::DEATH;
            }
            

            if(golem.check_trigger(_player->pos()))
            {
                if(bn::keypad::up_pressed()){
                    _player->set_listening(true);
                    golem.talk();
                }else if(!golem.is_talking()){
                    _player->set_listening(false);
                }
            }
            else if(explain_attack.check_trigger(_player->pos())){
                _player->set_listening(true);
                explain_attack.update();
            }
            else {
                _player->set_listening(false);
            }

            for(StorySave& save : saves){
                
                if(save.check_trigger(_player->pos()) && bn::keypad::up_pressed()){
                    vines.reset();
                    _player->set_healthbar_visibility(false);
                    save.execute_scene();
                    vines = bn::regular_bg_items::vines.create_bg(0, 0);
                    _player->set_healthbar_visibility(true);
                    vines.value().set_priority(0);
                    vines.value().set_camera(camera);
                }
                if(bn::abs(save.pos().x() - camera.x()) < 200 && bn::abs(save.pos().y() - camera.y()) < 100){
                    save.update();
                }
            }
            golem.update();

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

            //door
            if(bn::keypad::up_pressed() && !_player->is_listening())
            {
                if(_player->pos().x() < 160 && _player->pos().x() > 140){
                    if(_player->pos().y() < 200 && _player->pos().y() > 188){
                        _player->delete_data();
                        return Scene::DUNGEON_SKY;
                    }
                }

            }
            
            if(_player->pos().y() == 944 && _player->pos().x() > 790){
                _player->set_healthbar_visibility(false);
                scale_action = bn::sprite_scale_to_action(glow, 60, 2);
                map.set_blending_enabled(true);
                map_bg.set_blending_enabled(true);
                vines.value().set_blending_enabled(true);
                glow.set_blending_enabled(true);
                intensity_action = bn::blending_fade_alpha_to_action(60, 1);
            }

            if(scale_action.has_value()){
                if(!scale_action.value().done()){
                    scale_action.value().update();
                } 
                
            }

            if(intensity_action.has_value()){
                if(!intensity_action.value().done()){
                    intensity_action.value().update();
                    kill_timer++;
                    if(kill_timer > 60){
                        _player->delete_data();
                        return Scene::OTHER;
                    }
                }
            }
            
            // BN_LOG(_player->hp());
            
            // BN_PROFILER_RESET();
            bn::core::update();
            // bn::profiler::show();
            
        }
    }
}