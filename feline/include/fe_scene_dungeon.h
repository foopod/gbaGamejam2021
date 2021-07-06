#ifndef FE_SCENE_DUNEON_H
#define FE_SCENE_DUNEON_H

#include "fe_scene.h"
#include "fe_player.h"
#include "bn_fixed_point.h"

namespace fe
{
    class Dungeon
    {       
        private:
            Player* _player;
        public:
            Dungeon(Player& player);
            Scene execute(bn::fixed_point spawn);
    };
}

#endif