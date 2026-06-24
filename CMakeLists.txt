cmake_minimum_required(VERSION 3.23)
project(opengl2)

option(BUILD_SHARED_LIBS "Build shared libs" OFF)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp" "src/*.c" "src/*.h")
add_executable(opengl2 ${SOURCES})
unset(SOURCES)

set_target_properties(opengl2 PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

#target_link_options(opengl2 PRIVATE
#    "-static-libgcc"
#    "-static-libstdc++"
#    "-Wl,-Bstatic,--whole-archive"
#    "-lwinpthread"
#    "-Wl,--no-whole-archive"
#    "-O2"
#)

#find_package(PkgConfig REQUIRED)
#set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON)
#set(ENV{PKG_CONFIG} "pkg-config --static")

# set includes path.

target_include_directories(opengl2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# link static libs.

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
target_link_libraries(opengl2 PRIVATE glfw OpenGL::GL)

find_package(OpenAL REQUIRED)
target_link_libraries(opengl2 PRIVATE OpenAL::OpenAL)

find_package(ReactPhysics3D REQUIRED)
target_link_libraries(opengl2 PRIVATE ReactPhysics3D::ReactPhysics3D)

find_package(SndFile REQUIRED)
target_link_libraries(opengl2 PRIVATE SndFile::sndfile)