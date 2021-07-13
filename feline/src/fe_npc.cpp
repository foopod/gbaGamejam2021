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
#include "bn_sprite_items_jeremy.h"
#include "bn_sprite_items_pot_frog.h"
#include "bn_sprite_items_cage.h"
#include "bn_sprite_items_fam.h"
#include "bn_sprite_items_mutant.h"
#include "variable_8x8_sprite_font.h"
#include "bn_sprite_items_stone_plaque.h"

#include "bn_sound_items.h"

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
                            _sprite.value(), 20, bn::sprite_items::penguin_sprite.tiles_item(), 0,1);
            _lines = bn::span(_penguin_lines);
        } else if(_type == NPC_TYPE::PENGUIN2){
            _sprite = bn::sprite_items::penguin_sprite.create_sprite(_pos.x(), _pos.y());
            _action = bn::create_sprite_animate_action_forever(
                            _sprite.value(), 20, bn::sprite_items::penguin_sprite.tiles_item(), 0,1);
            _lines = bn::span(_penguin_lines2);
        }else if(_type == NPC_TYPE::TABLET){
            _sprite = bn::sprite_items::stone_plaque.create_sprite(_pos.x(), _pos.y());
            _action = bn::create_sprite_animate_action_forever(
                            _sprite.value(), 20, bn::sprite_items::stone_plaque.tiles_item(), 0,1);
            _lines = bn::span(_tablet_lines);
        }
        else if(_type == NPC_TYPE::CAGE){
            _sprite = bn::sprite_items::cage.create_sprite(_pos.x(), _pos.y());
            _action = bn::create_sprite_animate_action_forever(
                            _sprite.value(), 120, bn::sprite_items::cage.tiles_item(), 0,1);
            _lines = bn::span(_cage_lines);
        }
        else if(_type == NPC_TYPE::JEREMY){
            _sprite = bn::sprite_items::jeremy.create_sprite(_pos.x(), _pos.y());
            _action = bn::create_sprite_animate_action_forever(
                            _sprite.value(), 120, bn::sprite_items::jeremy.tiles_item(), 0,1);
            _lines = bn::span(_jeremy_lines);
        }
        else if(_type == NPC_TYPE::TORTOISE2){
            _sprite = bn::sprite_items::tortoise_sprite.create_sprite(_pos.x(), _pos.y());
            _action = bn::create_sprite_animate_action_forever(
                            _sprite.value(), 120, bn::sprite_items::tortoise_sprite.tiles_item(), 0,1);
            _lines = bn::span(_tortoise2_lines);
        }
        else if(_type == NPC_TYPE::FROG){
            _sprite = bn::sprite_items::pot_frog.create_sprite(_pos.x(), _pos.y());
            _action = bn::create_sprite_animate_action_forever(
                            _sprite.value(), 120, bn::sprite_items::pot_frog.tiles_item(), 0,1);
            _lines = bn::span(_frog_lines);
        }
        else if(_type == NPC_TYPE::FAM){
            _sprite = bn::sprite_items::fam.create_sprite(_pos.x(), _pos.y());
            _action = bn::create_sprite_animate_action_forever(
                            _sprite.value(), 20, bn::sprite_items::fam.tiles_item(), 0,1);
            _lines = bn::span(_fam_lines);
        }
        else if(_type == NPC_TYPE::MUTANT){
            _sprite = bn::sprite_items::mutant.create_sprite(_pos.x(), _pos.y()-4);
            _sprite.value().set_horizontal_flip(true);
            _action = bn::create_sprite_animate_action_forever(
                            _sprite.value(), 20, bn::sprite_items::mutant.tiles_item(), 0,0);
            _lines = bn::span(_mutant_lines);
        }
        else if(_type == NPC_TYPE::GIRLS){
            _lines = bn::span(_girls_lines);
        }
        else if(_type == NPC_TYPE::LAB_PC){
            _lines = bn::span(_lab_pc_lines);
        }
        else if(_type == NPC_TYPE::COMPUTER_STUFF){
            _lines = bn::span(_computers_lines);
        }
        else if(_type == NPC_TYPE::POTION){
            _lines = bn::span(_potion_lines);
        }
        else if(_type == NPC_TYPE::PEWPEW){
            _lines = bn::span(_pewpew_lines);
        }
        if(_sprite.has_value())
        {
            _sprite.value().set_camera(_camera);
            _sprite.value().set_bg_priority(1);
            _sprite.value().set_z_order(2);
        }
    }
    
    void NPC::update(){
        if(_action.has_value()){
            _action.value().update();
        }

        if(_is_talking){
            if(_currentChar == _lines.at(_currentLine).size() * 2){
                if(bn::keypad::up_pressed() || bn::keypad::a_pressed() || bn::keypad::a_held() || bn::keypad::up_held())
                {
                    if(_currentLine == _lines.size()-1)
                    {
                        _is_talking = false;
                        _currentChars = "";
                        _currentChar = 0;
                        _currentLine = 0;
                        _has_spoken_once = true;
                    } else {
                        bn::sound_items::hello.play();
                        _currentLine += 1;
                        _currentChar = 0;
                    }
                } else if(bn::keypad::start_pressed()){
                    _is_talking = false;
                    _currentChars = "";
                    _currentChar = 0;
                    _currentLine = 0;
                    _has_spoken_once = true;
                }
            } else {
                if(bn::keypad::start_pressed()){
                    _is_talking = false;
                    _currentChars = "";
                    _currentChar = 0;
                    _currentLine = 0;
                    _has_spoken_once = true;
                }
                _currentChars = _lines.at(_currentLine).substr(0,(_currentChar/2)+1);
                if(bn::keypad::a_held() || bn::keypad::up_held()){
                    _currentChar+=2;
                } else {
                    ++_currentChar;
                }
                
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

    bool NPC::finished_talking(){
        return _has_spoken_once;
    }

    bool NPC::check_trigger(bn::fixed_point pos){
        if(!_finished && !_hidden){
            if(bn::abs(_pos.x() - pos.x()) < 50){
                if(bn::abs(_pos.y() - pos.y()) < 50){
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
        bn::sound_items::hello.play();
    }

    bool NPC::is_talking(){
        return _is_talking;
    }

    void NPC::set_is_hidden(bool is_hidden){
        _hidden = is_hidden;
        if(_sprite.has_value()){
            _sprite.value().set_visible(!is_hidden);
        }
    }

    bool NPC::hidden(){
        return _hidden;
    }
}