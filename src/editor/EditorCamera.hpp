#ifndef EDITORCAMERA_HPP
#define EDITORCAMERA_HPP

#include "engine/external/glm.hpp"

struct EditorCameraInput
{
    glm::vec2 mouseDelta = glm::vec2(0.0f);
    float scrollY = 0.0f;
    bool orbiting = false;
    bool panning = false;
};

class EditorCamera final
{
    private:
        glm::vec3 position = glm::vec3(0, 2, 8);
        glm::quat orientation = glm::quat(1, 0, 0, 0);
        glm::vec3 orbitTarget = glm::vec3(0, 1, 0);
        float orbitDistance = 10.0f;

        float moveSpeed = 8.0f;
        float lookSpeed = 0.1f;
        float zoomSpeed = 1.2f;
        float panSpeed = 0.002f;
        float fov = glm::radians(70.0f);
        float nearPlane = 0.1f;
        float farPlane = 1000.0f;

        void syncPositionFromOrbit();
        void applyOrbitRotation(float yawDelta, float pitchDelta);

    public:
        void FocusOn(const glm::vec3 &target);
        void SetPose(const glm::vec3 &pos, const glm::quat &rot);
        void Update(double delta, bool allowInput, const EditorCameraInput &input);

        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix(const glm::uvec2 &screenSize) const;

        glm::vec3 GetPosition() const;
        glm::vec3 GetFront() const;
        glm::quat GetRotation() const;
        glm::vec3 GetOrbitTarget() const { return orbitTarget; }
};

#endif
