#ifndef FE_SCENE_OTHER_H
#define FE_SCENE_OTHER_H

#include "fe_scene.h"
#include "fe_player.h"
#include "bn_fixed_point.h"

namespace fe
{
    class Other
    {       
        private:
            Player* _player;
        public:
            Other(Player& player);
            Scene execute( bn::fixed_point spawn);
    };
}

#endif