#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "external/opengl.hpp"
#include "external/openal.hpp"
#include "external/physics.hpp"

#include <string>

class Engine final
{
    private:
        Engine() = delete;
        ~Engine();

        static inline bool inited = false;

        static inline GLFWwindow *window = nullptr;

    public:
        static inline std::string CurrentScene = "";

        static bool Init(unsigned short windowWidth, unsigned short windowHeight);
        static bool Shutdown();

        static GLFWwindow *GetWindow(); // can return nullptr.

        static bool Update(double delta);
        static bool Render();
};

#endif