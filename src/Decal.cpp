#include "Decal.hpp"

#include "engine/Scene.hpp"

// === PRIVATE ===

void Decal::constructor()
{
    tags.insert("Decal");
}

Decal::Decal(Scene *s, Transform t, double lifetime) : GameObject(s, t), Model(s, t), lifetime(lifetime) { constructor(); }
Decal::Decal(Scene *s, double lifetime) : GameObject(s), Model(s), lifetime(lifetime) { constructor(); }

Decal::~Decal() {}

void Decal::Update(double delta)
{
    Model::Update(delta);

    if (starttimestamp < 0) starttimestamp = glfwGetTime();
    if (glfwGetTime() >= starttimestamp + lifetime) { scene->DeleteObject(this); return; }

    double timesincecreate = glfwGetTime() - starttimestamp;
    double fadeoutlen = lifetime - fadeoutstart;

    if (fadeoutstart >= 0 && fadeoutlen > 0 && timesincecreate >= fadeoutstart) color.w = glm::min(1.0d, glm::max(0.0d, 1 - (timesincecreate - fadeoutstart) / fadeoutlen));
}