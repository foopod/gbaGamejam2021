#ifndef FE_HOUSE_SCENE_H
#define FE_HOUSE_SCENE_H

#include "fe_scene.h"
#include "bn_fixed_point.h"

namespace fe
{
    class House
    {       
        public:
            Scene execute(bn::fixed_point spawn);
    };
}

#endif