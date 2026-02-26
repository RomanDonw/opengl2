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
        ~Engine() = delete;

        static inline bool inited = false;

        static inline GLFWwindow *window = nullptr;
        static inline phys::PhysicsCommon physcommon = phys::PhysicsCommon();

    public:
        static inline std::string CurrentScene = "";

        static bool Init(unsigned short windowWidth, unsigned short windowHeight);
        static bool Shutdown();

        static GLFWwindow *GetWindow();

        static bool Update(double delta);
        static bool Render();
};

#endif