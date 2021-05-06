#include "fe_npc.h"
#include "fe_npc_type.h"

#include "bn_optional.h"
#include "bn_math.h"
#include "bn_log.h"
#include "bn_display.h"
#include "bn_keypad.h"
#include "bn_sprite_text_generator.h"

#include "bn_sprite_items_golem_sprite.h"
#include "bn_sprite_items_tortoise_sprite.h"
#include "variable_8x8_sprite_font.h"


namespace fe
{
    //bn::sprite_ptr sprite = bn::sprite_items::golem_sprite.create_sprite(0, 0);
    bn::sprite_text_generator text_generator(variable_8x8_sprite_font);
    bn::vector<bn::sprite_ptr, 32> text_sprites;

    constexpr const bn::fixed text_y_inc = 14;
    constexpr const bn::fixed text_y_limit = (bn::display::height() / 2) - text_y_inc;

    NPC::NPC(bn::fixed_point pos, bn::camera_ptr camera, NPC_TYPE type) :
        _pos(pos), _camera(camera), _type(type)
    {
        
        
        if(_type == NPC_TYPE::GOLEM)
        {
            _sprite = bn::sprite_items::golem_sprite.create_sprite(_pos.x(), _pos.y());
            _sprite.value().set_horizontal_scale(2);
            _sprite.value().set_vertical_scale(2);
            _sprite.value().set_camera(_camera);
            _sprite.value().set_bg_priority(1);
            _action = bn::create_sprite_animate_action_forever(
                            _sprite.value(), 120, bn::sprite_items::golem_sprite.tiles_item(), 0,1);
            text_generator.set_left_alignment();
            text_generator.set_bg_priority(0);
            _lines = bn::span(_golem_lines);
        }

        if(_type == NPC_TYPE::TORTOISE)
        {
            _sprite = bn::sprite_items::tortoise_sprite.create_sprite(_pos.x(), _pos.y());
            _sprite.value().set_horizontal_scale(2);
            _sprite.value().set_vertical_scale(2);
            _sprite.value().set_camera(_camera);
            _sprite.value().set_bg_priority(1);
            _action = bn::create_sprite_animate_action_forever(
                            _sprite.value(), 120, bn::sprite_items::tortoise_sprite.tiles_item(), 0,1);
            text_generator.set_bg_priority(0);
            _lines = bn::span(_tortoise_lines);
        }
        
    }

    void NPC::update(){
        _action.value().update();
        if(_is_talking){
            if(_currentChar == _lines.at(_currentLine).size() * 2){
                if(bn::keypad::a_pressed())
                {
                    if(_currentLine == _lines.size()-1)
                    {
                        _is_talking = false;
                        _currentChars = "";
                        _currentChar = 0;
                        _currentLine = 0;
                    } else {
                        _currentLine += 1;
                        _currentChar = 0;
                    }
                }
            } else {
                _currentChars = _lines.at(_currentLine).substr(0,(_currentChar/2)+1);
                ++_currentChar;
            }
            text_sprites.clear();
            text_generator.set_left_alignment();
            text_generator.generate(-110, text_y_limit, _currentChars, text_sprites);
        } else if(_is_near_player) {
            text_sprites.clear();
            text_generator.set_center_alignment();
            text_generator.generate(0, text_y_limit, "'a' to interact", text_sprites);
        } else {
            text_sprites.clear();
        }
    }

    bool NPC::near_player(bn::fixed_point pos){
        if(bn::abs(_pos.x() - pos.x()) < 30){
            if(bn::abs(_pos.y() - pos.y()) < 30){
                _is_near_player = true;
                return true;
            }
        }
        _is_near_player = false;
        return false;
    }

    void NPC::talk(){
        _is_talking = true;
    }

    bool NPC::is_talking(){
        return _is_talking;
    }
}