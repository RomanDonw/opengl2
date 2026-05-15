#include "EditorApplication.hpp"

#include "EditorResources.hpp"
#include "EditorState.hpp"
#include "EditorUI.hpp"
#include "SceneSerializer.hpp"

#include "engine/Console.hpp"
#include "engine/Engine.hpp"
#include "engine/Input.hpp"
#include "engine/ResourceManager.hpp"
#include "engine/resources/Mesh.hpp"
#include "engine/Logger.hpp"
#include "engine/Settings.hpp"
#include "engine/Time.hpp"
#include "engine/Timer.hpp"
#include "engine/Window.hpp"
#include "engine/external/imgui.hpp"
#include "engine/io/FileSystem.hpp"
#include "engine/render/RenderTarget.hpp"
#include "engine/script/ScriptEngine.hpp"

static EditorState g_state;

bool EditorApplication::Init()
{
    Settings::Load();
    if (Engine::Init(Settings::GetWindow(), nullptr) != SUCCESS) return false;

    Engine::SetAudioDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

    g_state.InitProject();
    Logger::Info("Project: " + g_state.project.rootPath);

    Logger::Info("Loading editor resources...");
    if (!EditorResources::LoadAll()) Logger::Warning("Some editor resources failed to load (check ./res/)");

    Logger::Info("Initializing editor scene...");
    g_state.EnsureScene();
    g_state.viewportTarget = new RenderTarget();

    Mesh *cubeMesh = ResourceManager::GetMesh("cube");
    const bool meshReady = cubeMesh && cubeMesh->HasBuffers();

    const std::string sceneAbs = g_state.project.Abs(g_state.scenePath);
    const bool sceneExists = FileSystem::Exists(sceneAbs);

    if (!meshReady || !sceneExists || !SceneSerializer::Load(g_state.scene, sceneAbs, &g_state.project))
    {
        Logger::Info("Creating new default scene");
        SceneSerializer::CreateDefaultScene(g_state.scene, &g_state.project);
        SceneSerializer::Save(g_state.scene, sceneAbs, &g_state.project);
    }

    g_state.EnsureEditorCamera();
    g_state.flyCamera.FocusOn(glm::vec3(0, 1, 0));
    g_state.SetEditorVisualDefaults();
    Logger::Info("Editor scene ready, objects: " + std::to_string(g_state.GetSceneObjectCount()));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Console::Log("SuperEngine Editor — project: " + g_state.project.config.name);
    Logger::Info("Editor initialized");
    return true;
}

void EditorApplication::Run()
{
    Time::Init();
    targetFPS = static_cast<unsigned short>(Settings::GetWindow().targetFPS);

    while (running && !Window::ShouldClose())
    {
        Input::Poll();
        Console::Update();
        Time::Tick(targetFPS);

        if (!Time::IsFrameReady())
        {
            glfwWaitEventsTimeout(0.001);
            continue;
        }

        const double dt = Time::GetDeltaTime();

        if (g_state.isPlaying)
        {
            Timer::Update(dt);
            Engine::Update(dt);
        }
        else
        {
            g_state.EnsureEditorCamera();
        }

        const glm::vec3 clearCol = g_state.GetSceneClearColor();
        glClearColor(clearCol.r, clearCol.g, clearCol.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Engine::BeginRenderUI();
        EditorUI::Render(g_state, dt);
        Console::Render();
        Engine::EndRenderUI();

        Input::EndFrame();
        glfwSwapBuffers(Window::GetHandle());
    }
}

void EditorApplication::Shutdown()
{
    g_state.EndPlay();
    g_state.SaveProject();

    delete g_state.viewportTarget;
    g_state.viewportTarget = nullptr;

    ScriptEngine::ClearAll();
    Settings::GetWindow() = Engine::GetWindowSettings();
    Settings::Save();
    Engine::Shutdown();
}
