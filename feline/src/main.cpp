#include "bn_core.h"
#include "bn_sprite_builder.h"
#include "bn_sprite_ptr.h"

#include "bn_sprite_items_cat.h"


int main()
{
    bn::core::init();

    bn::sprite_ptr cat_sprite = bn::sprite_items::cat.create_sprite(0, 0);

    while(true)
    {
        bn::core::update();
    }
}
