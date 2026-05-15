#include "demo/DemoScene.hpp"

#include "engine/Application.hpp"
#include "engine/Engine.hpp"
#include "engine/Settings.hpp"
#include "engine/Time.hpp"

int main()
{
    Settings::Load();

    if (Engine::Init(Settings::GetWindow(), NULL) != SUCCESS) return 1;

    Engine::SetAudioDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

    Application app;

    DemoScene demo;
    demo.SetApplication(&app);
    if (!demo.Load()) return 1;
    app.SetTargetFPS(static_cast<unsigned short>(Settings::GetWindow().targetFPS));

    app.SetOnUpdate([&](double delta) { demo.Update(delta); });

    app.SetOnRender([&]()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Engine::Render();
        Engine::BeginRenderUI();
        demo.RenderUI();
        Engine::EndRenderUI();
        demo.OnFramePresented();
    });

    app.SetOnShutdown([&]()
    {
        Settings::GetWindow() = Engine::GetWindowSettings();
        Settings::Save();
    });

    app.Run();
    Engine::Shutdown();
    return 0;
}
