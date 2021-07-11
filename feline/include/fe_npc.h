#ifndef BF_NPC_H
#define BF_NPC_H

#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_optional.h"
#include "bn_span.h"
#include "bn_vector.h"
#include "bn_display.h"
#include "bn_string.h"
#include "bn_string_view.h"
#include "bn_sprite_animate_actions.h"

#include "bn_sprite_text_generator.h"

#include "fe_npc_type.h"

namespace fe
{
    class NPC
    {
        private:
            NPC_TYPE _type;
            bn::fixed_point _pos;
            bn::camera_ptr& _camera;
            bn::optional<bn::sprite_ptr> _sprite;
            bn::optional<bn::sprite_animate_action<2>> _action;
            bool _is_talking = false;
            bool _is_near_player = false;
            bool _finished = false;
            bool _has_spoken_once = false;

            bool _hidden = false;

            bn::vector<bn::sprite_ptr, 32> _text_sprites;
            bn::sprite_text_generator& _text_generator;

            const bn::fixed _text_y_inc = 14;
            const bn::fixed _text_y_limit = (bn::display::height() / 2) - _text_y_inc;

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
                "*they motion toward the sprawling vines*"
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
                "Oh dear, they were here?",
                "These bastards can't have gone far.",
                "The tortoise brigade is on the case."
            };

            bn::string_view _penguin_lines[3] = {
                "Hurry!",
                "I saw them head down.",
                "They were heading for the caves!"
            };

            bn::string_view _penguin_lines2[3] = {
                "Oh good!",
                "You found buddy.",
                "Let me know if there is anything I can do."
            };

            bn::string_view _tablet_lines[4] = {
                "You feel warm",
                "You feel energized",
                "Almost like the energy could burst from",
                ".. within you at any moment",
            };

            bn::string_view _cage_lines[7] = {
                "DAD!!!",
                "You found me!",
                "They took mum and the others",
                "We have to save them!",
                "But we are going to need reinforcements",
                "We should find Uncle Jeremy",
                "Come on Dad, Let's Go!",
            };

            bn::string_view _jeremy_lines[15] = {
                "Damnit",
                "Those Bastards!",
                "What's the plan?",
                "...",
                "Well you know I can't do that",
                "No!",
                "Look. I want to help. I do.",
                "I would do anything.",
                "But I won't do that.",
                "Take the airship. But I can't come with you."
                "...",
                "Good Luck boys!",
                "Oh..",
                "And here.. take some of my lives.",
                "You are going to need them more than me."
            };
            
            bn::string_view _tortoise2_lines[9] = {
                "Sorry boys",
                "This is a police investigation.",
                "Oh hey, it's you.",
                "We have locked down the crime scene",
                "And we are doing a thorough inverstigation",
                "...",
                "What? You know where they are?!?!",
                "No! Don't go there.",
                "The tortoise brigade is on the case."
            };

            bn::string_view _frog_lines[4] = {
                "Ribbit",
                "Did you see who knocked over my crates?",
                "...",
                "Hm... Well if you do. Let me know.",
            };

            bn::string_view _girls_lines[4] = {
                "My..",
                "My goodness..",
                "What are they doing to you?",
                "I have to get to the bottom of this",
            };

            bn::string_view _lab_pc_lines[8] = {
                "They are..",
                "No.. They..",
                "...",
                "They are experimenting on cats.",
                "By modifying DNA, they are...",
                "No.. They can't..",
                "I have to get the kids out of here",
                "I have to find Eileen",
            };

            bn::string_view _potion_lines[2] = {
                "Ew.. What is that?",
                "I should probably just leave it be.",
            };

            bn::string_view _computers_lines[1] = {
                "I wonder what they need all this for?",
            };

            bn::string_view _pewpew_lines[6] = {
                "They left this one unlocked",
                "Oooohh...",
                "It has Space Invaders",
                "pew pew",
                "pew pew ... pew",
                "pew"
            };

            bn::string_view _mutant_lines[6] = {
                "Eileen...",
                "Is that you?",
                "Kids?",
                "...",
                "...",
                "..."
            };

            bn::string_view _fam_lines[7] = {
                "Eileen!",
                "Thank goodness you are okay",
                "That was horrifying",
                "I was so scared that I had lost you",
                "All of you!",
                "...",
                "Let's get the others and get out of here",
            };

        public:
            NPC(bn::fixed_point pos, bn::camera_ptr& camera, NPC_TYPE type, bn::sprite_text_generator& text_generator);
            void update();
            bn::fixed_point pos();
            bool check_trigger(bn::fixed_point player_pos);
            bool is_talking();
            void talk();
            bool finished_talking();
            void set_is_hidden(bool is_hidden);
            bool hidden();
    };
}

#endif