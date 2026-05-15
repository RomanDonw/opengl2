#include "ParticleEmitter.hpp"

#include <string>

#include "../../Easing.hpp"
#include "../../Random.hpp"
#include "../../ResourceManager.hpp"
#include "../../render/LightingUniforms.hpp"
#include "../../resources/Mesh.hpp"
#include "../../resources/ShaderProgram.hpp"

ParticleEmitter::ParticleEmitter(Scene *s, Transform t) : GameObject(s, t) {}

ParticleEmitter::ParticleEmitter(Scene *s) : GameObject(s) {}

ParticleEmitter::~ParticleEmitter() {}

void ParticleEmitter::Play() { playing = true; }

void ParticleEmitter::Stop() { playing = false; }

void ParticleEmitter::Clear()
{
    particles.clear();
    spawnAccumulator = 0.0f;
}

void ParticleEmitter::spawnParticle()
{
    if (static_cast<int>(particles.size()) >= maxParticles) return;

    Particle p;
    p.position = GetGlobalTransform().GetPosition();
    p.velocity = glm::vec3(Random::Float(velocityMin.x, velocityMax.x), Random::Float(velocityMin.y, velocityMax.y), Random::Float(velocityMin.z, velocityMax.z));
    p.maxLife = Random::Float(lifeMin, lifeMax);
    p.life = p.maxLife;
    p.size = sizeStart;
    p.color = colorStart;
    particles.push_back(p);
}

void ParticleEmitter::Update(double delta)
{
    if (playing)
    {
        spawnAccumulator += static_cast<float>(delta) * spawnRate;
        while (spawnAccumulator >= 1.0f)
        {
            spawnParticle();
            spawnAccumulator -= 1.0f;
        }
    }

    for (size_t i = 0; i < particles.size();)
    {
        Particle &p = particles[i];
        p.life -= static_cast<float>(delta);
        if (p.life <= 0.0f)
        {
            if (loop && playing) spawnParticle();
            particles[i] = particles.back();
            particles.pop_back();
            continue;
        }

        const float t = 1.0f - (p.life / p.maxLife);
        const float eased = Easing::OutQuad(t);
        p.position += p.velocity * static_cast<float>(delta);
        p.velocity.y -= 2.5f * static_cast<float>(delta);
        p.color = glm::mix(colorStart, colorEnd, eased);
        p.size = glm::mix(sizeStart, sizeEnd, eased);
        ++i;
    }
}

void ParticleEmitter::Render(const GameObjectRenderData *data)
{
    if (!data || particles.empty()) return;

    ShaderProgram *sp = ResourceManager::GetShaderProgram(shaderProgram);
    Mesh *mesh = ResourceManager::GetMesh(meshName);
    if (!sp || !mesh) return;

    sp->UseThisProgram();
    sp->SetUniformMatrix4x4("projection", *(data->proj));
    sp->SetUniformMatrix4x4("view", *(data->view));
    sp->SetUniformVector3("cameraPosition", data->cameraPosition);
    sp->SetUniformVector3("cameraRotation", data->cameraRotation);
    sp->SetUniformVector3("cameraFront", data->cameraFront);
    sp->SetUniformVector3("cameraUp", data->cameraUp);
    sp->SetUniformVector3("cameraRight", data->cameraRight);

    if (data->fog)
    {
        sp->SetUniformInteger("fogEnabled", (data->fog->enabled && data->fog->startDistance >= 0 && data->fog->endDistance > 0) ? GL_TRUE : GL_FALSE);
        sp->SetUniformFloat("fogStartDistance", data->fog->startDistance);
        sp->SetUniformFloat("fogEndDistance", data->fog->endDistance);
        sp->SetUniformVector3("fogColor", data->fog->color);
    }

    LightingUniforms::Apply(sp, data->lights);

    sp->SetUniformInteger("texture", 0);
    sp->SetUniformInteger("hasTexture", GL_FALSE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    for (const Particle &p : particles)
    {
        Transform t;
        t.SetPosition(p.position);
        t.SetScale(glm::vec3(p.size));
        const glm::mat4 modelMatrix = t.GetTransformationMatrix();

        sp->SetUniformMatrix3x3("textureTransformation", glm::mat3(1.0f));
        sp->SetUniformMatrix4x4("model", modelMatrix);
        sp->SetUniformVector4("color", p.color);
        mesh->RenderMesh();
    }

    glEnable(GL_DEPTH_TEST);
}
