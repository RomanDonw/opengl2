#include "PlayerController.hpp"

#include "engine/Input.hpp"
#include "engine/Scene.hpp"
#include "engine/Utils.hpp"
#include "engine/objects/Camera/Camera.hpp"
#include "engine/objects/RigidBody/RigidBody.hpp"
#include "engine/objects/Model/Model.hpp"

void PlayerController::Init(Scene *s, RigidBody *rb, Camera *cam, Model *vm, glm::vec3 vmBaseRot)
{
    scene = s;
    body = rb;
    camera = cam;
    viewmodel = vm;
    viewmodelBaseRotation = vmBaseRot;
    viewmodelWalkOffset = 0.0f;
}

void PlayerController::applyMovement()
{
    if (!body) return;

    const float mass = body->GetMass();

    if (Input::IsActionDown("MoveForward")) body->ApplyLocalForceToCenterOfMass(glm::vec3(0, 0, -1) * moveForce * mass);
    if (Input::IsActionDown("MoveBack")) body->ApplyLocalForceToCenterOfMass(glm::vec3(0, 0, 1) * moveForce * mass);
    if (Input::IsActionDown("MoveLeft")) body->ApplyLocalForceToCenterOfMass(glm::vec3(-1, 0, 0) * moveForce * mass);
    if (Input::IsActionDown("MoveRight")) body->ApplyLocalForceToCenterOfMass(glm::vec3(1, 0, 0) * moveForce * mass);

    if (Input::IsActionDown("Jump") && scene) body->ApplyLocalForceToCenterOfMass(-scene->GetGravity() * 2.0f * mass);

    body->SetAngularVelocity(glm::vec3(0));
}

void PlayerController::applyMouseLook()
{
    if (!camera || !body) return;

    Input::UpdateMouseLook();
    const glm::vec2 look = Input::GetMouseLookDelta();
    if (look.x == 0.0f && look.y == 0.0f) return;

    const glm::quat delta_pitch = glm::angleAxis(look.y, glm::vec3(1, 0, 0));
    const glm::quat delta_yaw = glm::angleAxis(look.x, glm::vec3(0, 1, 0));

    glm::quat new_rotation = camera->transform.GetRotation() * delta_pitch;

    const glm::vec3 front = new_rotation * glm::vec3(0, 0, -1);
    const glm::vec3 front_xz = glm::normalize(glm::vec3(front.x, 0, front.z));

    if (glm::dot(front, front_xz) >= glm::cos(glm::radians(60.0f))) camera->transform.SetRotation(new_rotation);
    body->transform.Rotate(delta_yaw);
}

void PlayerController::updateViewmodelSway(const glm::vec3 &deltaPosition)
{
    if (!viewmodel) return;

    viewmodelWalkOffset += glm::length(Utils::normalize(deltaPosition)) * glm::radians(11.25f / 4);
    viewmodelWalkOffset = glm::mod(viewmodelWalkOffset, glm::radians(360.0f));
    viewmodel->transform.SetRotation(viewmodelBaseRotation + glm::vec3(glm::sin(viewmodelWalkOffset) * 0.03f, 0.0f, 0.0f));
}

void PlayerController::ProcessInput(double delta)
{
    (void)delta;
    if (!body || !camera) return;

    applyMovement();
    applyMouseLook();
}

void PlayerController::SyncViewmodel(const glm::vec3 &positionBeforePhysics)
{
    if (!body) return;
    updateViewmodelSway(GetPosition() - positionBeforePhysics);
}

Scene *PlayerController::GetScene() const { return scene; }

RigidBody *PlayerController::GetBody() const { return body; }

Camera *PlayerController::GetCamera() const { return camera; }

glm::vec3 PlayerController::GetPosition() const
{
    if (!body) return glm::vec3(0.0f);
    return body->GetGlobalTransform().GetPosition();
}
