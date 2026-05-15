#include "Profiler.hpp"

#include "external/opengl.hpp"

double Profiler::nowSeconds() { return glfwGetTime(); }

void Profiler::BeginFrame()
{
    frameSamples.clear();
    frameStartTime = nowSeconds();
}

void Profiler::EndFrame() { frameTotalMs = (nowSeconds() - frameStartTime) * 1000.0; }

void Profiler::BeginScope(const char *name)
{
    Scope s;
    s.name = name ? name : "?";
    s.startTime = nowSeconds();
    scopeStack.push_back(std::move(s));
}

void Profiler::EndScope()
{
    if (scopeStack.empty()) return;

    const Scope s = scopeStack.back();
    scopeStack.pop_back();

    ProfilerSample sample;
    sample.name = s.name;
    sample.milliseconds = (nowSeconds() - s.startTime) * 1000.0;
    frameSamples.push_back(std::move(sample));
}

double Profiler::GetFrameMilliseconds() { return frameTotalMs; }

const std::vector<ProfilerSample> &Profiler::GetSamples() { return frameSamples; }
