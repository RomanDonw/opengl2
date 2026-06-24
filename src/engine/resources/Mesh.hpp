#ifndef MESH_HPP
#define MESH_HPP

#include "../external/glm.hpp"
#include "../external/opengl.hpp"

#include <vector>
#include <string>

#include "../Transform.hpp"

class ResourceManager;

class Mesh final
{
    friend class ResourceManager;

    private:
        bool hasbuffers = false;
        GLuint VAO, VBO_VERTEX, VBO_UV, VBO_NORMAL, EBO;

        Mesh();
        ~Mesh();

    public:
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;
        std::vector<glm::uvec3> indices;

        void ClearMesh();

        bool HasBuffers();
        bool GenerateBuffers();
        bool DeleteBuffers();
        void RegenerateBuffers();

        void ApplyTransformation(glm::mat4 mat);
        void ApplyTransformation(Transform t);

        void FlipMesh();

        bool LoadFromUCMESHFile(std::string filename);
        bool RenderMesh();
};

#endif