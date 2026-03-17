#ifndef ENITTY_HPP
#define ENTITY_HPP

#include "../GameObject/GameObject.hpp"
#include "Surface.hpp"
#include "../../external/physics.hpp"

class Scene;

class Entity : public GameObject
{
    friend class Scene;

    private:
        void constructor();

    protected:
        rp3d::RigidBody *rb;

        Entity(Scene *s, Transform t);
        Entity(Scene *s);

        ~Entity() override;

        void OnGlobalTransformChanged() override;
        void AfterUpdate() override;
        void Render(const glm::mat4 *proj, const glm::mat4 *view, const Transform *camt, const FogRenderSettings *fog) override;

    public:
        const GameObjectType type = GameObjectType::ENTITY;

        bool enableRender = true;
        std::string usedShaderProgram = "";
        glm::vec4 color = glm::vec4(1.0f);
        std::vector<Surface> surfaces = std::vector<Surface>();

        bool IsGravityEnabled() const;
        void SetEnabledGravity(bool state);

        glm::vec3 GetLinearVelocity() const;
        void SetLinearVelocity(glm::vec3 v);
};

#endif