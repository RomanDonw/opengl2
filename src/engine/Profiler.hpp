#ifndef PROFILER_HPP
#define PROFILER_HPP

#include <cstddef>
#include <string>
#include <vector>

struct ProfilerSample
{
    std::string name;
    double milliseconds = 0;
};

class Profiler final
{
    private:
        Profiler() = delete;

        struct Scope
        {
            std::string name;
            double startTime = 0;
        };

        static inline std::vector<Scope> scopeStack;
        static inline std::vector<ProfilerSample> frameSamples;
        static inline double frameStartTime = 0;
        static inline double frameTotalMs = 0;

        static double nowSeconds();

    public:
        static void BeginFrame();
        static void EndFrame();

        static void BeginScope(const char *name);
        static void EndScope();

        static double GetFrameMilliseconds();
        static const std::vector<ProfilerSample> &GetSamples();
};

class ProfilerScope final
{
    public:
        explicit ProfilerScope(const char *name) { Profiler::BeginScope(name); }
        ~ProfilerScope() { Profiler::EndScope(); }

        ProfilerScope(const ProfilerScope &) = delete;
        ProfilerScope &operator=(const ProfilerScope &) = delete;
};

#define SE_PROFILE_SCOPE(name) ProfilerScope _se_profile_scope_##__LINE__(name)

#endif
