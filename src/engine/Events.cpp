#include "Events.hpp"

uint64_t Events::SubscribeKey(std::function<void(const KeyEvent &)> handler)
{
    const uint64_t id = nextId++;
    keyHandlers.push_back({id, std::move(handler)});
    return id;
}

uint64_t Events::SubscribeMouseButton(std::function<void(const MouseButtonEvent &)> handler)
{
    const uint64_t id = nextId++;
    mouseButtonHandlers.push_back({id, std::move(handler)});
    return id;
}

uint64_t Events::SubscribeMouseMove(std::function<void(const MouseMoveEvent &)> handler)
{
    const uint64_t id = nextId++;
    mouseMoveHandlers.push_back({id, std::move(handler)});
    return id;
}

uint64_t Events::SubscribeScroll(std::function<void(const ScrollEvent &)> handler)
{
    const uint64_t id = nextId++;
    scrollHandlers.push_back({id, std::move(handler)});
    return id;
}

uint64_t Events::SubscribeChar(std::function<void(const CharEvent &)> handler)
{
    const uint64_t id = nextId++;
    charHandlers.push_back({id, std::move(handler)});
    return id;
}

template<typename T>
static void eraseById(std::vector<T> &vec, uint64_t id)
{
    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (vec[i].id == id)
        {
            vec.erase(vec.begin() + static_cast<std::ptrdiff_t>(i));
            return;
        }
    }
}

void Events::UnsubscribeKey(uint64_t id) { eraseById(keyHandlers, id); }
void Events::UnsubscribeMouseButton(uint64_t id) { eraseById(mouseButtonHandlers, id); }
void Events::UnsubscribeMouseMove(uint64_t id) { eraseById(mouseMoveHandlers, id); }
void Events::UnsubscribeScroll(uint64_t id) { eraseById(scrollHandlers, id); }
void Events::UnsubscribeChar(uint64_t id) { eraseById(charHandlers, id); }

void Events::DispatchKey(const KeyEvent &event)
{
    for (const KeySubscription &sub : keyHandlers) sub.handler(event);
}

void Events::DispatchMouseButton(const MouseButtonEvent &event)
{
    for (const MouseButtonSubscription &sub : mouseButtonHandlers) sub.handler(event);
}

void Events::DispatchMouseMove(const MouseMoveEvent &event)
{
    for (const MouseMoveSubscription &sub : mouseMoveHandlers) sub.handler(event);
}

void Events::DispatchScroll(const ScrollEvent &event)
{
    for (const ScrollSubscription &sub : scrollHandlers) sub.handler(event);
}

void Events::DispatchChar(const CharEvent &event)
{
    for (const CharSubscription &sub : charHandlers) sub.handler(event);
}

void Events::ClearAll()
{
    keyHandlers.clear();
    mouseButtonHandlers.clear();
    mouseMoveHandlers.clear();
    scrollHandlers.clear();
    charHandlers.clear();
}
