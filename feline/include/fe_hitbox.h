#ifndef FE_HITBOX_H
#define FE_HITBOX_H

namespace fe
{
    class Hitbox
    {
        private:
            int _x;
            int _y;
            int _width;
            int _height;

        public:
            Hitbox(int x, int y, int width, int height);

            [[nodiscard]] int x();
            [[nodiscard]] int y();
            [[nodiscard]] int width();
            [[nodiscard]] int height();
    };
}

#endif