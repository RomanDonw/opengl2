#include "Camera.hpp"

#include "../../Scene.hpp"

// === PRIVATE ===

Camera::Camera(Scene *s, Transform t) : Pivot(s, t) {}
Camera::Camera(Scene *s) : Pivot(s) {}

Camera::~Camera() { if (scene->GetCurrentCamera() == this) scene->SetCurrentCamera(nullptr); }

// === PUBLIC ===

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(transform.GetPosition(), transform.GetPosition() + transform.GetFront(), transform.GetUp());
}

glm::mat4 Camera::GetProjectionMatrix(glm::uvec2 scrsize)
{
    return glm::perspective(FOV, (float)scrsize.x / (float)scrsize.y, nearDistance, farDistance);
}