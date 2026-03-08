#include "Engine.hpp"

#include "ResourceManager.hpp"
#include "Scene.hpp"

#include <exception>
#include <stdexcept>

// === PRIVATE ===

Engine::~Engine() { Shutdown(); DeleteAllScenes(); }

void Engine::resizecallback(GLFWwindow *w, int width, int height) { if (width > 0 && height > 0) glViewport(0, 0, width, height); }

// === PUBLIC ===

bool Engine::Init(unsigned short windowWidth, unsigned short windowHeight)
{
    if (inited) return false;

    if (initOpenGL(&window, windowWidth, windowHeight) != INITOPENGL_SUCCESS) return false;

    glfwSetWindowSizeCallback(window, resizecallback);

    inited = true;
    return true;
}

bool Engine::Shutdown()
{
    if (!inited) return false;

    glfwMakeContextCurrent(NULL);
    glfwTerminate();
    window = nullptr;

    inited = false;
    return true;
}

GLFWwindow *Engine::GetWindow() { return window; }

bool Engine::Update(double delta)
{
    if (!inited) return false;

    if (Engine::HasScene(currscene)) Engine::GetScene(currscene)->Update(delta);

    return true;
}

glm::uvec2 Engine::GetWindowSize()
{
    if (!inited) throw std::runtime_error("engine isn't initialized");

    int w, h;
    glfwGetWindowSize(window, &w, &h);

    return glm::uvec2(w, h);
}

bool Engine::Render()
{
    if (!inited) return false;

    int w, h;
    glfwGetWindowSize(window, &w, &h);
    if (w <= 0 || h <= 0) return false;

    if (Engine::HasScene(currscene)) Engine::GetScene(currscene)->Render();

    return true;
}

bool Engine::HasScene(std::string name) { return scenes.contains(name) && name.length() != 0; }

Scene *Engine::CreateScene(std::string name)
{
    if (HasScene(name) || name.length() == 0) return nullptr;

    Scene *ret = new Scene();
    scenes.insert({name, ret});
    return ret;
}

Scene *Engine::GetScene(std::string name)
{
    if (!HasScene(name) || name.length() == 0) return nullptr;
    return scenes.at(name);
}

bool Engine::DeleteScene(std::string name)
{
    if (!HasScene(name) || name.length() == 0) return false;

    delete GetScene(name);
    scenes.erase(name);
    return true;
}

void Engine::DeleteAllScenes()
{
    for (std::pair<std::string, Scene *> pair : scenes) DeleteScene(pair.first);
}

std::string Engine::GetCurrentScene() { return currscene; }

void Engine::SetCurrentScene(std::string name)
{
    if (name == currscene) return;

    if (HasScene(currscene)) GetScene(currscene)->OnSceneUnload();

    if (!HasScene(name)) { currscene = ""; return; }

    GetScene(name)->OnSceneLoad();
    currscene = name;
}