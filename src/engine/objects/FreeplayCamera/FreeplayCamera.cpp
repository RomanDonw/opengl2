#include "FreeplayCamera.hpp"

#include "../../Input.hpp"
#include "../../Scene.hpp"
#include "../../external/opengl.hpp"

FreeplayCamera::FreeplayCamera(Scene *s, Transform t) : Camera(s, t)
{
    displayName = "FreeplayCamera";
    tags.insert("freeplay");
    SyncAnglesFromTransform();
}

FreeplayCamera::FreeplayCamera(Scene *s) : Camera(s)
{
    displayName = "FreeplayCamera";
    tags.insert("freeplay");
    SyncAnglesFromTransform();
}

FreeplayCamera::~FreeplayCamera() {}

glm::vec3 FreeplayCamera::GetFront() const
{
    return glm::normalize(transform.GetRotation() * glm::vec3(0.0f, 0.0f, -1.0f));
}

void FreeplayCamera::SyncAnglesFromTransform()
{
    const glm::vec3 euler = glm::degrees(glm::eulerAngles(transform.GetRotation()));
    yaw = euler.y;
    pitch = euler.x;
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}

void FreeplayCamera::ApplyRotationToTransform()
{
    const glm::quat qYaw = glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::quat qPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    transform.SetRotation(qYaw * qPitch);
}

static glm::vec3 movementForward(float yawDeg)
{
    const float y = glm::radians(yawDeg);
    return glm::normalize(glm::vec3(sinf(y), 0.0f, -cosf(y)));
}

void FreeplayCamera::Activate()
{
    active = true;
    SyncAnglesFromTransform();
    if (captureMouseOnStart) Input::SetMouseCaptured(true);
    if (GetScene()) GetScene()->SetCurrentCamera(this);
}

void FreeplayCamera::Deactivate()
{
    active = false;
    Input::SetMouseCaptured(false);
}

void FreeplayCamera::Update(double delta)
{
    if (!active) return;

    const float dt = static_cast<float>(delta);

    if (Input::IsMouseCaptured())
    {
        Input::UpdateMouseLook();
        const glm::vec2 look = Input::GetMouseLookDelta();
        if (look.x != 0.0f || look.y != 0.0f)
        {
            glm::quat rot = transform.GetRotation();
            rot = glm::angleAxis(-look.x * lookSpeed, glm::vec3(0.0f, 1.0f, 0.0f)) * rot;

            const glm::vec3 right = rot * glm::vec3(1.0f, 0.0f, 0.0f);
            rot = rot * glm::angleAxis(-look.y * lookSpeed, right);

            const glm::vec3 front = rot * glm::vec3(0.0f, 0.0f, -1.0f);
            const glm::vec3 frontFlat = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
            if (glm::length2(frontFlat) > 0.0001f && glm::dot(front, frontFlat) >= glm::cos(glm::radians(89.0f)))
                transform.SetRotation(rot);

            SyncAnglesFromTransform();
        }
    }

    glm::vec3 moveDir(0.0f);
    const glm::vec3 flatFwd = movementForward(yaw);
    const glm::vec3 right = glm::normalize(glm::cross(flatFwd, glm::vec3(0.0f, 1.0f, 0.0f)));

    if (Input::IsKeyDown(GLFW_KEY_W)) moveDir += flatFwd;
    if (Input::IsKeyDown(GLFW_KEY_S)) moveDir -= flatFwd;
    if (Input::IsKeyDown(GLFW_KEY_A)) moveDir -= right;
    if (Input::IsKeyDown(GLFW_KEY_D)) moveDir += right;
    if (Input::IsKeyDown(GLFW_KEY_E)) moveDir += glm::vec3(0.0f, 1.0f, 0.0f);
    if (Input::IsKeyDown(GLFW_KEY_Q)) moveDir -= glm::vec3(0.0f, 1.0f, 0.0f);

    if (glm::length2(moveDir) > 0.0001f)
        transform.SetPosition(transform.GetPosition() + glm::normalize(moveDir) * moveSpeed * dt);

    if (Scene *s = GetScene()) s->SetCurrentCamera(this);
}
