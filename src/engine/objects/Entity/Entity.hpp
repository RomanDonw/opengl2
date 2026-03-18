#ifndef ENITTY_HPP
#define ENTITY_HPP

#include "../GameObject/GameObject.hpp"
#include "Surface.hpp"
#include "../../external/physics.hpp"

class Scene;

enum
{
    STATIC = 0,
    DYNAMIC = 1
} typedef EntityRigidBodyType;

class Entity : public GameObject
{
    friend class Scene;

    private:
        bool lockphystransupdate = false; // this flag need to prevert double updating of physics transform when entity`s transform updates after scene physics update.

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

        size_t SetParent(GameObject *new_parent, bool save_global_pos = true) override;

        EntityRigidBodyType GetRigidBodyType();
        void SetRigidBodyType(EntityRigidBodyType type);

        // doesn't work with static (default) rigidbody type.

        bool IsGravityEnabled() const;
        void SetEnabledGravity(bool state);

        glm::vec3 GetLinearVelocity() const;
        void SetLinearVelocity(glm::vec3 v);
};

#endif