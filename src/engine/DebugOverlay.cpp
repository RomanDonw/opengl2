#include "DebugOverlay.hpp"

#include "Input.hpp"
#include "Time.hpp"
#include "Window.hpp"
#include "external/imgui.hpp"

#include <sstream>

void DebugOverlay::SetVisible(bool value) { visible = value; }

void DebugOverlay::Toggle() { visible = !visible; }

bool DebugOverlay::IsVisible() { return visible; }

void DebugOverlay::SetFps(unsigned int value) { fps = value; }

void DebugOverlay::Update()
{
    if (Input::IsKeyJustPressed(GLFW_KEY_F3)) Toggle();
}

void DebugOverlay::Render()
{
    if (!visible) return;

    const glm::uvec2 size = Window::GetSize();

    std::ostringstream oss;
    oss << "FPS: " << fps;
    oss << "\nFrame: " << Time::GetFrameCount();
    oss << "\nDelta: " << Time::GetDeltaTime() << "s";
    oss << "\nWindow: " << size.x << " x " << size.y;
    oss << "\nVSync: " << (Window::IsVSyncEnabled() ? "on" : "off");
    oss << "\nFullscreen: " << (Window::IsFullscreen() ? "yes" : "no");
    oss << "\nF3 overlay | P pause";

    ImGUI::GetForegroundDrawList()->AddText(ImVec2(10, 36), IM_COL32(200, 255, 200, 255), oss.str().c_str());
}
