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
#include "bn_sprite_items_penguin_sprite.h"
#include "variable_8x8_sprite_font.h"


namespace fe
{

    NPC::NPC(bn::fixed_point pos, bn::camera_ptr& camera, NPC_TYPE type, bn::sprite_text_generator& text_generator) :
        _pos(pos), _camera(camera), _type(type), _text_generator(text_generator)
    {
        _text_generator.set_bg_priority(0);
        
        if(_type == NPC_TYPE::GOLEM)
        {
            _sprite = bn::sprite_items::golem_sprite.create_sprite(_pos.x(), _pos.y());
            _action = bn::create_sprite_animate_action_forever(
                            _sprite.value(), 120, bn::sprite_items::golem_sprite.tiles_item(), 0,1);
            _lines = bn::span(_golem_lines);
        }else if(_type == NPC_TYPE::TORTOISE)
        {
            _sprite = bn::sprite_items::tortoise_sprite.create_sprite(_pos.x(), _pos.y());
            _action = bn::create_sprite_animate_action_forever(
                            _sprite.value(), 120, bn::sprite_items::tortoise_sprite.tiles_item(), 0,1);
            _lines = bn::span(_tortoise_lines);
        } else if(_type == NPC_TYPE::PENGUIN){
            _sprite = bn::sprite_items::penguin_sprite.create_sprite(_pos.x(), _pos.y());
            _action = bn::create_sprite_animate_action_forever(
                            _sprite.value(), 60, bn::sprite_items::penguin_sprite.tiles_item(), 0,1);
            _lines = bn::span(_penguin_lines);
        }
        _sprite.value().set_camera(_camera);
        _sprite.value().set_bg_priority(1);
        _sprite.value().set_z_order(2);
    }
    
    void NPC::update(){
        _action.value().update();
        if(_is_talking){
            if(_currentChar == _lines.at(_currentLine).size() * 2){
                if(bn::keypad::up_pressed() || bn::keypad::a_pressed())
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
            _text_generator.set_left_alignment();
            _text_sprites.clear();
            _text_generator.generate(-110, _text_y_limit, _currentChars, _text_sprites);
        } else if(_is_near_player && !_finished) {
            _text_generator.set_center_alignment();
            _text_sprites.clear();
            _text_generator.generate(0, _text_y_limit, "press 'up' to interact", _text_sprites);
        } else {
            _text_sprites.clear();
        }
    }

    bool NPC::check_trigger(bn::fixed_point pos){
        if(!_finished){
            if(bn::abs(_pos.x() - pos.x()) < 90){
                if(bn::abs(_pos.y() - pos.y()) < 90){
                    _is_near_player = true;
                    return true;
                }
            }
            _is_near_player = false;
        }
        return false;
    }

    void NPC::talk(){
        _is_talking = true;
    }

    bool NPC::is_talking(){
        return _is_talking;
    }
}