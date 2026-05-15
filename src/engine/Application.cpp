#include "Application.hpp"

#include "Console.hpp"
#include "DebugOverlay.hpp"
#include "Engine.hpp"
#include "Input.hpp"
#include "Profiler.hpp"
#include "Time.hpp"
#include "Window.hpp"

void Application::SetTargetFPS(unsigned short fps) { targetFPS = fps; }

void Application::SetOnInit(std::function<void()> callback) { onInit = std::move(callback); }

void Application::SetOnFixedUpdate(std::function<void(double)> callback) { onFixedUpdate = std::move(callback); }

void Application::SetOnUpdate(std::function<void(double)> callback) { onUpdate = std::move(callback); }

void Application::SetOnRender(std::function<void()> callback) { onRender = std::move(callback); }

void Application::SetOnShutdown(std::function<void()> callback) { onShutdown = std::move(callback); }

void Application::Quit() { running = false; }

void Application::SetPaused(bool value) { paused = value; }

bool Application::IsPaused() const { return paused; }

void Application::StepOneFrame() { stepFrame = true; }

void Application::Run()
{
    if (!Window::IsCreated()) return;

    Time::Init();

    Console::SetApplication(this);

    if (onInit) onInit();

    while (running && !Window::ShouldClose())
    {
        Profiler::BeginFrame();
        Input::Poll();
        Console::Update();
        DebugOverlay::Update();
        Time::Tick(targetFPS);

        if (Input::ShouldClose())
        {
            Window::SetShouldClose(true);
            break;
        }

        if (!Time::IsFrameReady())
        {
            glfwWaitEventsTimeout(0.001);
            continue;
        }

        const double dt = Time::GetDeltaTime();
        const bool simulate = !paused || stepFrame;

        if (simulate)
        {
            double fixedDt = 0;
            while (Time::ConsumeFixedUpdate(fixedDt))
            {
                if (onFixedUpdate) onFixedUpdate(fixedDt);
            }

            if (onUpdate) onUpdate(dt);
        }

        stepFrame = false;

        if (onRender) onRender();

        Profiler::EndFrame();
        Input::EndFrame();
        glfwSwapBuffers(Window::GetHandle());
    }

    if (onShutdown) onShutdown();
}
