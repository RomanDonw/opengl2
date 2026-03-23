#ifndef MAXWELLCAT_HPP
#define MAXWELLCAT_HPP

#include "engine/objects/RigidBody/RigidBody.hpp"

class Scene;
class AudioSource;
class Model;

class MaxwellCat final : public RigidBody
{
    friend class Scene;

    private:
        AudioSource *src;
        Model *model;

        void constructor();

        MaxwellCat(Scene *s, Transform t);
        MaxwellCat(Scene *s);

        ~MaxwellCat() override;

        void Update(double delta) override;
};

#endif