# OpenGL 2 3D game engine.

This is attempt to create yet another 3D game engine with using OpenGL.

## Building
### Windows

You need to have latest versions of __CMake__ and __MinGW UCRT64__ (recommended to using MinGW UCRT64 from MSYS2).

1. Go to `build` folder in the root of project.
2. Run `cmake -S .. -G "MinGW Makefiles"`.
3. Execute `make` in the build folder.
4. Wait for building project.
5. Run output executable.

If you have some troubles with linking or running project try to rebuilt GLAD library by running `build_glad.bat` batch file.

### Linux

You need to change `CMakeLists.txt` file to using external libraries instead of libs that integrated to project, because they built for Windows.
Next do the same steps as for Windows build.