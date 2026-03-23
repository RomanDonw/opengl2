#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <vector>
#include <string>
#include <unordered_set>

#include "GameObjectTypes.hpp"
#include "GameObjectTransform.hpp"

#include "../../FogRenderSettings.hpp"

class Scene;

class GameObject
{
    friend class GameObjectTransform;
    friend class Scene;

    private:
        int32_t order; // can be changed only by Scene!

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
        virtual void AfterUpdate();
        virtual void Render(const glm::mat4 *proj, const glm::mat4 *view, const Transform *camt, const FogRenderSettings *fog);

        virtual void OnSceneLoad();
        virtual void OnSceneUnload();

    public:
        const GameObjectType type = GameObjectType::GAMEOBJECT;
        GameObjectTransform transform;
        std::unordered_set<std::string> tags;

        GameObject *GetParent(); // can return nullptr.
        virtual size_t SetParent(GameObject *newparent, bool saveglobalpos = true);

        Scene *GetScene();

        Transform GetParentGlobalTransform();
        Transform GetGlobalTransform();
};

#endif