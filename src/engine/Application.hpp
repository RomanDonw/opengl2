#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <functional>

class Application final
{
    private:
        bool running = true;
        bool paused = false;
        bool stepFrame = false;
        unsigned short targetFPS = 100;

        std::function<void()> onInit;
        std::function<void(double)> onFixedUpdate;
        std::function<void(double)> onUpdate;
        std::function<void()> onRender;
        std::function<void()> onShutdown;

    public:
        void SetTargetFPS(unsigned short fps);
        void SetOnInit(std::function<void()> callback);
        void SetOnFixedUpdate(std::function<void(double)> callback);
        void SetOnUpdate(std::function<void(double)> callback);
        void SetOnRender(std::function<void()> callback);
        void SetOnShutdown(std::function<void()> callback);

        void Quit();
        void SetPaused(bool value);
        bool IsPaused() const;
        void StepOneFrame();

        void Run();
};

#endif
