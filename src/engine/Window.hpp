#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "WindowSettings.hpp"
#include "external/glm.hpp"
#include "external/opengl.hpp"

#include <string>

class Window final
{
    private:
        Window() = delete;

        static inline WindowSettings settings;
        static inline GLFWwindow *handle = nullptr;
        static inline bool isFullscreen = false;
        static inline int windowedX = 100;
        static inline int windowedY = 100;
        static inline int windowedWidth = 1280;
        static inline int windowedHeight = 720;

        static void applyHints();
        static void storeWindowedPlacement();

    public:
        static WindowSettings &GetSettings();
        static const WindowSettings &GetSettingsConst();

        static bool Create();
        static void Destroy();

        static GLFWwindow *GetHandle();
        static bool IsCreated();

        static glm::uvec2 GetSize();
        static void SetTitle(const std::string &title);
        static void SetSize(unsigned int width, unsigned int height);
        static void SetVSync(bool enabled);
        static bool IsVSyncEnabled();
        static void SetFullscreen(bool enabled);
        static bool IsFullscreen();
        static bool IsFocused();
        static bool ShouldClose();
        static void SetShouldClose(bool value);
        static void SetIconified(bool iconified);
        static bool IsIconified();
        static void Focus();
        static void ApplyRuntimeSettings();
};

#endif
