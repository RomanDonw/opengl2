#include "editor/EditorApplication.hpp"

int main()
{
    EditorApplication editor;
    if (!editor.Init()) return 1;
    editor.Run();
    editor.Shutdown();
    return 0;
}
