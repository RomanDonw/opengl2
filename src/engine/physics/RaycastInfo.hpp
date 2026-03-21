#ifndef RAYCASTINFO_HPP
#define RAYCASTINFO_HPP

#include "../external/glm.hpp"

class RigidBody;
class Collider;

struct
{
    glm::vec3 point; // in global coordinate space.
    glm::vec3 normal; // also in global coordinate space.
    RigidBody *rigidbody;
    Collider *collider;
} typedef RaycastInfo;

#endif