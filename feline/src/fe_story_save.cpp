#include "fe_story_save.h"
#include "fe_story_type.h"

#include "bn_optional.h"
#include "bn_math.h"
#include "bn_log.h"
#include "bn_display.h"
#include "bn_keypad.h"
#include "bn_sprite_text_generator.h"

#include "bn_sprite_items_stone_plaque.h"
#include "variable_8x8_sprite_font.h"


namespace fe
{

    StorySave::StorySave(bn::fixed_point pos, STORY_TYPE type, bn::camera_ptr& camera, bn::sprite_text_generator& text_generator):
        _pos(pos), _type(type), _camera(camera), _text_generator(text_generator)
    {
        _text_generator.set_bg_priority(0);
        
        if(_type == STORY_TYPE::BEGINNING)
        {
            _sprite = bn::sprite_items::stone_plaque.create_sprite(_pos.x(), _pos.y());
            _action = bn::create_sprite_animate_action_forever(
                            _sprite.value(), 10, bn::sprite_items::stone_plaque.tiles_item(), 0,1,2);
        }
        _sprite.value().set_camera(_camera);
        _sprite.value().set_bg_priority(1);
        _sprite.value().set_z_order(2);
    }
    
    void StorySave::update(){
        if(_action.has_value()){
            _action.value().update();
        }
    }

    // bool StorySave::check_trigger(bn::fixed_point pos){
    //     if(bn::abs(_pos.x() - pos.x()) < 90){
    //         if(bn::abs(_pos.y() - pos.y()) < 90){
    //             return true;
    //         }
    //     }
    //     return false;
    // }

    // bool StorySave::is_active(){
    //     return true;
    // }

    // void StorySave::activate(){

    // }
    

    // bn::fixed_point StorySave::pos(){
    //     return _pos;
    // }
}