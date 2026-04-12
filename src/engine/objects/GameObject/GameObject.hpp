#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <string>
#include <unordered_set>

#include "GameObjectTransform.hpp"
#include "GameObjectRenderData.hpp"

#include "../../FogRenderSettings.hpp"

class Scene;

class GameObject
{
    friend class GameObjectTransform;
    friend class Scene;

    private:
        int32_t order; // can be changed only by Scene class!

        // for security reasons moved to private section.
        Scene *scene;
        GameObject *parent = nullptr;
        std::unordered_set<GameObject *> children = std::unordered_set<GameObject *>();

    protected:
        virtual void OnLocalTransformChanged();
        virtual void OnParentTransformChanged();

        virtual void OnGlobalTransformChanged();

        GameObject(Scene *s, Transform t);
        GameObject(Scene *s);

        virtual ~GameObject();

        virtual void Update(double delta);
        virtual void AfterUpdate();
        virtual void Render(const GameObjectRenderData *data);

        virtual void OnSceneLoad();
        virtual void OnSceneUnload();

    public:
        GameObjectTransform transform;
        std::unordered_set<std::string> tags;

        GameObject *GetParent() const; // can return nullptr.
        bool SetParent(GameObject *newparent, bool saveglobalpos = true); // newparent can be nullptr; returns true if parent was changed, otherwise returns false.

        std::unordered_set<GameObject *> GetChildren() const;

        Scene *GetScene() const;

        Transform GetParentGlobalTransform() const;
        Transform GetGlobalTransform() const;
};

#endif