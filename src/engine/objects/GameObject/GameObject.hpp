#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <vector>

#include "GameObjectTransform.hpp"

class Scene;

class GameObject
{
    friend class GameObjectTransform;
    friend class Scene;

    protected:
        virtual void OnLocalTransformChanged();
        virtual void OnParentTransformChanged();

        virtual void OnGlobalTransformChanged();

        GameObject(Transform t);
        GameObject();

        virtual ~GameObject();

        GameObject *parent = nullptr;
        std::vector<GameObject *> children = std::vector<GameObject *>();

        virtual void Update(double delta);
        virtual void Render();

    public:
        GameObjectTransform transform;

        size_t SetParent(GameObject *new_parent, bool save_global_pos = true);
        Transform GetParentGlobalTransform();
        Transform GetGlobalTransform();
};

#endif