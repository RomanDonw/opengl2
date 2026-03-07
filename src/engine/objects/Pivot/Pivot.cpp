#include "Pivot.hpp"

Pivot::Pivot(Scene *s, Transform t) : GameObject(s, t) {}
Pivot::Pivot(Scene *s) : GameObject(s) {}

Pivot::~Pivot() {}

void Pivot::Render(const glm::mat4 *proj, const glm::mat4 *view, const Transform *camt) {}