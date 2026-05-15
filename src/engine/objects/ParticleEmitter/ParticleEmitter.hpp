#ifndef PARTICLEEMITTER_HPP
#define PARTICLEEMITTER_HPP

#include <vector>

#include "../GameObject/GameObject.hpp"

class Scene;

struct Particle
{
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec4 color = glm::vec4(1.0f);
    float size = 0.1f;
    float life = 0.0f;
    float maxLife = 1.0f;
};

class ParticleEmitter final : public GameObject
{
    friend class Scene;

    protected:
        ParticleEmitter(Scene *s, Transform t);
        ParticleEmitter(Scene *s);

        ~ParticleEmitter() override;

        void Update(double delta) override;
        void Render(const GameObjectRenderData *data) override;

    public:
        bool playing = true;
        bool loop = true;
        float spawnRate = 40.0f;
        int maxParticles = 256;
        glm::vec3 velocityMin = glm::vec3(-0.5f, 1.0f, -0.5f);
        glm::vec3 velocityMax = glm::vec3(0.5f, 3.0f, 0.5f);
        glm::vec4 colorStart = glm::vec4(1.0f, 0.8f, 0.2f, 1.0f);
        glm::vec4 colorEnd = glm::vec4(1.0f, 0.1f, 0.0f, 0.0f);
        float sizeStart = 0.15f;
        float sizeEnd = 0.02f;
        float lifeMin = 0.4f;
        float lifeMax = 1.2f;
        std::string meshName = "sphere";
        std::string shaderProgram = "default";

        void Play();
        void Stop();
        void Clear();

    private:
        std::vector<Particle> particles;
        float spawnAccumulator = 0.0f;

        void spawnParticle();
};

#endif
