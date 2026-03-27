#ifndef SCENE_HPP
#define SCENE_HPP

#include <unordered_set>
#include <concepts>
#include <map>
#include <unordered_map>
#include <functional>

#include "Engine.hpp"

#include "FogRenderSettings.hpp"
#include "physics/RaycastCallbackState.hpp"
#include "physics/RaycastInfo.hpp"

class Engine;
class Camera;
class AudioListener;
class RigidBody;
class GameObject;
class Collider;
class SceneRaycastCallback;

template <typename T>
concept GameObjectConcept = std::derived_from<T, GameObject>;

class Scene final
{
    friend class Engine;
    friend class AudioListener;
    friend class RigidBody;
    friend class Collider;
    friend class SceneRaycastCallback;

    private:
        Scene();
        ~Scene();

        rp3d::PhysicsWorld *world;
        std::unordered_map<rp3d::RigidBody *, RigidBody *> rbslinks;
        std::unordered_map<rp3d::Collider *, Collider *> collslinks;

        std::map<int32_t, std::unordered_set<GameObject *>, std::less<int32_t>> objects;
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

            objects.at(0).insert(ret);
            ret->order = 0;
            
            return ret;
        }
        
        void DeleteObject(GameObject *obj);

        int32_t GetObjectOrder(GameObject *obj);
        void SetObjectOrder(GameObject *obj, int32_t order);

        void ForEachAllObjects(std::function<bool (GameObject *)> callback);
        void ForEachAllOrders(std::function<bool (std::unordered_set<GameObject *>)> callback);

        Camera *GetCurrentCamera(); // can return nullptr.
        void SetCurrentCamera(Camera *cam); // can be nullptr.

        glm::vec3 GetGravity();
        void SetGravity(glm::vec3 v);

        bool Raycast(glm::vec3 start, glm::vec3 end, std::function<RaycastCallbackState (RaycastInfo)> callback, unsigned short collidewithmaskbits = 0xFFFF);
};

#endif