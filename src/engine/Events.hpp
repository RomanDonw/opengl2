#ifndef EVENTS_HPP
#define EVENTS_HPP

#include <cstdint>
#include <functional>
#include <vector>

struct KeyEvent
{
    int key = 0;
    int action = 0;
    int mods = 0;
};

struct MouseButtonEvent
{
    int button = 0;
    int action = 0;
    int mods = 0;
};

struct MouseMoveEvent
{
    double x = 0;
    double y = 0;
    double dx = 0;
    double dy = 0;
};

struct ScrollEvent
{
    double xoffset = 0;
    double yoffset = 0;
};

struct CharEvent
{
    unsigned int codepoint = 0;
};

class Events final
{
    private:
        Events() = delete;

        struct KeySubscription { uint64_t id; std::function<void(const KeyEvent &)> handler; };
        struct MouseButtonSubscription { uint64_t id; std::function<void(const MouseButtonEvent &)> handler; };
        struct MouseMoveSubscription { uint64_t id; std::function<void(const MouseMoveEvent &)> handler; };
        struct ScrollSubscription { uint64_t id; std::function<void(const ScrollEvent &)> handler; };
        struct CharSubscription { uint64_t id; std::function<void(const CharEvent &)> handler; };

        static inline uint64_t nextId = 1;
        static inline std::vector<KeySubscription> keyHandlers;
        static inline std::vector<MouseButtonSubscription> mouseButtonHandlers;
        static inline std::vector<MouseMoveSubscription> mouseMoveHandlers;
        static inline std::vector<ScrollSubscription> scrollHandlers;
        static inline std::vector<CharSubscription> charHandlers;

    public:
        static uint64_t SubscribeKey(std::function<void(const KeyEvent &)> handler);
        static uint64_t SubscribeMouseButton(std::function<void(const MouseButtonEvent &)> handler);
        static uint64_t SubscribeMouseMove(std::function<void(const MouseMoveEvent &)> handler);
        static uint64_t SubscribeScroll(std::function<void(const ScrollEvent &)> handler);
        static uint64_t SubscribeChar(std::function<void(const CharEvent &)> handler);

        static void UnsubscribeKey(uint64_t id);
        static void UnsubscribeMouseButton(uint64_t id);
        static void UnsubscribeMouseMove(uint64_t id);
        static void UnsubscribeScroll(uint64_t id);
        static void UnsubscribeChar(uint64_t id);

        static void DispatchKey(const KeyEvent &event);
        static void DispatchMouseButton(const MouseButtonEvent &event);
        static void DispatchMouseMove(const MouseMoveEvent &event);
        static void DispatchScroll(const ScrollEvent &event);
        static void DispatchChar(const CharEvent &event);

        static void ClearAll();
};

#endif
