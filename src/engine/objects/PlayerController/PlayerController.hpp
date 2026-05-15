#ifndef PLAYERCONTROLLER_HPP
#define PLAYERCONTROLLER_HPP

#include "../../external/glm.hpp"

class Scene;
class Camera;
class RigidBody;
class Model;

class PlayerController
{
    private:
        Scene *scene = nullptr;
        RigidBody *body = nullptr;
        Camera *camera = nullptr;
        Model *viewmodel = nullptr;
        glm::vec3 viewmodelBaseRotation = glm::vec3(0.0f);
        float viewmodelWalkOffset = 0.0f;
        float moveForce = 10.0f;

        void applyMovement();
        void applyMouseLook();
        void updateViewmodelSway(const glm::vec3 &deltaPosition);

    public:
        PlayerController() = default;

        void Init(Scene *s, RigidBody *rb, Camera *cam, Model *vm = nullptr, glm::vec3 vmBaseRot = glm::vec3(0.0f));

        void ProcessInput(double delta);
        void SyncViewmodel(const glm::vec3 &positionBeforePhysics);

        Scene *GetScene() const;
        RigidBody *GetBody() const;
        Camera *GetCamera() const;
        glm::vec3 GetPosition() const;
};

#endif
