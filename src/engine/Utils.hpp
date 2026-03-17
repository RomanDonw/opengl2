#ifndef UTILS_HPP
#define UTILS_HPP

#include "external/glm.hpp"
#include "external/physics.hpp"
#include "Transform.hpp"

#include <string>

namespace Utils
{
    float sign(float x);
    double sign(double x);
    
    /*
        Returns angles of given radius-vector relative to Z+ axis.
        Examples:
            vector {0; 0; 1} gives {0; 0; 0} angles,
            {0; 0; -1} gives {0; PI; 0} angles,
            {1; 0; 0} gives {0; PI / 2; 0} angles.
    */
    glm::vec3 angles(glm::vec3 v, float roll = 0);
    
    glm::vec3 normalize(glm::vec3 v);
    glm::vec3 wrapangles(glm::vec3 euler);
    std::string tostring(glm::vec3 v);

    glm::vec3 rp3dvec3toglmvec3(rp3d::Vector3 v);
    rp3d::Vector3 glmvec3torp3dvec3(glm::vec3 v);

    glm::quat rp3dquattoglmquat(rp3d::Quaternion q);
    rp3d::Quaternion glmquattorp3dquat(glm::quat q);

    Transform rp3dtransformtotransform(rp3d::Transform t);
    rp3d::Transform transformtorp3dtransform(Transform t);
}

#endif