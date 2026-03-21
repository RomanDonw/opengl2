#ifndef SCENERAYCASTCALLBACK_HPP
#define SCENERAYCASTCALLBACK_HPP

#include "../external/physics.hpp"

#include <functional>

#include "RaycastCallbackState.hpp"
#include "RaycastInfo.hpp"

class Scene;

class SceneRaycastCallback final : public rp3d::RaycastCallback
{
    friend class Scene;

    private:
        Scene *scene;
        std::function<RaycastCallbackState (RaycastInfo)> callback;
        bool hit = false;

        SceneRaycastCallback(Scene *s, std::function<RaycastCallbackState (RaycastInfo)> callback);
        ~SceneRaycastCallback() override;

    public:
        float notifyRaycastHit(const rp3d::RaycastInfo &info);
};

#endif