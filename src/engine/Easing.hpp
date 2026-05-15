#ifndef EASING_HPP
#define EASING_HPP

#include <cmath>

namespace Easing
{
    inline float Linear(float t) { return t; }

    inline float SmoothStep(float t)
    {
        t = t < 0 ? 0 : (t > 1 ? 1 : t);
        return t * t * (3.0f - 2.0f * t);
    }

    inline float InQuad(float t) { return t * t; }
    inline float OutQuad(float t) { return 1.0f - (1.0f - t) * (1.0f - t); }
    inline float InOutQuad(float t) { return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f; }

    inline float InCubic(float t) { return t * t * t; }
    inline float OutCubic(float t) { return 1.0f - std::pow(1.0f - t, 3.0f); }
    inline float InOutCubic(float t) { return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f; }

    inline float InElastic(float t)
    {
        if (t == 0 || t == 1) return t;
        return -std::pow(2.0f, 10.0f * t - 10.0f) * std::sin((t * 10.0f - 10.75f) * (2.0f * 3.14159265f / 3.0f));
    }

    inline float OutBounce(float t)
    {
        constexpr float n1 = 7.5625f;
        constexpr float d1 = 2.75f;

        if (t < 1.0f / d1) return n1 * t * t;
        if (t < 2.0f / d1) { t -= 1.5f / d1; return n1 * t * t + 0.75f; }
        if (t < 2.5f / d1) { t -= 2.25f / d1; return n1 * t * t + 0.9375f; }
        t -= 2.625f / d1;
        return n1 * t * t + 0.984375f;
    }
}

#endif
