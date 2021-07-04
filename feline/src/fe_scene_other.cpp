#include "fe_scene_other.h"

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
#include "bn_affine_bg_items_other.h"
#include "bn_affine_bg_items_other.h"
#include "bn_regular_bg_items_other_bg.h"

#include "bn_sprite_items_child.h"
#include "bn_sprite_items_glow.h"

#include "bn_sprite_text_generator.h"
#include "variable_8x8_sprite_font.h"

#include "bn_music_items.h"
#include "bn_music_actions.h"

namespace fe
{
    Scene Other::execute(Player player, bn::fixed_point spawn_location)
    {
        bn::camera_ptr camera = bn::camera_ptr::create(spawn_location.x(), spawn_location.y());

        bn::sprite_text_generator text_generator(variable_8x8_sprite_font);
        bn::music_items::sanctuary.play();
        bn::music::set_volume(1);

        // map
        bn::affine_bg_ptr map = bn::affine_bg_items::other.create_bg(512, 512);
        bn::regular_bg_ptr bg = bn::regular_bg_items::other_bg.create_bg(512, 100);
        map.set_priority(1);
        fe::Level level = fe::Level(map);

        bn::sprite_ptr child = bn::sprite_items::child.create_sprite(903, 144);
        child.set_bg_priority(1);
        child.put_above();
        child.set_camera(camera);
        bn::sprite_ptr glow = bn::sprite_items::glow.create_sprite(903, 152);
        
        glow.set_camera(camera);
        glow.set_bg_priority(0);
        glow.set_scale(0.1);
        bn::optional<bn::sprite_scale_to_action> scale_action;
        bn::optional<bn::blending_fade_alpha_to_action> intensity_action;
        glow.put_above();

        int kill_timer = 0;

        // camera
        // bg.set_camera(camera);
        bg.set_camera(camera);
        map.set_camera(camera);
        
        // bn::fixed max_cpu_usage;
        // int counter = 1;
        bn::vector<Enemy, 16> enemies = {};
        enemies.push_back(Enemy(576, 576, camera, map, ENEMY_TYPE::BOSS, 3));
        enemies.push_back(Enemy(673, 384, camera, map, ENEMY_TYPE::BOSS, 3));
        enemies.push_back(Enemy(292, 288, camera, map, ENEMY_TYPE::BOSS, 3));
        enemies.push_back(Enemy(337, 104, camera, map, ENEMY_TYPE::BOSS, 3));

        // player
        player.spawn(spawn_location, camera, map, enemies);
        player.set_healthbar_visibility(true);
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

            for(Enemy& enemy : enemies){
                if(bn::abs(enemy.pos().x() - camera.x()) < 200 && bn::abs(enemy.pos().y() - camera.y()) < 100){
                    enemy.update(player.pos());
                } else {
                    enemy.set_visible(false);
                }
            }


            player.update_position(map, level);
            player.apply_animation_state();
            // BN_LOG(bn::to_string<32>(player.pos().x())+", " + bn::to_string<32>(player.pos().y()));
            
            if(player.pos().y() > 700){
                return Scene::OTHER;
            }

            if(player.pos().y() == 152 && player.pos().x() > 870){
                player.set_healthbar_visibility(false);
                scale_action = bn::sprite_scale_to_action(glow, 60, 2);
                map.set_blending_enabled(true);
                bg.set_blending_enabled(true);
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
                        return Scene::OTHER_DUNGEON;
                    }
                }
            }
            
            bn::core::update();
        }
    }
}