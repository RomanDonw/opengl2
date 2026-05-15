#ifndef EDITORSCRIPTEDITOR_HPP
#define EDITORSCRIPTEDITOR_HPP

class EditorState;

struct ImVec2;

class EditorScriptEditor final
{
    public:
        static void Render(EditorState &state, const ImVec2 &pos, const ImVec2 &size);
};

#endif
