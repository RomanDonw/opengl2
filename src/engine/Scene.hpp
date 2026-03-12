#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <concepts>
#include <exception>
#include <stdexcept>
#include <iterator>
#include <algorithm>

#include "FogRenderSettings.hpp"

class Engine;
class GameObject;
class Camera;
class AudioListener;

class Scene final
{
    friend class Engine;
    friend class AudioListener;

    private:
        Scene();
        ~Scene();

        std::vector<GameObject *> objects;
        Camera *currcam = nullptr;
        bool hasAudioListener = false;

        void Render();
        void Update(double delta);

        void OnSceneLoad();
        void OnSceneUnload();

    public:
        FogRenderSettings fog;

        template<std::derived_from<GameObject> T>
        bool HasObject(T *obj) { return std::ranges::contains(objects, obj); }

        template<std::derived_from<GameObject> T, typename... Args>
        T *CreateObject(Args&&... args)
        {
            T *ret = new T(this, std::forward<Args>(args)...);

            objects.push_back(ret);
            
            return ret;
        }

        template<std::derived_from<GameObject> T>
        void DeleteObject(T *obj)
        {
            if (!HasObject(obj)) throw std::runtime_error("this scene doesn't have this object");

            objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());

            delete obj;
        }

        Camera *GetCurrentCamera(); // can return nullptr.
        void SetCurrentCamera(Camera *cam);
};

#endif