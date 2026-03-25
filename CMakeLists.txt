cmake_minimum_required(VERSION 3.23)
project(opengl2)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp" "src/*.h")

add_executable(opengl2 ${SOURCES})

set_target_properties(opengl2 PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_options(opengl2 PRIVATE
    "-static-libgcc"
    "-static-libstdc++"
    "-Wl,-Bstatic,--whole-archive"
    "-lwinpthread"
    "-Wl,--no-whole-archive"
    "-O2"
)

# set includes path.

target_include_directories(opengl2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# link static libs.

find_package(glfw3 REQUIRED)
target_link_libraries(opengl2 PRIVATE glfw3)

#target_link_libraries(opengl2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw3.a")
target_link_libraries(opengl2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libglad.a")
#target_link_libraries(opengl2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libJolt.dll.a")
target_link_libraries(opengl2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libreactphysics3d.a")
target_link_libraries(opengl2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libz.a")
target_link_libraries(opengl2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libmysofa.a")
target_link_libraries(opengl2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libopenal.dll.a")