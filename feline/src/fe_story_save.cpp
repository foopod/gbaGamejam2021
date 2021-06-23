#include "fe_story_save.h"
#include "fe_story_type.h"

#include "bn_core.h"
#include "bn_optional.h"
#include "bn_math.h"
#include "bn_log.h"
#include "bn_display.h"
#include "bn_keypad.h"
#include "bn_sprite_text_generator.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_item.h"

#include "bn_sprite_items_stone_plaque.h"
#include "bn_sprite_items_stone_plaque_broken.h"
#include "bn_regular_bg_items_stone_tablet.h"
#include "bn_regular_bg_items_stone_tablet_translate.h"
#include "bn_regular_bg_items_stone_tablet_2.h"
#include "bn_regular_bg_items_stone_tablet_translate_2.h"
#include "bn_regular_bg_items_stone_tablet_3.h"
#include "bn_regular_bg_items_stone_tablet_translate_3.h"
#include "bn_regular_bg_items_stone_tablet_translate_4.h"
#include "variable_8x8_sprite_font.h"


namespace fe
{

    StorySave::StorySave(bn::fixed_point pos, STORY_TYPE type, bn::camera_ptr& camera, bn::sprite_text_generator& text_generator):
        _pos(pos), _type(type), _camera(camera), _text_generator(text_generator)
    {
        _text_generator.set_bg_priority(0);
        _text_generator.set_one_sprite_per_character(false);
        
        if(type != STORY_TYPE::CRACKED){
            _sprite = bn::sprite_items::stone_plaque.create_sprite(_pos.x(), _pos.y());
            _action = bn::create_sprite_animate_action_forever(
                        _sprite.value(), 10, bn::sprite_items::stone_plaque.tiles_item(), 0,1,2);
        } else {
            _sprite = bn::sprite_items::stone_plaque_broken.create_sprite(_pos.x(), _pos.y());
            _action = bn::create_sprite_animate_action_forever(
                        _sprite.value(), 10, bn::sprite_items::stone_plaque_broken.tiles_item(), 0,1,2);  
        }

        _sprite.value().set_camera(_camera);
        _sprite.value().set_bg_priority(1);
        _sprite.value().set_z_order(2);
    }
    
    void StorySave::update(){
        if(_action.has_value()){
            _action.value().update();
        }
        if(_is_near_player){
            _text_generator.set_center_alignment();
            _text_sprites.clear();
            _text_generator.generate(0, _text_y_limit, "press 'up' to interact", _text_sprites);
        } else {
            _text_sprites.clear();
        }
    }

    bool StorySave::check_trigger(bn::fixed_point pos){
        if(bn::abs(_pos.x() - pos.x()) < 30){
            if(bn::abs(_pos.y() - pos.y()) < 30){
                _is_near_player = true;
                return true;
                
            }
        }
        _is_near_player = false;
        return false;
    }

    // bool StorySave::is_active(){
    //     return true;
    // }

    void StorySave::execute_scene(){
        _is_near_player = false;
        update();
        _slab.reset();

        bn::optional<bn::regular_bg_item> slab_english;
        bn::optional<bn::regular_bg_item> slab_foreign;
        
        if(_type == STORY_TYPE::FIRST){
            slab_english = bn::regular_bg_items::stone_tablet_translate;
            slab_foreign = bn::regular_bg_items::stone_tablet;
        } else if(_type == STORY_TYPE::SECOND){
            slab_english = bn::regular_bg_items::stone_tablet_translate_2;
            slab_foreign = bn::regular_bg_items::stone_tablet_2;
        } else if(_type == STORY_TYPE::THIRD){
            slab_english = bn::regular_bg_items::stone_tablet_translate_3;
            slab_foreign = bn::regular_bg_items::stone_tablet_3;
        } else if(_type == STORY_TYPE::CRACKED){
            slab_english = bn::regular_bg_items::stone_tablet_translate_4;
            slab_foreign = bn::regular_bg_items::stone_tablet_3;
        }
        

        _slab = slab_english.value().create_bg(0, _y);
        _slab.value().set_priority(0);
        _slab.value().set_mosaic_enabled(true);
        int frames = 0;
        bool is_translated = false;
        _text_generator.set_right_alignment();
        if(_type != STORY_TYPE::CRACKED){
            _text_generator.generate(110,-70, "TAP 'A' to read, 'B' to exit", _text_sprites);
        } else {
            _text_generator.generate(110,-70, "Too Broken to translate, 'B' to exit", _text_sprites);
        }
        

        while(!bn::keypad::b_pressed()){
            bn::fixed y = _slab.value().y();
            if(bn::keypad::down_held() && y > -48){
                _slab.value().set_y(y - 1);
            }
            if(bn::keypad::up_held() && y < 48){
                _slab.value().set_y(y + 1);
            }
            _y = _slab.value().y();
            if(bn::keypad::a_pressed()){
                frames+=15;
                if(frames > 30){
                    frames = 30;
                }
            }
            if(frames > 1 && !_is_translate && _type != STORY_TYPE::CRACKED){
                _slab.reset();
                _slab = slab_foreign.value().create_bg(0, _y);
                _slab.value().set_priority(0);
                _is_translate = true;
            } else if(frames == 0 && _is_translate){
                _slab.reset();
                _slab = slab_english.value().create_bg(0, _y);
                _slab.value().set_priority(0);
                _is_translate = false;
            }
            

            bn::core::update();
            if(frames > 0){
                --frames;
            }
        }
        slab_english.reset();
        slab_foreign.reset();
        _text_sprites.clear();
        _slab.reset();
    }
    

    bn::fixed_point StorySave::pos(){
        return _pos;
    }
}