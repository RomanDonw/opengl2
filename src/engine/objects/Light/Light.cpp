#include "Light.hpp"

Light::Light(Scene *s, Transform t) : GameObject(s, t) {}

Light::Light(Scene *s) : GameObject(s) {}

Light::~Light() {}
