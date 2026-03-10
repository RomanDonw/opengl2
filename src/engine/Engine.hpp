#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "external/glm.hpp"
#include "external/opengl.hpp"
#include "external/openal.hpp"
#include "external/physics.hpp"

#include <string>
#include <unordered_map>

class Scene;
class AudioDevice;

class Engine final
{
    private:
        Engine() = delete;
        ~Engine();

        static inline bool inited = false;

        static inline GLFWwindow *window = nullptr;
        static inline AudioDevice *currdev = nullptr;

        static void resizecallback(GLFWwindow *w, int width, int height);

        static inline std::string currscene = "";
        static inline std::unordered_map<std::string, Scene *> scenes = std::unordered_map<std::string, Scene *>();

    public:
        static bool Init(unsigned short windowWidth, unsigned short windowHeight);
        static bool Shutdown();

        static GLFWwindow *GetWindow(); // can return nullptr.
        static glm::uvec2 GetWindowSize();
        static bool IsKeyPressed(unsigned short keycode);
        static bool IsMouseButtonPressed(unsigned char button);

        static AudioDevice *GetCurrentAudioDevice(); // can return nullptr.
        static void SetCurrentAudioDevice(AudioDevice *device); // can accept nullptr.

        static void SetDistanceModel(ALenum model);

        static bool HasScene(std::string name);
        static Scene *CreateScene(std::string name); // can return nullptr.
        static Scene *GetScene(std::string name); // can return nullptr.
        static bool DeleteScene(std::string name);
        static void DeleteAllScenes();

        static std::string GetCurrentScene();
        static void SetCurrentScene(std::string name);

        static bool Update(double delta);
        static bool Render();
};

#endif