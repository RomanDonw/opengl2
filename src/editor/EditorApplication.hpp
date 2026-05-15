#ifndef EDITORAPPLICATION_HPP
#define EDITORAPPLICATION_HPP

class EditorApplication final
{
    private:
        bool running = true;
        unsigned short targetFPS = 120;

    public:
        bool Init();
        void Run();
        void Shutdown();
};

#endif
