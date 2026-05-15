#include "Utils.hpp"

#include <cstdio>
#include <sstream>

namespace Utils
{
    glm::vec3 normalize(glm::vec3 v)
    {
        if (glm::length(v) == 0.0f) return glm::vec3(0.0f);
        return glm::normalize(v);
    }

    float sign(float x)
    {
        if (x > 0) return 1;
        else if (x < 0) return -1;
        return 0;
    }

    double sign(double x)
    {
        if (x > 0) return 1;
        else if (x < 0) return -1;
        return 0;
    }

    glm::vec3 angles(glm::vec3 v, float roll)
    {
        float len = glm::length(v);
        if (len == 0) return glm::vec3(0.0f);

        return glm::vec3(glm::asin(v.y / len), glm::atan(v.x, v.z), roll);
    }

    std::string tostring(glm::vec3 v)
    { return "{" + std::to_string(v.x) + "; " + std::to_string(v.y) + "; " + std::to_string(v.z) + "}"; }

    glm::vec3 wrapangles(glm::vec3 euler) { return glm::vec3(fmod(euler.x, 360.0f), fmod(euler.y, 360.0f), fmod(euler.z, 360.0f)); }

    glm::vec3 rp3dvec3toglmvec3(rp3d::Vector3 v) { return glm::vec3(v.x, v.y, v.z); }
    rp3d::Vector3 glmvec3torp3dvec3(glm::vec3 v) { return rp3d::Vector3(v.x, v.y, v.z); }

    glm::quat rp3dquattoglmquat(rp3d::Quaternion q) { return glm::quat(q.w, q.x, q.y, q.z); }
    rp3d::Quaternion glmquattorp3dquat(glm::quat q) { return rp3d::Quaternion(q.x, q.y, q.z, q.w); }

    Transform rp3dtransformtotransform(rp3d::Transform t) { return Transform(rp3dvec3toglmvec3(t.getPosition()), rp3dquattoglmquat(t.getOrientation())); }
    rp3d::Transform transformtorp3dtransform(Transform t) { return rp3d::Transform(glmvec3torp3dvec3(t.GetPosition()), glmquattorp3dquat(t.GetRotation())); }

    bool ReadTextFile(const std::string &filename, std::string *output)
    {
        const size_t BUFFER_SIZE = 4096;
        char buffer[BUFFER_SIZE];
        std::ostringstream oss;

        FILE *f = fopen(filename.c_str(), "r");
        if (!f) return false;

        while (!feof(f))
        {
            const size_t readbytes = fread(buffer, sizeof(char), BUFFER_SIZE, f);
            for (size_t i = 0; i < readbytes; i++) oss << buffer[i];
        }

        fclose(f);
        *output = oss.str();
        return true;
    }

    bool WriteTextFile(const std::string &filename, const std::string &content)
    {
        FILE *f = fopen(filename.c_str(), "w");
        if (!f) return false;
        const size_t written = fwrite(content.data(), 1, content.size(), f);
        fclose(f);
        return written == content.size();
    }
}
