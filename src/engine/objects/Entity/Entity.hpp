#ifndef ENITTY_HPP
#define ENTITY_HPP

#include "../GameObject/GameObject.hpp"
#include "Surface.hpp"

class Scene;

class Entity : public GameObject
{
    friend class Scene;

    protected:
        Entity(Scene *s, Transform t);
        Entity(Scene *s);

        ~Entity();

        void Render(const glm::mat4 *proj, const glm::mat4 *view, const Transform *camt, const FogRenderSettings *fog) override;

    public:
        const GameObjectType type = GameObjectType::ENTITY;

        bool enableRender = true;
        std::string usedShaderProgram = "";
        glm::vec4 color = glm::vec4(1.0f);
        std::vector<Surface> surfaces = std::vector<Surface>();
};

#endif