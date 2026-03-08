#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <vector>
#include <string>

#include "GameObjectTypes.hpp"
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

        GameObject(Scene *s, Transform t);
        GameObject(Scene *s);

        virtual ~GameObject();

        Scene *scene;
        GameObject *parent = nullptr;
        std::vector<GameObject *> children = std::vector<GameObject *>();

        virtual void Update(double delta);
        virtual void Render(const glm::mat4 *proj, const glm::mat4 *view, const Transform *camt);

    public:
        const GameObjectType type = GameObjectType::GAMEOBJECT;
        GameObjectTransform transform;

        GameObject *GetParent(); // can return nullptr.
        size_t SetParent(GameObject *new_parent, bool save_global_pos = true);

        Scene *GetScene();

        Transform GetParentGlobalTransform();
        Transform GetGlobalTransform();
};

#endif