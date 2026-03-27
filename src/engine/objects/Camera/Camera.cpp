#include "Camera.hpp"

#include "../../Scene.hpp"

// === PRIVATE ===

Camera::Camera(Scene *s, Transform t) : GameObject(s, t) {}
Camera::Camera(Scene *s) : GameObject(s) {}

Camera::~Camera() { if (GetScene()->GetCurrentCamera() == this) GetScene()->SetCurrentCamera(nullptr); }

// === PUBLIC ===

glm::mat4 Camera::GetViewMatrix()
{
    Transform globt = GetGlobalTransform();
    return glm::lookAt(globt.GetPosition(), globt.GetPosition() + globt.GetFront(), globt.GetUp());
}

glm::mat4 Camera::GetProjectionMatrix(glm::uvec2 scrsize)
{
    return glm::perspective(FOV, (float)scrsize.x / (float)scrsize.y, nearDistance, farDistance);
}