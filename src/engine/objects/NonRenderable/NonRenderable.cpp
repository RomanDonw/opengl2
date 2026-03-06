#include "NonRenderable.hpp"

NonRenderable::NonRenderable(Scene *s, Transform t) : GameObject(s, t) {}
NonRenderable::NonRenderable(Scene *s) : GameObject(s) {}

NonRenderable::~NonRenderable() {}

void NonRenderable::Render(const glm::mat4 *proj, const glm::mat4 *view, const Transform *camt) {}