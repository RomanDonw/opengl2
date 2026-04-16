#include "Mesh.hpp"

#include <cstring>
#include <filesystem>
#include <iterator>

struct
{
    float x, y, z;
    float u, v;
} typedef UCMESHVertexInfo;

struct
{
    unsigned int v0, v1, v2;
} typedef UCMESHTriangleInfo;

struct
{
    unsigned int v0, v1, v2, v3;
} typedef UCMESHQuadInfo;

Mesh::Mesh() {}
Mesh::~Mesh() { DeleteBuffers(); }

void Mesh::ClearMesh()
{
    vertices.clear();
    uvs.clear();
    indices.clear();
}

bool Mesh::HasBuffers() { return hasbuffers; }

bool Mesh::GenerateBuffers()
{
    if (hasbuffers) return false;

    // generate buffers.

    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO_VERTEX);
    glGenBuffers(1, &VBO_UV);
    glGenBuffers(1, &VBO_NORMAL);

    glGenBuffers(1, &EBO);

    // bind data to buffers.

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_VERTEX);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_NORMAL);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uvec3), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    hasbuffers = true;
    return true;
}

bool Mesh::DeleteBuffers()
{
    if (!hasbuffers) return false;

    glDeleteVertexArrays(1, &VAO);

    glDeleteBuffers(1, &VBO_VERTEX);
    glDeleteBuffers(1, &VBO_UV);
    glDeleteBuffers(1, &VBO_NORMAL);

    glDeleteBuffers(1, &EBO);

    hasbuffers = false;
    return true;
}

void Mesh::RegenerateBuffers()
{
    DeleteBuffers();
    GenerateBuffers();
}

void Mesh::ApplyTransformation(glm::mat4 mat)
{
    for (size_t i = 0; i < vertices.size(); i++)
    {
        glm::vec4 v = mat * glm::vec4(vertices[i].x, vertices[i].y, vertices[i].z, 0);
        vertices[i] = glm::vec3(v.x, v.y, v.z);
    }
}
void Mesh::ApplyTransformation(Transform t) { ApplyTransformation(t.GetTransformationMatrix()); }

void Mesh::FlipMesh()
{
    for (size_t i = 0; i < indices.size(); i++)
    {
        glm::uvec3 tri = indices[i];
        indices[i] = glm::uvec3(tri.z, tri.y, tri.x);
    }
}

bool Mesh::LoadFromUCMESHFile(std::string filename)
{
    if (!std::filesystem::is_regular_file(filename)) return false;

    FILE *f = fopen(filename.c_str(), "rb");
    if (!f) return false;

    char sig[6];
    fread(&sig, sizeof(char), 6, f);
    if (feof(f) || strncmp(sig, "UCMESH", 6)) goto fileerrorquit;

    uint16_t version;
    fread(&version, sizeof(version), 1, f);
    if (feof(f) || version != 0) goto fileerrorquit;

    uint32_t vertices_count;
    fread(&vertices_count, sizeof(vertices_count), 1, f);
    if (feof(f)) goto fileerrorquit;

    uint32_t primitives_count;
    fread(&primitives_count, sizeof(primitives_count), 1, f);
    if (feof(f)) goto fileerrorquit;

    ClearMesh();

    UCMESHVertexInfo v;
    for (uint32_t i = 0; i < vertices_count; i++)
    {
        fread(&v, sizeof(v), 1, f);
        if (feof(f)) goto readmesherrorquit;

        vertices.push_back(glm::vec3(v.x, v.y, v.z));
        uvs.push_back(glm::vec2(v.u, v.v));
    }

    uint8_t prim_type;
    for (uint32_t i = 0; i < primitives_count; i++)
    {
        fread(&prim_type, sizeof(prim_type), 1, f);
        if (feof(f)) goto readmesherrorquit;

        switch (prim_type)
        {
            case 0: // triangle.
                UCMESHTriangleInfo tri;
                fread(&tri, sizeof(tri), 1, f);
                if (feof(f)) goto readmesherrorquit;

                if (tri.v0 >= vertices_count || tri.v1 >= vertices_count || tri.v2 >= vertices_count) continue;

                indices.push_back(glm::uvec3(tri.v0, tri.v1, tri.v2));
                break;

            case 1: // quad.
                UCMESHQuadInfo quad;
                fread(&quad, sizeof(quad), 1, f);
                if (feof(f)) goto readmesherrorquit;

                if (quad.v0 >= vertices_count || quad.v1 >= vertices_count || quad.v2 >= vertices_count || quad.v3 >= vertices_count) continue;

                indices.push_back(glm::uvec3(quad.v3, quad.v0, quad.v1));
                indices.push_back(glm::uvec3(quad.v1, quad.v2, quad.v3));
                break;

            default:
                goto readmesherrorquit;
        }
    }

    RegenerateBuffers();

    fclose(f);
    return true;

    readmesherrorquit:
        ClearMesh();
    fileerrorquit:
        fclose(f);
    return false;
}

bool Mesh::RenderMesh()
{
    if (!HasBuffers()) return false;

    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);

    return true;
}