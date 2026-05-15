#ifndef EDITORUI_HPP
#define EDITORUI_HPP

class EditorState;

class EditorUI final
{
    public:
        static void Render(EditorState &state, double delta);
};

#endif
