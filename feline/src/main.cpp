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
#include "bn_camera_actions.h"
#include "bn_sprite_animate_actions.h"

#include "bn_sprite_items_cat.h"
#include "bn_sprite_items_blimp_top.h"
#include "bn_sprite_items_blimp_bottom.h"
#include "bn_affine_bg_items_map.h"
#include "bn_music_items.h"

#include "bn_string_view.h"
#include "bn_vector.h"
#include "bn_sprite_text_generator.h"
#include "variable_8x16_sprite_font.h"

#include "fe_hitbox.h"
#include "fe_player.h"
#include "fe_elevator.h"

     

int main()
{
    bn::core::init();

    bn::fixed_point init_pos = bn::fixed_point(120, 432);

    // player sprite
    bn::sprite_ptr cat_sprite = bn::sprite_items::cat.create_sprite(init_pos.x(), init_pos.y());
    bn::camera_ptr camera = bn::camera_ptr::create(init_pos.x(), init_pos.y());

    // elevator sprite
    bn::sprite_ptr top_elevator_sprite = bn::sprite_items::blimp_top.create_sprite(0, 0);
    bn::sprite_ptr bottom_elevator_sprite = bn::sprite_items::blimp_bottom.create_sprite(0, 0);
    top_elevator_sprite.set_camera(camera);
    bottom_elevator_sprite.set_camera(camera);
    fe::Elevator elevator = fe::Elevator(bn::fixed_point(480,544), top_elevator_sprite, bottom_elevator_sprite, 600);

    // map
    bn::affine_bg_ptr map_bg = bn::affine_bg_items::map.create_bg(256, 256);

    cat_sprite.set_camera(camera);
    map_bg.set_camera(camera);

    // player
    fe::Player player = fe::Player(init_pos, cat_sprite, camera);

    // text set up
    bn::sprite_text_generator text_generator(variable_8x16_sprite_font);
    bn::vector<bn::sprite_ptr, 32> text_sprites;
    bn::music_items::song.play(0.3);
    
    while(true)
    {

        elevator.update_position();
        player.update_position(map_bg, elevator);
        player.apply_animation_state();
        
        text_sprites.clear();
        //text_generator.generate(0, -40, bn::to_string<32>(elevator.pos().y()) , text_sprites);
        
        bn::core::update();
    }
}
