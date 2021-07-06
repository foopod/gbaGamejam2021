#ifndef FE_SCENE_HOUSE_H
#define FE_SCENE_HOUSE_H

#include "fe_scene.h"
#include "fe_player.h"
#include "bn_fixed_point.h"

namespace fe
{
    class House
    {       
        private:
            Player* _player;
        public:
            House(Player& player);
            Scene execute(bn::fixed_point spawn);
    };
}

#endif