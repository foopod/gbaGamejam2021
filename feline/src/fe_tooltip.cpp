#include "fe_tooltip.h"

#include "bn_optional.h"
#include "bn_math.h"
#include "bn_log.h"
#include "bn_display.h"
#include "bn_keypad.h"

#include "bn_sprite_items_golem_sprite.h"
#include "bn_sprite_items_tortoise_sprite.h"
#include "bn_sprite_items_penguin_sprite.h"



namespace fe
{

    Tooltip::Tooltip(bn::fixed_point pos, bn::string_view line, bn::sprite_text_generator& text_generator) :
        _pos(pos), _line(line), _text_generator(text_generator)
    {
        _text_generator.set_left_alignment();
        _text_generator.set_bg_priority(0);
    }

    void Tooltip::update(){
        if(_is_talking){
            if(_currentChar == _line.size()){
                if(bn::keypad::a_pressed())
                {
                    _is_talking = false;
                    _finished = true;
                    _currentChar = 0;
                }
            } else {
                _currentChars = _line.substr(0,(_currentChar)+1);
                ++_currentChar;
            }
            _text_sprites.clear();
            _text_generator.generate(-110, _text_y_limit, _currentChars, _text_sprites);
        } else {
            _text_sprites.clear();
        }
    }

    bool Tooltip::check_trigger(bn::fixed_point player_pos){
        if(!_finished){
            if(bn::abs(_pos.x() - player_pos.x()) < 30){
                if(bn::abs(_pos.y() - player_pos.y()) < 30){
                    _is_talking = true;
                    return true;
                }
            }
        }
        return false;
    }
}