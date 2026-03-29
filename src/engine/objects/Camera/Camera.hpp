#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "../../external/glm.hpp"

//#include "../Pivot/Pivot.hpp"
#include "../GameObject/GameObject.hpp"

class Scene;

class Camera final : public GameObject
{
    friend class Scene;

    private:
        Camera(Scene *s, Transform t);
        Camera(Scene *s);

        ~Camera() override;

    public:
        float FOV = glm::radians(60.0f);
        float nearDistance = 0.1;
        float farDistance = 1000;

        glm::mat4 GetViewMatrix();
        glm::mat4 GetProjectionMatrix(glm::uvec2 scrsize);
};

#endif