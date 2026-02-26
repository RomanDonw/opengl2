#include "Engine.hpp"

#include "ResourceManager.hpp"
#include "Scene.hpp"

bool Engine::Init(unsigned short windowWidth, unsigned short windowHeight)
{
    if (inited) return false;

    if (initOpenGL(&window, windowWidth, windowHeight) != INITOPENGL_SUCCESS) return false;

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

    if (ResourceManager::HasScene(CurrentScene)) ResourceManager::GetScene(CurrentScene)->Update(delta);

    return true;
}

bool Engine::Render()
{
    if (!inited) return false;

    if (ResourceManager::HasScene(CurrentScene)) ResourceManager::GetScene(CurrentScene)->Render();

    return true;
}