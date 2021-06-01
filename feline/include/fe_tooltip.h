#ifndef BF_TOOLTIP_H
#define BF_TOOLTIP_H

#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_optional.h"
#include "bn_span.h"
#include "bn_display.h"
#include "bn_vector.h"
#include "bn_string.h"
#include "bn_string_view.h"
#include "bn_sprite_animate_actions.h"

#include "bn_sprite_text_generator.h"


namespace fe
{
    class Tooltip
    {
        private:
            bn::fixed_point _pos;
            bool _is_talking = false;
            bool _finished = false;
            bn::sprite_text_generator& _text_generator;
            const bn::fixed _text_y_inc = 14;
            const bn::fixed _text_y_limit = (bn::display::height() / 2) - _text_y_inc;
            bn::vector<bn::sprite_ptr, 32> _text_sprites;


            bn::string_view _line;
            int _currentChar = 0;
            bn::string_view _currentChars = "";

        public:
            Tooltip(bn::fixed_point pos, bn::string_view line, bn::sprite_text_generator& text_generator);
            void update();
            bool check_trigger(bn::fixed_point pos);
            
    };
}

#endif