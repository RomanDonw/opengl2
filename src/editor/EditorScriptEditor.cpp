#include "EditorScriptEditor.hpp"

#include "EditorState.hpp"

#include "engine/external/imgui.hpp"
#include "engine/objects/ScriptBehaviour/ScriptBehaviour.hpp"

#include <cstring>

namespace
{
    constexpr int kBufSize = 65536;
    constexpr float kLineNumW = 52.0f;
    constexpr int kTabSpaces = 4;

    static char g_buffer[kBufSize];
    static std::string g_lastPath;
    static float g_scrollY = 0.0f;

    static int countLines(const char *text)
    {
        int n = 1;
        for (const char *p = text; *p; ++p)
            if (*p == '\n') ++n;
        return n;
    }

    static int inputCallback(ImGuiInputTextCallbackData *data)
    {
        if (data->EventFlag == ImGuiInputTextFlags_CallbackAlways)
        {
            if (ImGui::IsKeyPressed(ImGuiKey_Tab))
            {
                const char spaces[] = "    ";
                data->InsertChars(data->CursorPos, spaces, spaces + kTabSpaces);
                return 1;
            }
        }
        return 0;
    }

    static void renderLineNumbers(const char *text, float height)
    {
        const int lines = countLines(text);
        const float lineH = ImGui::GetTextLineHeight();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.14f, 1));
        ImGui::BeginChild("line_nums", ImVec2(kLineNumW, height), false);
        ImGui::SetScrollY(g_scrollY);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.48f, 0.55f, 1));
        for (int i = 1; i <= lines; ++i)
        {
            ImGui::Text("%4d", i);
            if (i < lines) ImGui::Dummy(ImVec2(0, lineH - ImGui::GetTextLineHeight()));
        }
        ImGui::PopStyleColor();
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }
}

void EditorScriptEditor::Render(EditorState &state, const ImVec2 &pos, const ImVec2 &size)
{
    ImGUI::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGUI::SetNextWindowSize(size, ImGuiCond_Always);
    ImGUI::Begin("Script", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGUI::Text("Object: %s", state.selected ? state.selected->displayName.c_str() : "(file)");
    ImGUI::SameLine();
    ImGUI::Text("| %s", state.scriptPath.empty() ? "(no file)" : state.scriptPath.c_str());
    if (state.scriptDirty)
    {
        ImGUI::SameLine();
        ImGUI::TextColored(ImVec4(1, 0.55f, 0.2f, 1), "*");
    }

    if (g_lastPath != state.scriptPath || (!state.scriptDirty && g_buffer[0] == '\0'))
    {
        std::memset(g_buffer, 0, kBufSize);
        std::strncpy(g_buffer, state.scriptBuffer.c_str(), kBufSize - 1);
        g_lastPath = state.scriptPath;
    }

    if (ImGUI::Button("Apply") && state.selected)
    {
        state.scriptBuffer = g_buffer;
        if (state.scriptPath.empty())
            state.scriptPath = state.GetScriptPathFor(state.selected);

        ScriptBehaviour *sb = state.EnsureScript(state.selected, false);
        sb->SetScriptFile(state.scriptPath);
        sb->SetSource(g_buffer);
        state.scriptDirty = false;
    }
    ImGUI::SameLine();
    if (ImGUI::Button("Save .ses") && state.selected)
    {
        state.scriptBuffer = g_buffer;
        if (state.scriptPath.empty())
            state.scriptPath = state.GetScriptPathFor(state.selected);
        state.SaveScriptToDisk();
        g_lastPath.clear();
    }
    ImGUI::SameLine();
    if (ImGUI::Button("Reload") && !state.scriptPath.empty())
    {
        state.LoadScriptFromDisk();
        g_lastPath.clear();
        std::memset(g_buffer, 0, kBufSize);
        std::strncpy(g_buffer, state.scriptBuffer.c_str(), kBufSize - 1);
    }

    const float editorH = ImGUI::GetContentRegionAvail().y;
    const float editorW = size.x - kLineNumW - 8.0f;

    renderLineNumbers(g_buffer, editorH);
    ImGUI::SameLine();

    ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackAlways;
    ImGUI::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.12f, 1));
    ImGUI::BeginChild("code_edit", ImVec2(editorW, editorH), false);
    if (ImGUI::InputTextMultiline("##code", g_buffer, kBufSize, ImVec2(-1, -1), flags, inputCallback))
    {
        state.scriptBuffer = g_buffer;
        state.scriptDirty = true;
    }
    g_scrollY = ImGUI::GetScrollY();
    ImGUI::EndChild();
    ImGUI::PopStyleColor();

    ImGUI::End();
}
