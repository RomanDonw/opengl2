#include "EditorCamera.hpp"

#include "engine/Input.hpp"
#include "engine/external/opengl.hpp"

void EditorCamera::FocusOn(const glm::vec3 &target)
{
    position = target + glm::vec3(0, 2, 6);
    const glm::vec3 dir = glm::normalize(target - position);
    pitch = glm::degrees(asinf(dir.y));
    yaw = glm::degrees(atan2f(dir.z, dir.x));
}

void EditorCamera::Update(double delta, bool allowInput)
{
    if (!allowInput) return;

    const float dt = static_cast<float>(delta);
  glm::vec3 front = GetFront();
    const glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
    const glm::vec3 up = glm::vec3(0, 1, 0);

    if (Input::IsKeyDown(GLFW_KEY_W)) position += front * moveSpeed * dt;
    if (Input::IsKeyDown(GLFW_KEY_S)) position -= front * moveSpeed * dt;
    if (Input::IsKeyDown(GLFW_KEY_A)) position -= right * moveSpeed * dt;
    if (Input::IsKeyDown(GLFW_KEY_D)) position += right * moveSpeed * dt;
    if (Input::IsKeyDown(GLFW_KEY_E)) position += up * moveSpeed * dt;
    if (Input::IsKeyDown(GLFW_KEY_Q)) position -= up * moveSpeed * dt;

    if (Input::IsMouseCaptured())
    {
        Input::UpdateMouseLook();
        const glm::vec2 look = Input::GetMouseLookDelta();
        yaw += glm::degrees(look.x) * lookSpeed;
        pitch -= glm::degrees(look.y) * lookSpeed;
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }
}

glm::vec3 EditorCamera::GetFront() const
{
    const glm::vec3 dir = glm::vec3(
        cosf(glm::radians(yaw)) * cosf(glm::radians(pitch)),
        sinf(glm::radians(pitch)),
        sinf(glm::radians(yaw)) * cosf(glm::radians(pitch)));
    return glm::normalize(dir);
}

glm::mat4 EditorCamera::GetViewMatrix() const
{
    return glm::lookAt(position, position + GetFront(), glm::vec3(0, 1, 0));
}

glm::mat4 EditorCamera::GetProjectionMatrix(const glm::uvec2 &screenSize) const
{
    if (screenSize.x == 0 || screenSize.y == 0) return glm::mat4(1.0f);
    return glm::perspective(fov, static_cast<float>(screenSize.x) / static_cast<float>(screenSize.y), nearPlane, farPlane);
}

glm::vec3 EditorCamera::GetPosition() const { return position; }
