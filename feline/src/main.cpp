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

#include "bn_sprite_items_cat.h"
#include "bn_sprite_items_blimp_top.h"
#include "bn_sprite_items_blimp_bottom.h"
#include "bn_affine_bg_items_map2.h"
#include "bn_regular_bg_items_dungeon_bg.h"
#include "bn_regular_bg_items_vines.h"
#include "bn_music_items.h"

#include "bn_string_view.h"
#include "bn_vector.h"
#include "bn_log.h"
#include "bn_sprite_text_generator.h"
#include "variable_8x16_sprite_font.h"

#include "fe_hitbox.h"
#include "fe_player.h"
#include "fe_elevator.h"
#include "fe_level.h"
#include "fe_dungeon_scene.h"
#include "fe_sky_scene.h"
#include "fe_loading_scene.h"
#include "fe_scene.h"

int main()
{
    bn::core::init();
    fe::Scene scene = fe::Scene::SKY;
    fe::Loading loading = fe::Loading();

    while(true)
    {
        if(scene == fe::Scene::SKY){
            fe::Sky sky = fe::Sky();
            scene = sky.execute();
        } 
        else if(scene == fe::Scene::DUNGEON)
        {
            fe::Dungeon dungeon = fe::Dungeon();
            scene = dungeon.execute();
        }

        loading.execute();
        bn::core::update();
        
    }
    
}
