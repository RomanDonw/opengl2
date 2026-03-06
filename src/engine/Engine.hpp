#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "external/glm.hpp"
#include "external/opengl.hpp"
#include "external/openal.hpp"
#include "external/physics.hpp"

#include <string>
#include <utility>

class Engine final
{
    private:
        Engine() = delete;
        ~Engine();

        static inline bool inited = false;

        static inline GLFWwindow *window = nullptr;

        static void resizecallback(GLFWwindow *w, int width, int height);

    public:
        static inline std::string CurrentScene = "";

        static bool Init(unsigned short windowWidth, unsigned short windowHeight);
        static bool Shutdown();

        static GLFWwindow *GetWindow(); // can return nullptr.
        static glm::uvec2 GetWindowSize();

        static bool Update(double delta);
        static bool Render();
};

#endif