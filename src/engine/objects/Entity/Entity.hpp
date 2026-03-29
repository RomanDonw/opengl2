#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "../Model/Model.hpp"
#include "../RigidBody/RigidBody.hpp"

class Scene;

class Entity : public Model, public RigidBody
{
    friend class Scene;

    protected:
        Entity(Scene *s, Transform t);
        Entity(Scene *s);

        ~Entity();
};

#endif