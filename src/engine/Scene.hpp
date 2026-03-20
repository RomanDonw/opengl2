#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <concepts>
#include <map>
#include <functional>

#include "Engine.hpp"

#include "FogRenderSettings.hpp"

class Engine;
class Camera;
class AudioListener;
class RigidBody;
class GameObject;

template <typename T>
concept GameObjectConcept = std::derived_from<T, GameObject>;

class Scene final
{
    friend class Engine;
    friend class AudioListener;
    friend class RigidBody;

    private:
        Scene();
        ~Scene();

        rp3d::PhysicsWorld *world;

        std::map<int32_t, std::vector<GameObject *>, std::less<int32_t>> objects;
        Camera *currcam = nullptr;
        bool hasAudioListener = false;

        void Render();
        void Update(double delta);

        void OnSceneLoad();
        void OnSceneUnload();

    public:
        FogRenderSettings fog;

        bool HasObject(GameObject *obj);

        template<GameObjectConcept T, typename... Args>
        T *CreateObject(Args&&... args)
        {
            T *ret = new T(this, std::forward<Args>(args)...);

            objects.at(0).push_back(ret);
            ret->order = 0;
            
            return ret;
        }
        
        void DeleteObject(GameObject *obj);

        int32_t GetObjectOrder(GameObject *obj);
        void SetObjectOrder(GameObject *obj, int32_t order);

        void ForEachAllObjects(std::function<bool (GameObject *)> callback);
        void ForEachAllOrders(std::function<bool (std::vector<GameObject *>)> callback);

        Camera *GetCurrentCamera(); // can return nullptr.
        void SetCurrentCamera(Camera *cam); // can be nullptr.

        glm::vec3 GetGravity();
        void SetGravity(glm::vec3 v);
};

#endif