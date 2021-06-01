#ifndef FE_EXTRAS_H
#define FE_EXTRAS_H

#include "bn_fixed_point.h"

namespace fe
{
    [[nodiscard]] constexpr bn::fixed modulo(bn::fixed a, bn::fixed m)
    {
        return a - m * ((a/m).integer());
    }
}

#endif