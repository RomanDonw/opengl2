#ifndef PIVOT_HPP
#define PIVOT_HPP

#include "../NonRenderable/NonRenderable.hpp"

class Scene;

class Pivot final : public NonRenderable
{
    friend class Scene;

    private:
        Pivot(Scene *s, Transform t);
        Pivot(Scene *s);

        ~Pivot();
};

#endif