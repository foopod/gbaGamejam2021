#ifndef BF_STORY_SAVE_H
#define BF_STORY_SAVE_H

#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_optional.h"
#include "bn_span.h"
#include "bn_vector.h"
#include "bn_display.h"
#include "bn_string.h"
#include "bn_string_view.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_animate_actions.h"

#include "bn_sprite_text_generator.h"

#include "fe_story_type.h"

namespace fe
{
    class StorySave
    {
        private:
            STORY_TYPE _type; 
            bn::fixed_point _pos;
            bn::camera_ptr& _camera;
            bn::optional<bn::sprite_ptr> _sprite;
            bn::optional<bn::sprite_animate_action<3>> _action;
            bool _is_active = false;
            bool _is_near_player = false;
            bool _is_translate = false;

            const bn::fixed _text_y_inc = 14;
            const bn::fixed _text_y_limit = (bn::display::height() / 2) - _text_y_inc;

            bn::fixed _y = 44;

            bn::optional<bn::regular_bg_ptr> _slab;
            bn::optional<bn::regular_bg_ptr> _slab_t;

            bn::sprite_text_generator& _text_generator;
            bn::vector<bn::sprite_ptr, 12> _text_sprites;

        public:
            StorySave(bn::fixed_point pos, STORY_TYPE type, bn::camera_ptr& camera, bn::sprite_text_generator& text_generator);
            void update();
            bool check_trigger(bn::fixed_point player_pos);
            // bool is_active();
            void execute_scene();
            bn::fixed_point pos();
    };
}

#endif