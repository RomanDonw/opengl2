#ifndef DEBUGOVERLAY_HPP
#define DEBUGOVERLAY_HPP

class DebugOverlay final
{
    private:
        DebugOverlay() = delete;

        static inline bool visible = true;
        static inline unsigned int fps = 0;

    public:
        static void SetVisible(bool value);
        static void Toggle();
        static bool IsVisible();

        static void SetFps(unsigned int value);
        static void Update();
        static void Render();
};

#endif
