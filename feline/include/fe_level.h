#ifndef FE_LEVEL_H
#define FE_LEVEL_H

#include "bn_affine_bg_ptr.h"
#include "bn_vector.h"

namespace fe
{
    class Level
    {
        private:
            bn::vector<int,32> _floor_tiles;
            bn::vector<int,32> _wall_tiles;
            bn::vector<int,32> _ceil_tiles;
            
        public:
            Level(bn::affine_bg_ptr bg);

            [[nodiscard]] bn::vector<int,32> floor_tiles();
            [[nodiscard]] bn::vector<int,32> wall_tiles();
            [[nodiscard]] bn::vector<int,32> ceil_tiles();
    };
}

#endif