#ifndef TIMER_HPP
#define TIMER_HPP

#include <cstdint>
#include <functional>
#include <vector>

class Timer final
{
    private:
        Timer() = delete;

        struct Entry
        {
            uint64_t id;
            double remaining;
            double interval;
            bool repeating;
            bool active;
            std::function<void()> callback;
        };

        static inline uint64_t nextId = 1;
        static inline std::vector<Entry> entries;

    public:
        static uint64_t After(double delaySeconds, std::function<void()> callback);
        static uint64_t Every(double intervalSeconds, std::function<void()> callback);
        static bool Cancel(uint64_t id);
        static void ClearAll();
        static void Update(double delta);
        static size_t GetActiveCount();
};

#endif
