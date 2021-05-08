#include "fe_level.h"

#include "bn_affine_bg_ptr.h"
#include "bn_affine_bg_map_ptr.h"
#include "bn_optional.h"
#include "bn_span.h"
#include "bn_log.h"
#include "bn_string.h"


namespace fe
{
    Level::Level(bn::affine_bg_ptr bg)
    {
        bool processed_map = false;
        int index = 0;
        int empties = 0;
        _floor_tiles = {};
        _wall_tiles = {};
        _ceil_tiles = {};
        bn::span<const bn::affine_bg_map_cell> cells = bg.map().cells_ref().value();
        
        while (!processed_map)
        {
            if(cells.at(index) == 0){
                ++empties;
                if(empties > 2){
                    processed_map = true;
                }
            } else {
                if(empties == 0){
                    _floor_tiles.push_back(cells.at(index));
                } else if(empties == 1){
                    _wall_tiles.push_back(cells.at(index));
                } else if(empties == 2){
                    _ceil_tiles.push_back(cells.at(index));
                }
            }
            ++index;
        }
    }

    bn::vector<int,32> Level::floor_tiles(){
        return _floor_tiles;
    }

    bn::vector<int,32> Level::wall_tiles(){
        return _wall_tiles;
    }

    bn::vector<int,32> Level::ceil_tiles(){
        return _ceil_tiles;
    }

}