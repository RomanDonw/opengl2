#include "EditorResources.hpp"

#include "../engine/Logger.hpp"
#include "../engine/ResourceManager.hpp"
#include "../engine/Utils.hpp"
#include "../engine/resources/Mesh.hpp"
#include "../engine/resources/ShaderProgram.hpp"

static bool loadMesh(const char *name, const char *objPath)
{
    Mesh *mesh = ResourceManager::CreateMesh(name);
    if (mesh->LoadFromObjFile(objPath))
    {
        Logger::Info(std::string("Loaded mesh: ") + name + " <- " + objPath);
        return true;
    }
    Logger::Error(std::string("Failed to load mesh: ") + objPath);
    return false;
}

bool EditorResources::LoadAll()
{
    bool ok = true;
    ShaderProgram *sh = ResourceManager::CreateShaderProgram("default");
    std::string log;
    std::string src;

    if (!Utils::ReadTextFile("./res/default.v.glsl", &src))
    {
        Logger::Error("Missing ./res/default.v.glsl — run from build/ directory");
        ok = false;
    }
    else sh->LoadVertexShader(src);

    if (!sh->CompileVertexShader(&log))
    {
        Logger::Error("Vertex shader: " + log);
        ok = false;
    }

    if (!Utils::ReadTextFile("./res/default.f.glsl", &src))
    {
        Logger::Error("Missing ./res/default.f.glsl");
        ok = false;
    }
    else sh->LoadFragmentShader(src);

    if (!sh->CompileFragmentShader(&log))
    {
        Logger::Error("Fragment shader: " + log);
        ok = false;
    }

    if (!sh->LinkShaderProgram(&log))
    {
        Logger::Error("Shader link: " + log);
        ok = false;
    }
    else Logger::Info("Default shader linked");

    if (!loadMesh("cube", "./res/models/cube.obj")) ok = false;
    loadMesh("sphere", "./res/models/sphere.obj");

    return ok;
}
