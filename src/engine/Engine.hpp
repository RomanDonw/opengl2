#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "external/glm.hpp"
#include "external/opengl.hpp"
#include "external/openal.hpp"
#include "external/physics.hpp"
#include "external/imgui.hpp"

#include <string>
#include <unordered_map>

#include "WindowSettings.hpp"
#include "physics/PhysicsHeapAllocator.hpp"

class Scene;
class RigidBody;

enum
{
    SUCCESS,
    ERROR_ALREADY_INITED,

    ERROR_OPENGL_INIT,

    ERROR_OPENAL_OPENDEVICE,
    ERROR_OPENAL_CREATECONTEXT,
    ERROR_OPENAL_DETECT_EXT_EFX,
    ERROR_OPENAL_INIT_EXT_EFX
} typedef EngineInitReturnCode;

class Engine final
{
    friend class Scene;
    friend class RigidBody;

    private:
        Engine() = delete;
        ~Engine();

        static inline bool inited = false;

        static inline GLFWwindow *window = nullptr;

        static inline ALCdevice *audiodev = nullptr;
        static inline ALCcontext *audiocontext = nullptr;

        static inline PhysicsHeapAllocator physalloc = PhysicsHeapAllocator();
        static inline rp3d::PhysicsCommon *phys = nullptr;

        static inline std::string currscene = "";
        static inline std::unordered_map<std::string, Scene *> scenes = std::unordered_map<std::string, Scene *>();

        static void resizecallback(GLFWwindow *w, int width, int height);
        static void shutdownwin();
        static void shutdownaudio();

    public:
        static EngineInitReturnCode Init(const WindowSettings &windowSettings, const ALchar *audiodevname, bool enableHRTF = false);
        static EngineInitReturnCode Init(unsigned short windowWidth, unsigned short windowHeight, const ALchar *audiodevname, bool enableHRTF = false);
        static bool Shutdown();

        static GLFWwindow *GetWindow();
        static glm::uvec2 GetWindowSize();
        static WindowSettings &GetWindowSettings();

        static void SetAudioDistanceModel(ALenum model);

        static bool HasScene(std::string name);
        static Scene *CreateScene(std::string name); // can return nullptr.
        static Scene *GetScene(std::string name); // can return nullptr.
        static bool DeleteScene(std::string name);
        static void DeleteAllScenes();

        static std::string GetCurrentScene();
        static void SetCurrentScene(std::string name);

        static bool Update(double delta);
        static bool Render();
        static bool BeginRenderUI();
        static bool EndRenderUI();
};

#endif