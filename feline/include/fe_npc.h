#ifndef BF_NPC_H
#define BF_NPC_H

#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_optional.h"
#include "bn_span.h"
#include "bn_string.h"
#include "bn_string_view.h"
#include "bn_sprite_animate_actions.h"

#include "fe_npc_type.h"

namespace fe
{
    class NPC
    {
        private:
            NPC_TYPE _type;
            bn::fixed_point _pos;
            bn::camera_ptr _camera;
            bn::optional<bn::sprite_ptr> _sprite;
            bn::optional<bn::sprite_animate_action<2>> _action;
            bool _is_talking = false;
            bool _is_near_player = false;

            bn::span<bn::string_view> _lines;
            int _currentLine = 0;
            int _currentChar = 0;
            bn::string_view _currentChars = "";

            bn::string_view _golem_lines[14] = {
                ". . .",
                "* groans *",
                "S o m e o n e  t h e r e  ?",
                "Hello young one.",
                "* cough *",
                "Something troubles you?",
                ". . .",
                "Oh.",
                "Thats no good.",
                "I am gravely sorry.",
                "I don't know if they came passed me.",
                "I haven't awoken for many centuries now.",
                "You see..",
                "*he motions towards the sprawling cobwebs*"
            };

            bn::string_view _tortoise_lines[12] = {
                "To be honest...",
                "I have no idea what happened here.",
                "It doesn't look like they took anything.",
                ". . .",
                "Oh.",
                "Oh my . . .",
                "You have kids and a wife?",
                "Glad they weren't here then.",
                ". . . ",
                "Oh dear, they were.",
                "These bastards can't have gone far.",
                "The tortoise brigade is on the case."
            };

        public:
            NPC(bn::fixed_point pos, bn::camera_ptr camera, NPC_TYPE type);
            void update();
            bn::fixed_point pos();
            bool near_player(bn::fixed_point player_pos);
            bool is_talking();
            void talk();
            
    };
}

#endif