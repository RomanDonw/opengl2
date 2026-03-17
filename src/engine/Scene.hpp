#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <concepts>
#include <exception>
#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <map>
#include <functional>

#include "Engine.hpp"

#include "FogRenderSettings.hpp"

class Engine;
class GameObject;
class Camera;
class AudioListener;

class Scene final
{
    friend class Engine;
    friend class AudioListener;
    friend class GameObject;

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

        template<std::derived_from<GameObject> T>
        bool HasObject(T *obj)
        {
            bool ret = false;
            ForEachAllObjects([&](GameObject *o) -> bool
            {
                ret = obj == o;
                return !ret;
            });
            return ret;
        }

        template<std::derived_from<GameObject> T, typename... Args>
        T *CreateObject(Args&&... args)
        {
            T *ret = new T(this, std::forward<Args>(args)...);

            objects.at(0).push_back(ret);
            ret->order = 0;
            
            return ret;
        }

        template<std::derived_from<GameObject> T>
        void DeleteObject(T *obj)
        {
            if (!HasObject(obj)) throw std::runtime_error("this scene doesn't have this object");

            std::vector<GameObject *> *group = &objects.at(obj->order);
            group->erase(std::remove(group->begin(), group->end(), obj), group->end());
            if (!group->size()) objects.erase(obj->order);

            delete obj;
        }

        void ForEachAllObjects(std::function<bool (GameObject *)> callback);
        void ForEachAllOrders(std::function<bool (std::vector<GameObject *>)> callback);


        Camera *GetCurrentCamera(); // can return nullptr.
        void SetCurrentCamera(Camera *cam); // can be nullptr.


        template<std::derived_from<GameObject> T>
        int32_t GetObjectOrder(T *obj) { return obj->order; }

        template<std::derived_from<GameObject> T>
        void SetObjectOrder(T *obj, int32_t order)
        {
            if (!HasObject(obj)) throw std::runtime_error("this scene doesn't have this object");
            if (order == obj->order) return;

            std::vector<GameObject *> *group = &objects.at(obj->order);
            group->erase(std::remove(group->begin(), group->end(), obj), group->end());
            if (!group->size()) objects.erase(obj->order);

            if (!objects.contains(order)) objects.insert({order, std::vector<GameObject *>()});
            objects.at(order).push_back(obj);
            obj->order = order;
        }
};

#endif