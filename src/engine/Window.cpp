#include "Window.hpp"

#include <iostream>

WindowSettings &Window::GetSettings() { return settings; }

const WindowSettings &Window::GetSettingsConst() { return settings; }

void Window::applyHints()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, settings.glMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, settings.glMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, settings.resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, settings.decorated ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_SAMPLES, settings.msaaSamples > 0 ? settings.msaaSamples : 0);
}

void Window::storeWindowedPlacement()
{
    if (!handle) return;
    glfwGetWindowPos(handle, &windowedX, &windowedY);
    glfwGetWindowSize(handle, &windowedWidth, &windowedHeight);
}

bool Window::Create()
{
    if (handle) return true;

    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW." << std::endl;
        return false;
    }

    applyHints();

    GLFWmonitor *monitor = settings.fullscreen ? glfwGetPrimaryMonitor() : nullptr;
    handle = glfwCreateWindow(static_cast<int>(settings.width), static_cast<int>(settings.height), settings.title.c_str(), monitor, nullptr);

    if (!handle)
    {
        std::cout << "Can't initialize GLFW window." << std::endl;
        glfwTerminate();
        handle = nullptr;
        return false;
    }

    isFullscreen = settings.fullscreen;

    if (settings.maximizeOnStart && !isFullscreen) glfwMaximizeWindow(handle);

    glfwSetWindowSizeLimits(handle, settings.minWidth, settings.minHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glfwMakeContextCurrent(handle);
    SetVSync(settings.vsync);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD." << std::endl;
        glfwMakeContextCurrent(nullptr);
        glfwDestroyWindow(handle);
        handle = nullptr;
        glfwTerminate();
        return false;
    }

    int w, h;
    glfwGetFramebufferSize(handle, &w, &h);
    glViewport(0, 0, w, h);

    storeWindowedPlacement();
    return true;
}

void Window::Destroy()
{
    if (!handle)
    {
        if (glfwInit()) glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(nullptr);
    glfwDestroyWindow(handle);
    handle = nullptr;
    glfwTerminate();
}

GLFWwindow *Window::GetHandle() { return handle; }

bool Window::IsCreated() { return handle != nullptr; }

glm::uvec2 Window::GetSize()
{
    if (!handle) return glm::uvec2(0);

    int w, h;
    glfwGetWindowSize(handle, &w, &h);
    return glm::uvec2(w, h);
}

void Window::SetTitle(const std::string &title)
{
    settings.title = title;
    if (handle) glfwSetWindowTitle(handle, title.c_str());
}

void Window::SetSize(unsigned int width, unsigned int height)
{
    settings.width = width;
    settings.height = height;
    if (!handle || isFullscreen) return;

    glfwSetWindowSize(handle, static_cast<int>(width), static_cast<int>(height));
}

void Window::SetVSync(bool enabled)
{
    settings.vsync = enabled;
    if (handle) glfwSwapInterval(enabled ? 1 : 0);
}

bool Window::IsVSyncEnabled() { return settings.vsync; }

void Window::SetFullscreen(bool enabled)
{
    if (!handle) { settings.fullscreen = enabled; return; }

    if (enabled == isFullscreen) { settings.fullscreen = enabled; return; }

    if (enabled)
    {
        storeWindowedPlacement();
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(handle, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        isFullscreen = true;
    }
    else
    {
        glfwSetWindowMonitor(handle, nullptr, windowedX, windowedY, windowedWidth, windowedHeight, 0);
        isFullscreen = false;
    }

    settings.fullscreen = enabled;
}

bool Window::IsFullscreen() { return isFullscreen; }

bool Window::IsFocused()
{
    if (!handle) return false;
    return glfwGetWindowAttrib(handle, GLFW_FOCUSED) == GLFW_TRUE;
}

bool Window::ShouldClose()
{
    if (!handle) return true;
    return glfwWindowShouldClose(handle) == GLFW_TRUE;
}

void Window::SetShouldClose(bool value)
{
    if (handle) glfwSetWindowShouldClose(handle, value ? GLFW_TRUE : GLFW_FALSE);
}

void Window::SetIconified(bool iconified)
{
    if (!handle) return;
    if (iconified) glfwIconifyWindow(handle);
    else glfwRestoreWindow(handle);
}

bool Window::IsIconified()
{
    if (!handle) return false;
    return glfwGetWindowAttrib(handle, GLFW_ICONIFIED) == GLFW_TRUE;
}

void Window::Focus()
{
    if (handle) glfwFocusWindow(handle);
}

void Window::ApplyRuntimeSettings()
{
    if (!handle) return;

    SetTitle(settings.title);
    SetVSync(settings.vsync);

    if (settings.fullscreen != isFullscreen) SetFullscreen(settings.fullscreen);
    else if (!isFullscreen) SetSize(settings.width, settings.height);
}
