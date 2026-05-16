#include "Console.hpp"

#include "Application.hpp"

#include "external/glm.hpp"
#include "Engine.hpp"
#include "Input.hpp"
#include "Scene.hpp"
#include "Time.hpp"
#include "Window.hpp"
#include "external/imgui.hpp"

#include <algorithm>
#include <sstream>

static Application *linkedApp = nullptr;

void Console::SetApplication(Application *app) { linkedApp = app; }

void Console::registerDefaults()
{
    RegisterCommand("help", "List commands", [](const std::vector<std::string> &)
    {
        for (const auto &pair : commands)
            Log(pair.first + " - " + pair.second.description);
    });

    RegisterCommand("clear", "Clear console output", [](const std::vector<std::string> &) { lines.clear(); });

    RegisterCommand("fps", "Show frame stats", [](const std::vector<std::string> &)
    {
        std::ostringstream oss;
        oss << "FPS frame=" << Time::GetFrameCount() << " dt=" << Time::GetDeltaTime() << "s scale=" << Time::GetTimeScale();
        Log(oss.str());
    });

    RegisterCommand("timescale", "Set time scale (timescale <value>)", [](const std::vector<std::string> &args)
    {
        if (args.size() < 2) { LogError("usage: timescale <value>"); return; }
        Time::SetTimeScale(std::stof(args[1]));
        Log("Time scale set to " + args[1]);
    });

    RegisterCommand("pause", "Toggle pause", [](const std::vector<std::string> &)
    {
        if (!linkedApp) { LogError("Application not linked"); return; }
        linkedApp->SetPaused(!linkedApp->IsPaused());
        Log(linkedApp->IsPaused() ? "Paused" : "Resumed");
    });

    RegisterCommand("quit", "Exit application", [](const std::vector<std::string> &)
    {
        if (linkedApp) linkedApp->Quit();
        Window::SetShouldClose(true);
    });

    RegisterCommand("gravity", "Set scene gravity (gravity x y z)", [](const std::vector<std::string> &args)
    {
        if (args.size() < 4) { LogError("usage: gravity x y z"); return; }
        Scene *scene = Engine::GetScene(Engine::GetCurrentScene());
        if (!scene) { LogError("No active scene"); return; }
        scene->SetGravity(glm::vec3(std::stof(args[1]), std::stof(args[2]), std::stof(args[3])));
        Log("Gravity updated");
    });
}

void Console::Init()
{
    registerDefaults();
    Log("Console ready. Type 'help' for commands. Press ` to toggle.");
}

void Console::Shutdown()
{
    commands.clear();
    lines.clear();
    history.clear();
    linkedApp = nullptr;
}

void Console::RegisterCommand(const std::string &name, const std::string &description, std::function<void(const std::vector<std::string> &)> handler)
{
    commands[name] = Command{description, std::move(handler)};
}

void Console::Log(const std::string &text) { lines.push_back({text, false}); scrollToBottom = true; }

void Console::LogError(const std::string &text) { lines.push_back({text, true}); scrollToBottom = true; }

void Console::Toggle() { visible = !visible; }

void Console::SetVisible(bool value) { visible = value; }

bool Console::IsVisible() { return visible; }

void Console::Update()
{
    if (Input::IsKeyJustPressed(GLFW_KEY_GRAVE_ACCENT)) Toggle();
}

std::vector<std::string> Console::tokenize(const std::string &line)
{
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) tokens.push_back(token);
    return tokens;
}

void Console::execute(const std::string &line)
{
    if (line.empty()) return;

    Log("> " + line);
    history.push_back(line);
    historyIndex = -1;

    const std::vector<std::string> tokens = tokenize(line);
    if (tokens.empty()) return;

    const auto it = commands.find(tokens[0]);
    if (it == commands.end())
    {
        LogError("Unknown command: " + tokens[0]);
        return;
    }

    it->second.handler(tokens);
}

int Console::inputCallback(ImGuiInputTextCallbackData *data)
{
    if (data->EventFlag != ImGuiInputTextFlags_CallbackHistory)
        return 0;

    const int prevIndex = historyIndex;
    if (data->EventKey == ImGuiKey_UpArrow)
    {
        if (historyIndex == -1)
            historyIndex = static_cast<int>(history.size()) - 1;
        else if (historyIndex > 0)
            --historyIndex;
    }
    else if (data->EventKey == ImGuiKey_DownArrow)
    {
        if (historyIndex != -1 && ++historyIndex >= static_cast<int>(history.size()))
            historyIndex = -1;
    }

    if (prevIndex != historyIndex)
    {
        const char *historyStr = (historyIndex >= 0) ? history[historyIndex].c_str() : "";
        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, historyStr);
    }

    return 0;
}

void Console::Render()
{
    if (!visible) return;

    ImGUI::SetNextWindowSize(ImVec2(640, 360), ImGuiCond_FirstUseEver);
    ImGUI::Begin("Console", &visible);

    const float footerHeight = ImGUI::GetStyle().ItemSpacing.y + ImGUI::GetFrameHeightWithSpacing();
    ImGUI::BeginChild("ScrollingRegion", ImVec2(0, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar);

    for (const ConsoleLine &line : lines)
    {
        const ImU32 col = line.isError ? IM_COL32(255, 120, 120, 255) : IM_COL32(220, 220, 220, 255);
        ImGUI::PushStyleColor(ImGuiCol_Text, col);
        ImGUI::TextUnformatted(line.text.c_str());
        ImGUI::PopStyleColor();
    }

    if (scrollToBottom && ImGUI::GetScrollY() >= ImGUI::GetScrollMaxY()) ImGUI::SetScrollHereY(1.0f);
    scrollToBottom = false;

    ImGUI::EndChild();

    const ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory;
    if (ImGUI::InputText("##input", inputBuffer, sizeof(inputBuffer), flags, inputCallback))
    {
        execute(inputBuffer);
        inputBuffer[0] = '\0';
        scrollToBottom = true;
    }

    ImGUI::End();
}
