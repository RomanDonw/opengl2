#include "Engine.hpp"

#include <exception>
#include <stdexcept>

#include "ResourceManager.hpp"
#include "Scene.hpp"

// === PRIVATE ===

Engine::~Engine() { Shutdown(); }

void Engine::resizecallback(GLFWwindow *w, int width, int height) { if (width > 0 && height > 0) glViewport(0, 0, width, height); }

void Engine::shutdownwin()
{
    glfwMakeContextCurrent(NULL);
    glfwTerminate();
    window = nullptr;
}

void Engine::shutdownaudio()
{
    alcMakeContextCurrent(NULL);
    alcDestroyContext(audiocontext);
    audiocontext = nullptr;

    alcCloseDevice(audiodev);
    audiodev = nullptr;
}

// === PUBLIC ===

EngineInitReturnCode Engine::Init(unsigned short windowWidth, unsigned short windowHeight, const ALchar *audiodevname, bool enableHRTF)
{
    if (inited) return ERROR_ALREADY_INITED;

    // init OpenGL.

    if (initOpenGL(&window, windowWidth, windowHeight) != INITOPENGL_SUCCESS) return ERROR_OPENGL_INIT;
    glfwSetWindowSizeCallback(window, resizecallback);

    // init audio.

    audiodev = alcOpenDevice(audiodevname);
    if (!audiodev) { shutdownwin(); return ERROR_OPENAL_OPENDEVICE; }

    ALCint attribs[] = {ALC_HRTF_SOFT, enableHRTF ? ALC_TRUE : ALC_FALSE, 0};
    audiocontext = alcCreateContext(audiodev, attribs);
    if (!audiocontext)
    {
        alcCloseDevice(audiodev);
        audiodev = nullptr;

        shutdownwin();
        return ERROR_OPENAL_CREATECONTEXT;
    }

    alcMakeContextCurrent(audiocontext);

    if (alcIsExtensionPresent(audiodev, "ALC_EXT_EFX") == AL_FALSE) { shutdownaudio(); shutdownwin(); return ERROR_OPENAL_DETECT_EXT_EFX; }
    if (!initEFX()) { shutdownaudio(); shutdownwin(); return ERROR_OPENAL_INIT_EXT_EFX; }

    // ImGUI & physics.

    IMGUI_CHECKVERSION();
    ImGUI::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    phys = new rp3d::PhysicsCommon(&physalloc);

    inited = true;
    return SUCCESS;
}

bool Engine::Shutdown()
{
    if (!inited) return false;

    delete phys;
    phys = nullptr;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGUI::DestroyContext();

    shutdownaudio();
    shutdownwin();

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

bool Engine::IsKeyPressed(unsigned short keycode)
{
    if (!inited) throw std::runtime_error("engine isn't initialized");

    return glfwGetKey(window, keycode) == GLFW_PRESS;
}

bool Engine::IsMouseButtonPressed(unsigned char button)
{
    if (!inited) throw std::runtime_error("engine isn't initialized");

    return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

void Engine::SetAudioDistanceModel(ALenum model) { alDistanceModel(model); }

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

bool Engine::BeginRenderUI()
{
    if (!inited) return false;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGUI::NewFrame();

    return true;
}

bool Engine::EndRenderUI()
{
    if (!inited) return false;

    ImGUI::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGUI::GetDrawData());

    return true;
}