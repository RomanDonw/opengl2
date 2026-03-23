#ifndef DECAL_HPP
#define DECAL_HPP

#include "engine/objects/Model/Model.hpp"

class Scene;

class Decal : public Model
{
    friend class Scene;

    private:
        void constructor();

        double starttimestamp = -1;

        Decal(Scene *s, Transform t, double lifetime);
        Decal(Scene *s, double lifetime);

        ~Decal() override;

        void Update(double delta) override;

    public:
        double lifetime;
        double fadeoutstart = -1;
};

#endif