#include "fe_elevator.h"

#include "bn_sprite_ptr.h"
#include "bn_fixed_point.h"

namespace fe
{
    Elevator::Elevator(bn::fixed_point pos, bn::sprite_ptr top_sprite, bn::sprite_ptr bottom_sprite, int dist) : 
        _pos(pos), _top_sprite(top_sprite), _bottom_sprite(bottom_sprite), _dist(dist), _start_y(pos.y().integer())
    {
        _top_sprite.set_position(pos);
        _bottom_sprite.set_position(pos);
    }

    bn::fixed_point Elevator::pos(){
        return _pos;
    }

    int Elevator::top_y(){
        return (_pos.y() - 32).integer();
    }

    int Elevator::bottom_y(){
        return _pos.y().integer();
    }

    void Elevator::update_position(){
        if(_pos.y() > _start_y - _dist)
        {
            _pos.set_y(_pos.y() - 0.2);
        } 
        else 
        {
            _pos.set_y(_start_y);
        }
        _top_sprite.set_y(_pos.y() - 16);
        _bottom_sprite.set_y(_pos.y() + 16);
    }
}
