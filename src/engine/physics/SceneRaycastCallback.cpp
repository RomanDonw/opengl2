#include "SceneRaycastCallback.hpp"

#include <utility>

#include "../Utils.hpp"
#include "../Scene.hpp"

// === PRIVATE ===

SceneRaycastCallback::SceneRaycastCallback(Scene *s, std::function<RaycastCallbackState (RaycastInfo)> callback) : scene(s), callback(callback) {}
SceneRaycastCallback::~SceneRaycastCallback() {}

// === PUBLIC ===

float SceneRaycastCallback::notifyRaycastHit(const rp3d::RaycastInfo &info)
{
    hit = true;

    RaycastInfo i;
    i.point = Utils::rp3dvec3toglmvec3(info.worldPoint);
    i.normal = Utils::rp3dvec3toglmvec3(info.worldNormal);
    i.rigidbody = scene->rbslinks.at((rp3d::RigidBody *)info.body);
    i.collider = scene->collslinks.at(info.collider);

    switch (callback(i))
    {
        case STOP:
            return 0;

        case CONTINUE:
            return 1;

        case IGNORECOLLIDER:
            return -1;
    }

    return std::numeric_limits<float>::signaling_NaN();
}