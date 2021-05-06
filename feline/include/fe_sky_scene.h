#ifndef FE_SKY_SCENE_H
#define FE_SKY_SCENE_H

#include "fe_scene.h"
#include "bn_fixed_point.h"

namespace fe
{
    class Sky
    {       
        public:
            Scene execute(bn::fixed_point spawn);
    };
}

#endif