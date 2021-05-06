#include "fe_house_scene.h"

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

//fe code
#include "fe_level.h"
#include "fe_player.h"
#include "fe_scene.h"
#include "fe_npc.h"
#include "fe_npc_type.h"

//assets
#include "bn_sprite_items_cat.h"
#include "bn_affine_bg_items_house.h"
#include "bn_regular_bg_items_house_bg.h"


namespace fe
{
    Scene House::execute(bn::fixed_point spawn)
    {

        // player sprite
        bn::sprite_ptr cat_sprite = bn::sprite_items::cat.create_sprite(spawn.x(), spawn.y());
        cat_sprite.set_bg_priority(1);
        bn::camera_ptr camera = bn::camera_ptr::create(spawn.x(), spawn.y());

        //NPC
        NPC tortoise = NPC(bn::fixed_point(535, 304), camera, NPC_TYPE::TORTOISE);

        // map
        bn::regular_bg_ptr map_bg = bn::regular_bg_items::house_bg.create_bg(512, 512);
        bn::affine_bg_ptr map = bn::affine_bg_items::house.create_bg(512, 512);
        map_bg.set_priority(2);
        map.set_priority(1);
        fe::Level level = fe::Level(map);
        map.set_horizontal_scale(2);
        map.set_vertical_scale(2);

        // camera
        cat_sprite.set_camera(camera);
        map.set_camera(camera);
        map_bg.set_camera(camera);

        // player
        fe::Player player = fe::Player(spawn, cat_sprite, camera);
        while(true)
        {
            if(tortoise.near_player(player.pos()))
            {
                if(bn::keypad::a_pressed()){
                    player.set_listening(true);
                    tortoise.talk();
                }else if(!tortoise.is_talking()){
                    player.set_listening(false);
                }
            }
            tortoise.update();

            //elevator.update_position();
            player.update_position(map,level);
            player.apply_animation_state();
            // BN_LOG(bn::to_string<32>(player.pos().x())+" " + bn::to_string<32>(player.pos().y()));
            
            if(bn::keypad::up_pressed())
            {
                if(player.pos().x() < 720 && player.pos().x() > 700){
                    if(player.pos().y() < 320 && player.pos().y() > 280){
                        return Scene::HOUSE_SKY;
                    }
                }
            }
            
            
            bn::core::update();
        }
    }
}