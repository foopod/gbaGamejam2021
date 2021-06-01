#include "bn_core.h"
#include "bn_math.h"
#include "bn_sprite_builder.h"
#include "bn_sprite_ptr.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_regular_bg_ptr.h"
#include "bn_fixed_point.h"
#include "bn_regular_bg_item.h"
#include "bn_affine_bg_ptr.h"
#include "bn_affine_bg_item.h"
#include "bn_affine_bg_tiles_ptr.h"
#include "bn_affine_bg_map_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_camera_actions.h"
#include "bn_sprite_animate_actions.h"

#include "bn_string_view.h"
#include "bn_vector.h"
#include "bn_log.h"
#include "bn_sprite_text_generator.h"

#include "fe_hitbox.h"
#include "fe_scene_dungeon.h"
#include "fe_scene_sky.h"
#include "fe_scene_house.h"
#include "fe_scene_loading.h"
#include "fe_scene.h"

#include "bn_sprite_items_cat_sprite.h"

int main()
{
    bn::core::init();
    fe::Scene scene = fe::Scene::SKY_DUNGEON;
    fe::Loading loading = fe::Loading();

    bn::sprite_ptr cat_sprite = bn::sprite_items::cat_sprite.create_sprite(0,0);
    fe::Player player = fe::Player(cat_sprite);

    while(true)
    {
        if(scene == fe::Scene::HOUSE_START){
            fe::House house = fe::House();
            scene = house.execute(player, bn::fixed_point(288, 292));
        } 
        else if(scene == fe::Scene::HOUSE_SKY){
            fe::Sky sky = fe::Sky();
            scene = sky.execute(player, bn::fixed_point(213, 728));
        } 
        else if(scene == fe::Scene::SKY_DUNGEON)
        {
            fe::Dungeon dungeon = fe::Dungeon();
            scene = dungeon.execute(player, bn::fixed_point(150, 192));
        } 
        else if(scene == fe::Scene::DUNGEON_SKY)
        {
            fe::Sky sky = fe::Sky();
            scene = sky.execute(player, bn::fixed_point(67, 968));
        }
        else if(scene == fe::Scene::SKY_HOUSE)
        {
            fe::House house = fe::House();
            scene = house.execute(player, bn::fixed_point(710, 312));
        }
        cat_sprite.set_visible(false);
        loading.execute();
        bn::core::update();
        
    }
    
}
