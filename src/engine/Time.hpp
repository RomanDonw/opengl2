#ifndef TIME_HPP
#define TIME_HPP

class Time final
{
    private:
        Time() = delete;

        static inline double lastTickTime = 0;
        static inline double deltaTime = 0;
        static inline double unscaledDeltaTime = 0;
        static inline double fixedDeltaTime = 1.0 / 100.0;
        static inline double time = 0;
        static inline double fixedAccumulator = 0;
        static inline float timeScale = 1.0f;
        static inline unsigned int frameCount = 0;
        static inline bool frameReady = false;

    public:
        static void Init();
        static void Tick(unsigned targetFPS);

        static bool IsFrameReady();
        static double GetDeltaTime();
        static double GetUnscaledDeltaTime();
        static double GetFixedDeltaTime();
        static void SetFixedDeltaTime(double dt);
        static double GetTime();
        static float GetTimeScale();
        static void SetTimeScale(float scale);
        static unsigned int GetFrameCount();
        static bool ConsumeFixedUpdate(double &outFixedDt);
};

#endif
