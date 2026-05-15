#include "Time.hpp"

#include "external/opengl.hpp"

void Time::Init()
{
    glfwSetTime(0);
    lastTickTime = 0;
    deltaTime = 0;
    unscaledDeltaTime = 0;
    time = 0;
    fixedAccumulator = 0;
    frameCount = 0;
    frameReady = false;
}

void Time::Tick(unsigned targetFPS)
{
    frameReady = false;

    const double now = glfwGetTime();
    unscaledDeltaTime = now - lastTickTime;

    if (targetFPS == 0 || unscaledDeltaTime < 1.0 / static_cast<double>(targetFPS)) return;

    deltaTime = unscaledDeltaTime * static_cast<double>(timeScale);
    lastTickTime = now;
    time += deltaTime;
    frameCount++;
    frameReady = true;
    fixedAccumulator += deltaTime;
}

bool Time::IsFrameReady() { return frameReady; }

double Time::GetDeltaTime() { return deltaTime; }

double Time::GetUnscaledDeltaTime() { return unscaledDeltaTime; }

double Time::GetFixedDeltaTime() { return fixedDeltaTime; }

void Time::SetFixedDeltaTime(double dt) { fixedDeltaTime = dt; }

double Time::GetTime() { return time; }

float Time::GetTimeScale() { return timeScale; }

void Time::SetTimeScale(float scale) { timeScale = scale; }

unsigned int Time::GetFrameCount() { return frameCount; }

bool Time::ConsumeFixedUpdate(double &outFixedDt)
{
    if (fixedAccumulator < fixedDeltaTime) return false;

    fixedAccumulator -= fixedDeltaTime;
    outFixedDt = fixedDeltaTime;
    return true;
}
