#ifndef RANDOM_HPP
#define RANDOM_HPP

#include "external/glm.hpp"

#include <cstdint>
#include <random>

namespace Random
{
    inline std::mt19937 &Engine()
    {
        static std::mt19937 rng{std::random_device{}()};
        return rng;
    }

    inline void Seed(unsigned int value) { Engine().seed(value); }

    inline float Float(float min = 0.0f, float max = 1.0f)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(Engine());
    }

    inline int Int(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(Engine());
    }

    inline bool Chance(float probability01)
    {
        return Float(0.0f, 1.0f) <= probability01;
    }

    inline glm::vec3 Vec3(float min = -1.0f, float max = 1.0f)
    {
        return glm::vec3(Float(min, max), Float(min, max), Float(min, max));
    }

    inline glm::vec3 OnUnitSphere()
    {
        glm::vec3 v;
        do v = Vec3(-1.0f, 1.0f);
        while (glm::dot(v, v) > 1.0f);
        return glm::normalize(v);
    }
}

#endif
