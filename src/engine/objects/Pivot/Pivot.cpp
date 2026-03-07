#include "Pivot.hpp"

Pivot::Pivot(Scene *s, Transform t) : NonRenderable(s, t) {}

Pivot::Pivot(Scene *s) : NonRenderable(s) {}

Pivot::~Pivot() {}