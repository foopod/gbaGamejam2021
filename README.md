# Devlog

Follow along with me as I attempt to build a platformer thing maybe..

Devlog has moved to [my site](https://jonoshields.com/gbagamejam2021/) to make it easier for me to write and probably easier for you to navigate.

I still keep my to-do list here though...

+ Add enemies get hurt
+ Add player can get hurt
+ Add basic enemy AI

Notes:

Okay, so in projects there always comes a part where a lot of work is needed for very little reward. I always struggle to find motivation to see my way through. Queue the last week of pocrastinating.

And eventually today of me actually sitting down and doing it all.

Today I...

+ Refactored the player class to so that an instance of Player lasts for the whole game.
+ Added enemy class and enemy list in Player.
+ Adding attacking hitboxes

ARHGH!! Okay. So I have been stuck on this for a couple of days now and it is really doing my head in.

Trying to use pointers for ths first time, but it seems to be copying the vector.

When I spawn my character into a level I call its spawn method and pass in the list of enemies...

``` cpp
// player.h
private:
    bn::vector<Enemy,32>* _enemies;

public:
    void spawn(bn::vector<Enemy,32>& enemies);
            

//player.cpp
void Player::spawn(bn::vector<Enemy,32>& enemies)
{
    _enemies = &enemies;
}

_enemies.at(0).position(); //returns original position

//scene.cpp
player.spawn(enemies);
enemies.at(0).position(); //returns current posiiton
```

Getting the enemies location in my scene works fine, but the _enemies in the player class doesn't get updated.