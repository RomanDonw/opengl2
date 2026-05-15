#include "Timer.hpp"

#include <algorithm>

uint64_t Timer::After(double delaySeconds, std::function<void()> callback)
{
    Entry e;
    e.id = nextId++;
    e.remaining = delaySeconds > 0 ? delaySeconds : 0;
    e.interval = 0;
    e.repeating = false;
    e.active = true;
    e.callback = std::move(callback);
    entries.push_back(std::move(e));
    return e.id;
}

uint64_t Timer::Every(double intervalSeconds, std::function<void()> callback)
{
    Entry e;
    e.id = nextId++;
    e.remaining = intervalSeconds > 0 ? intervalSeconds : 0;
    e.interval = intervalSeconds > 0 ? intervalSeconds : 0;
    e.repeating = true;
    e.active = true;
    e.callback = std::move(callback);
    entries.push_back(std::move(e));
    return e.id;
}

bool Timer::Cancel(uint64_t id)
{
    for (Entry &e : entries)
    {
        if (e.id == id && e.active)
        {
            e.active = false;
            return true;
        }
    }
    return false;
}

void Timer::ClearAll() { entries.clear(); }

void Timer::Update(double delta)
{
    for (Entry &e : entries)
    {
        if (!e.active) continue;

        e.remaining -= delta;
        if (e.remaining > 0) continue;

        if (e.callback) e.callback();

        if (e.repeating && e.interval > 0)
        {
            e.remaining += e.interval;
            while (e.remaining <= 0) e.remaining += e.interval;
        }
        else e.active = false;
    }

    entries.erase(std::remove_if(entries.begin(), entries.end(), [](const Entry &e) { return !e.active; }), entries.end());
}

size_t Timer::GetActiveCount()
{
    return static_cast<size_t>(std::count_if(entries.begin(), entries.end(), [](const Entry &e) { return e.active; }));
}
