#include "Mesh.hpp"

#include <cctype>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>

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

static int parseObjIndex(const std::string &token, int count)
{
    if (token.empty()) return -1;
    size_t slash = token.find('/');
    const std::string idxStr = token.substr(0, slash);
    int idx = std::stoi(idxStr);
    if (idx < 0) idx = count + idx + 1;
    return idx - 1;
}

static void pushTri(Mesh *mesh, const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2,
    const glm::vec2 &uv0, const glm::vec2 &uv1, const glm::vec2 &uv2, const glm::vec3 &n)
{
    const unsigned int base = static_cast<unsigned int>(mesh->vertices.size());
    mesh->vertices.push_back(p0);
    mesh->vertices.push_back(p1);
    mesh->vertices.push_back(p2);
    mesh->uvs.push_back(uv0);
    mesh->uvs.push_back(uv1);
    mesh->uvs.push_back(uv2);
    mesh->normals.push_back(n);
    mesh->normals.push_back(n);
    mesh->normals.push_back(n);
    mesh->indices.push_back(glm::uvec3(base, base + 1, base + 2));
}

bool Mesh::LoadFromObjFile(const std::string &filename)
{
    if (!std::filesystem::is_regular_file(filename)) return false;

    std::ifstream file(filename);
    if (!file) return false;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> normcoords;

    ClearMesh();

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string tag;
        iss >> tag;

        if (tag == "v")
        {
            glm::vec3 p;
            iss >> p.x >> p.y >> p.z;
            positions.push_back(p);
        }
        else if (tag == "vt")
        {
            glm::vec2 t;
            iss >> t.x >> t.y;
            texcoords.push_back(t);
        }
        else if (tag == "vn")
        {
            glm::vec3 n;
            iss >> n.x >> n.y >> n.z;
            normcoords.push_back(n);
        }
        else if (tag == "f")
        {
            std::vector<std::string> tokens;
            std::string part;
            while (iss >> part) tokens.push_back(part);
            if (tokens.size() < 3) continue;

            auto corner = [&](const std::string &tok) -> std::tuple<glm::vec3, glm::vec2, glm::vec3>
            {
                const int vi = parseObjIndex(tok, static_cast<int>(positions.size()));
                glm::vec3 p = vi >= 0 && vi < static_cast<int>(positions.size()) ? positions[vi] : glm::vec3(0);

                size_t slash1 = tok.find('/');
                size_t slash2 = tok.find('/', slash1 + 1);
                glm::vec2 uv(0);
                glm::vec3 n(0, 1, 0);

                if (slash1 != std::string::npos)
                {
                    const std::string tStr = tok.substr(slash1 + 1, slash2 == std::string::npos ? std::string::npos : slash2 - slash1 - 1);
                    if (!tStr.empty())
                    {
                        const int ti = parseObjIndex(tStr, static_cast<int>(texcoords.size()));
                        if (ti >= 0 && ti < static_cast<int>(texcoords.size())) uv = texcoords[ti];
                    }
                }

                if (slash2 != std::string::npos)
                {
                    const std::string nStr = tok.substr(slash2 + 1);
                    if (!nStr.empty())
                    {
                        const int ni = parseObjIndex(nStr, static_cast<int>(normcoords.size()));
                        if (ni >= 0 && ni < static_cast<int>(normcoords.size())) n = normcoords[ni];
                    }
                }

                return {p, uv, n};
            };

            const auto [p0, uv0, n0] = corner(tokens[0]);
            for (size_t i = 1; i + 1 < tokens.size(); ++i)
            {
                const auto [p1, uv1, n1] = corner(tokens[i]);
                const auto [p2, uv2, n2] = corner(tokens[i + 1]);
                glm::vec3 fn = n0;
                if (glm::length(fn) < 0.001f)
                {
                    fn = glm::normalize(glm::cross(p1 - p0, p2 - p0));
                    if (glm::length(fn) < 0.001f) fn = glm::vec3(0, 1, 0);
                }
                pushTri(this, p0, p1, p2, uv0, uv1, uv2, fn);
            }
        }
    }

    if (vertices.empty() || indices.empty()) return false;

    RegenerateBuffers();
    return true;
}

bool Mesh::LoadFromFile(const std::string &filename)
{
    if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".obj") return LoadFromObjFile(filename);

    const std::string objPath = filename.size() > 7 && filename.substr(filename.size() - 7) == ".ucmesh"
        ? filename.substr(0, filename.size() - 7) + ".obj"
        : filename + ".obj";

    if (std::filesystem::is_regular_file(objPath)) return LoadFromObjFile(objPath);
    if (std::filesystem::is_regular_file(filename)) return LoadFromUCMESHFile(filename);
    return false;
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