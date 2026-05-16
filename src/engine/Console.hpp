#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include "external/imgui/imgui.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

struct ConsoleLine
{
    std::string text;
    bool isError = false;
};

class Application;

class Console final
{
    private:
        Console() = delete;

        struct Command
        {
            std::string description;
            std::function<void(const std::vector<std::string> &)> handler;
        };

        static inline bool visible = false;
        static inline bool scrollToBottom = true;
        static inline char inputBuffer[512]{};
        static inline std::vector<ConsoleLine> lines;
        static inline std::unordered_map<std::string, Command> commands;
        static inline std::vector<std::string> history;
        static inline int historyIndex = -1;

        static void registerDefaults();
        static std::vector<std::string> tokenize(const std::string &line);
        static void execute(const std::string &line);
        static int inputCallback(ImGuiInputTextCallbackData *data);

    public:
        static void Init();
        static void Shutdown();
        static void SetApplication(Application *app);

        static void RegisterCommand(const std::string &name, const std::string &description, std::function<void(const std::vector<std::string> &)> handler);

        static void Log(const std::string &text);
        static void LogError(const std::string &text);

        static void Toggle();
        static void SetVisible(bool value);
        static bool IsVisible();

        static void Update();
        static void Render();
};

#endif
