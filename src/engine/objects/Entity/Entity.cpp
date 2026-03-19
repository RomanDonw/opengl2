#include "Entity.hpp"

Entity::Entity(Scene *s, Transform t) : GameObject(s, t), Model(s, t), RigidBody(s, t) {}
Entity::Entity(Scene *s) : GameObject(s), Model(s), RigidBody(s) {}

Entity::~Entity() {}