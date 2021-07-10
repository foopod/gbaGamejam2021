#include "fe_scene_sky_return.h"

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
#include "fe_tooltip.h"
#include "fe_npc.h"
#include "fe_npc_type.h"

//assets
#include "bn_sprite_items_cat_sprite.h"
#include "bn_affine_bg_items_cliffs2.h"
#include "bn_regular_bg_items_cliffs_bg.h"

#include "bn_sprite_text_generator.h"
#include "variable_8x8_sprite_font.h"

#include "bn_music_items.h"
#include "bn_music_actions.h"

namespace fe
{

     SkyReturn::SkyReturn(Player& player)
    : _player(&player){}

    Scene SkyReturn::execute(bn::fixed_point spawn_location)
    {
        bn::camera_ptr camera = bn::camera_ptr::create(spawn_location.x(), spawn_location.y());

        bn::sprite_text_generator text_generator(variable_8x8_sprite_font);

        bn::music_items::piana.play();
        bn::music::set_volume(0.6);

        //NPC
        NPC penguin = NPC(bn::fixed_point(231, 376), camera, NPC_TYPE::PENGUIN2, text_generator);
        NPC jeremy = NPC(bn::fixed_point(880, 208), camera, NPC_TYPE::JEREMY, text_generator);
        NPC tortoise = NPC(bn::fixed_point(150, 264), camera, NPC_TYPE::TORTOISE2, text_generator);
        NPC frog = NPC(bn::fixed_point(600, 616), camera, NPC_TYPE::FROG, text_generator);
        Tooltip explain_telejump = Tooltip(bn::fixed_point(385, 580),"Tap 'A' in the air to telejump", text_generator);

        // map
        bn::affine_bg_ptr map = bn::affine_bg_items::cliffs2.create_bg(512, 512);
        bn::regular_bg_ptr map_bg = bn::regular_bg_items::cliffs_bg.create_bg(512, 512);
        map_bg.set_priority(2);
        map.set_priority(1);
        fe::Level level = fe::Level(map);

        // camera
        map_bg.set_camera(camera);
        map.set_camera(camera);
        
        // bn::fixed max_cpu_usage;
        // int counter = 1;

        bn::vector<Enemy, 16> enemies = {};

        // _player
        _player->spawn(spawn_location, camera, map, enemies);
        _player->set_can_teleport(true);
        _player->set_healthbar_visibility(true);
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

            if(penguin.check_trigger(_player->pos()))
            {
                if(bn::keypad::up_pressed()){
                    _player->set_listening(true);
                    penguin.talk();
                }else if(!penguin.is_talking()){
                    _player->set_listening(false);
                }
            } 
            else if(tortoise.check_trigger(_player->pos()))
            {
                if(bn::keypad::up_pressed()){
                    _player->set_listening(true);
                    tortoise.talk();
                }else if(!tortoise.is_talking()){
                    _player->set_listening(false);
                }
            }
            else if(jeremy.check_trigger(_player->pos()))
            {
                if(bn::keypad::up_pressed()){
                    _player->set_listening(true);
                    jeremy.talk();
                }else if(!jeremy.is_talking()){
                    _player->set_listening(false);
                }
            }
            else if(frog.check_trigger(_player->pos()))
            {
                if(bn::keypad::up_pressed()){
                    _player->set_listening(true);
                    frog.talk();
                }else if(!frog.is_talking()){
                    _player->set_listening(false);
                }
            }
            else if(explain_telejump.check_trigger(_player->pos())){
                _player->set_listening(true);
                explain_telejump.update();
            }
            else {
                _player->set_listening(false);
            }
            
            penguin.update();
            tortoise.update();
            jeremy.update();
            frog.update();

            map_bg.set_x(map_bg.x() + 0.3);

            _player->update_position(map, level);
            _player->apply_animation_state();
            // BN_LOG(bn::to_string<32>(_player->pos().x())+" " + bn::to_string<32>(_player->pos().y()));
            
            // if(bn::keypad::up_pressed())
            // {
            //     if(_player->pos().x() < 185 && _player->pos().x() > 155){
            //         if(_player->pos().y() < 644 && _player->pos().y() > 614){
            //             _player->delete_data();
            //             return Scene::SKY_DUNGEON;
            //         }
            //     }
            // }

            if(_player->pos().x() >892 &&  _player->pos().y() > 414 ){
                _player->delete_data();
                return Scene::RETURN_SKY;
            }

            if(jeremy.finished_talking()){
                _player->set_hp(9);
                if(_player->pos().x() > 800 &&  _player->pos().x() < 830  && _player->pos().y() == 136){
                    _player->delete_data();
                    return Scene::LAB;
                }
            }
            
            
            bn::core::update();
        }
    }
}