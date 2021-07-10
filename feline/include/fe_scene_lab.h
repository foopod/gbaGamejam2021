#ifndef FE_SCENE_LAB_H
#define FE_SCENE_LAB_H

#include "fe_scene.h"
#include "fe_player.h"
#include "bn_fixed_point.h"

namespace fe
{
    class Lab
    {       
        private:
            Player* _player;
        public:
            Lab(Player& player);
            Scene execute( bn::fixed_point spawn);
    };
}

#endif