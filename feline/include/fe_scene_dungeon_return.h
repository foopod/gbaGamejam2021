#ifndef FE_SCENE_DUNEON_RETURN_H
#define FE_SCENE_DUNEON_RETURN_H

#include "fe_scene.h"
#include "fe_player.h"
#include "bn_fixed_point.h"

namespace fe
{
    class DungeonReturn
    {       
        private:
            Player* _player;
        public:
            DungeonReturn(Player& player);
            Scene execute(bn::fixed_point spawn);
    };
}

#endif