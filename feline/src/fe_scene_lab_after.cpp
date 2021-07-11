#include "fe_scene_lab_after.h"

//butano
#include "bn_core.h"
#include "bn_log.h"
#include "bn_math.h"
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


#include "bn_blending_actions.h"
#include "bn_sprite_actions.h"

//assets
#include "bn_sprite_items_cat_sprite.h"
#include "bn_affine_bg_items_lab.h"
#include "bn_regular_bg_items_lab_bg.h"
#include "bn_regular_bg_items_white.h"

#include "bn_sprite_items_child.h"
#include "bn_sprite_items_blimp.h"

#include "bn_sprite_text_generator.h"
#include "variable_8x8_sprite_font.h"

#include "fe_tooltip.h"

#include "bn_music_items.h"
#include "bn_music_actions.h"
#include "bn_blending_actions.h"

namespace fe
{
    LabAfter::LabAfter(Player& player)
    : _player(&player){}

    Scene LabAfter::execute(bn::fixed_point spawn_location)
    {
        bn::camera_ptr camera = bn::camera_ptr::create(spawn_location.x(), spawn_location.y());

        bn::sprite_text_generator text_generator(variable_8x8_sprite_font);
        // bn::music_items::sanctuary.play();
        // bn::music::set_volume(0.8);

        // map
        bn::affine_bg_ptr map = bn::affine_bg_items::lab.create_bg(512, 512);
        bn::regular_bg_ptr bg = bn::regular_bg_items::lab_bg.create_bg(0, 0);
        bn::optional<bn::regular_bg_ptr> white = bn::regular_bg_items::white.create_bg(0, 0);
        map.set_priority(1);
        white.value().set_priority(0);
        white.value().set_blending_enabled(true);
        fe::Level level = fe::Level(map);

        for(int i = 0; i < 60; ++i){
            bn::core::update();
        }
        
        bn::blending_transparency_alpha_to_action transparency_action = bn::blending_transparency_alpha_to_action(180, 0);

        // int kill_timer = 0;

        // camera
        // bg.set_camera(camera);
        bg.set_camera(camera);
        map.set_camera(camera);
        
        // bn::fixed max_cpu_usage;
        // int counter = 1;
        NPC fam = NPC(bn::fixed_point(543, 696), camera, NPC_TYPE::FAM, text_generator);


        bn::vector<Enemy, 16> enemies = {};
      
        // _player
        _player->spawn(spawn_location, camera, map, enemies);
        _player->set_healthbar_visibility(true);
        _player->set_can_teleport(true);

        


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

            if(!transparency_action.done()){
                transparency_action.update();
            }

            if(fam.check_trigger(_player->pos()))
            {
                if(bn::keypad::up_pressed()){
                    _player->set_listening(true);
                    fam.talk();
                }else if(!fam.is_talking()){
                    _player->set_listening(false);
                }
            }
            fam.update();

            if(fam.finished_talking()){
                _player->set_listening(false);
                _player->delete_data();
                return Scene::END;
            }

            _player->update_position(map, level);
                _player->apply_animation_state();

            // BN_LOG(bn::to_string<32>(_player->pos().x())+", " + bn::to_string<32>(_player->pos().y()));
            
            bn::core::update();
        }
    }
}