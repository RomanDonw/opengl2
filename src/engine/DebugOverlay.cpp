#include "DebugOverlay.hpp"

#include "Input.hpp"
#include "Profiler.hpp"
#include "Time.hpp"
#include "Timer.hpp"
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
    oss << "\nFrame ms: " << Profiler::GetFrameMilliseconds();
    oss << "\nTimers: " << Timer::GetActiveCount();
    oss << "\nF3 overlay | ` console | P pause";

    ImGUI::GetForegroundDrawList()->AddText(ImVec2(10, 36), IM_COL32(200, 255, 200, 255), oss.str().c_str());

    if (!Profiler::GetSamples().empty())
    {
        std::ostringstream prof;
        prof << "Profiler:";
        for (const ProfilerSample &s : Profiler::GetSamples()) prof << "\n  " << s.name << ": " << s.milliseconds << " ms";
        ImGUI::GetForegroundDrawList()->AddText(ImVec2(10, 150), IM_COL32(180, 220, 255, 255), prof.str().c_str());
    }
}
