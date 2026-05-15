#ifndef FREEPLAYCAMERA_HPP
#define FREEPLAYCAMERA_HPP

#include "../Camera/Camera.hpp"

#include "../../external/glm.hpp"

class Scene;

class FreeplayCamera final : public Camera
{
    friend class Scene;

    protected:
        FreeplayCamera(Scene *s, Transform t);
        FreeplayCamera(Scene *s);

        ~FreeplayCamera() override;

    public:
        void Update(double delta) override;
        bool active = true;
        float moveSpeed = 10.0f;
        float lookSpeed = 1.0f;
        bool captureMouseOnStart = false;

        float yaw = -135.0f;
        float pitch = -20.0f;

        void Activate();
        void Deactivate();
        void SyncAnglesFromTransform();
        glm::vec3 GetFront() const;
        void ApplyRotationToTransform();
};

#endif
