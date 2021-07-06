#ifndef FE_SCENE_DEATH_H
#define FE_SCENE_DEATH_H

#include "fe_scene.h"
#include "fe_player.h"

namespace fe
{
    class Death
    {       
        private:
            Player* _player;
        public:
            Death(Player& player);
            Scene execute();
    };
}

#endif