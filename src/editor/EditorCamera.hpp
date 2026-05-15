#ifndef EDITORCAMERA_HPP
#define EDITORCAMERA_HPP

#include "engine/external/glm.hpp"

class EditorCamera final
{
    private:
        glm::vec3 position = glm::vec3(10, 6, 10);
        float yaw = -135.0f;
        float pitch = -20.0f;
        float moveSpeed = 6.0f;
        float lookSpeed = 1.0f;
        float fov = glm::radians(70.0f);
        float nearPlane = 0.1f;
        float farPlane = 1000.0f;

    public:
        void FocusOn(const glm::vec3 &target);
        void Update(double delta, bool allowInput);
        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix(const glm::uvec2 &screenSize) const;
        glm::vec3 GetPosition() const;
        glm::vec3 GetFront() const;
};

#endif
