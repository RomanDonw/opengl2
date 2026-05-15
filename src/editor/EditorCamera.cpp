#include "EditorCamera.hpp"

#include "engine/Input.hpp"
#include "engine/external/opengl.hpp"

namespace
{
    constexpr float kPitchLimitDeg = 60.0f;

    static glm::quat quatFromForward(const glm::vec3 &forward)
    {
        const glm::vec3 f = glm::normalize(forward);
        glm::vec3 upRef = glm::vec3(0.0f, 1.0f, 0.0f);
        if (std::abs(glm::dot(f, upRef)) > 0.99f) upRef = glm::vec3(0.0f, 0.0f, 1.0f);

        const glm::vec3 right = glm::normalize(glm::cross(upRef, f));
        const glm::vec3 up = glm::normalize(glm::cross(f, right));

        glm::mat4 m(1.0f);
        m[0][0] = right.x;
        m[1][0] = right.y;
        m[2][0] = right.z;
        m[0][1] = up.x;
        m[1][1] = up.y;
        m[2][1] = up.z;
        m[0][2] = -f.x;
        m[1][2] = -f.y;
        m[2][2] = -f.z;
        return glm::normalize(glm::quat_cast(m));
    }
}

void EditorCamera::syncPositionFromOrbit()
{
    const glm::vec3 front = GetFront();
    position = orbitTarget - front * orbitDistance;
}

void EditorCamera::applyOrbitRotation(float yawDelta, float pitchDelta)
{
    const glm::quat deltaPitch = glm::angleAxis(pitchDelta, glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::quat deltaYaw = glm::angleAxis(yawDelta, glm::vec3(0.0f, 1.0f, 0.0f));

    const glm::quat trial = glm::normalize(deltaYaw * orientation * deltaPitch);
    const glm::vec3 front = trial * glm::vec3(0.0f, 0.0f, -1.0f);
    const glm::vec3 frontFlat = glm::normalize(glm::vec3(front.x, 0.0f, front.z));

    const float pitchLimitCos = glm::cos(glm::radians(kPitchLimitDeg));
    if (glm::length2(frontFlat) > 0.0001f && glm::dot(front, frontFlat) >= pitchLimitCos)
        orientation = trial;
    else
        orientation = glm::normalize(deltaYaw * orientation);

    syncPositionFromOrbit();
}

void EditorCamera::FocusOn(const glm::vec3 &target)
{
    orbitTarget = target;
    position = target + glm::vec3(3.0f, 2.0f, 6.0f);
    orbitDistance = glm::length(position - orbitTarget);
    if (orbitDistance < 0.1f) orbitDistance = 10.0f;
    orientation = quatFromForward(target - position);
    syncPositionFromOrbit();
}

void EditorCamera::SetPose(const glm::vec3 &pos, const glm::quat &rot)
{
    orientation = glm::normalize(rot);
    if (orbitDistance < 0.5f) orbitDistance = 10.0f;
    orbitTarget = pos + GetFront() * orbitDistance;
    syncPositionFromOrbit();
}

glm::vec3 EditorCamera::GetFront() const
{
    return orientation * glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::quat EditorCamera::GetRotation() const { return orientation; }

void EditorCamera::Update(double delta, bool allowInput, const EditorCameraInput &input)
{
    if (!allowInput) return;

    const float dt = static_cast<float>(delta);

    if (input.orbiting && (input.mouseDelta.x != 0.0f || input.mouseDelta.y != 0.0f))
    {
        const float yawDelta = -glm::radians(input.mouseDelta.x * lookSpeed);
        const float pitchDelta = -glm::radians(input.mouseDelta.y * lookSpeed);
        applyOrbitRotation(yawDelta, pitchDelta);
    }

    if (input.panning && (input.mouseDelta.x != 0.0f || input.mouseDelta.y != 0.0f))
    {
        const glm::vec3 front = GetFront();
        glm::vec3 right = glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f));
        if (glm::length2(right) < 1e-6f) right = orientation * glm::vec3(1.0f, 0.0f, 0.0f);
        right = glm::normalize(right);

        const float panScale = panSpeed * orbitDistance;
        orbitTarget -= right * input.mouseDelta.x * panScale;
        orbitTarget += glm::vec3(0.0f, 1.0f, 0.0f) * input.mouseDelta.y * panScale;
        syncPositionFromOrbit();
    }

    if (input.scrollY != 0.0f)
    {
        float zoomFactor = 1.0f - input.scrollY * zoomSpeed * 0.1f;
        if (Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT)) zoomFactor = 1.0f - input.scrollY * zoomSpeed * 0.25f;

        orbitDistance *= zoomFactor;
        if (orbitDistance < 0.25f) orbitDistance = 0.25f;
        if (orbitDistance > 2000.0f) orbitDistance = 2000.0f;
        syncPositionFromOrbit();
    }

    if (Input::IsMouseButtonJustPressed(GLFW_MOUSE_BUTTON_MIDDLE))
        orbitDistance = 10.0f;

    glm::vec3 move(0.0f);
    const glm::vec3 front = GetFront();
    glm::vec3 right = glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f));
    if (glm::length2(right) < 1e-6f) right = orientation * glm::vec3(1.0f, 0.0f, 0.0f);
    right = glm::normalize(right);

    if (Input::IsKeyDown(GLFW_KEY_W)) move += front;
    if (Input::IsKeyDown(GLFW_KEY_S)) move -= front;
    if (Input::IsKeyDown(GLFW_KEY_A)) move -= right;
    if (Input::IsKeyDown(GLFW_KEY_D)) move += right;
    if (Input::IsKeyDown(GLFW_KEY_E) || Input::IsKeyDown(GLFW_KEY_SPACE)) move += glm::vec3(0.0f, 1.0f, 0.0f);
    if (Input::IsKeyDown(GLFW_KEY_Q) || Input::IsKeyDown(GLFW_KEY_LEFT_CONTROL)) move -= glm::vec3(0.0f, 1.0f, 0.0f);

    if (glm::length2(move) > 0.0001f)
    {
        const glm::vec3 deltaMove = glm::normalize(move) * moveSpeed * dt;
        orbitTarget += deltaMove;
        syncPositionFromOrbit();
    }
}

glm::mat4 EditorCamera::GetViewMatrix() const
{
    const glm::vec3 front = GetFront();
    const glm::vec3 up = orientation * glm::vec3(0.0f, 1.0f, 0.0f);
    return glm::lookAt(position, position + front, up);
}

glm::mat4 EditorCamera::GetProjectionMatrix(const glm::uvec2 &screenSize) const
{
    if (screenSize.x == 0 || screenSize.y == 0) return glm::mat4(1.0f);
    return glm::perspective(fov, static_cast<float>(screenSize.x) / static_cast<float>(screenSize.y), nearPlane, farPlane);
}

glm::vec3 EditorCamera::GetPosition() const { return position; }
