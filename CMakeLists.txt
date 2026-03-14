cmake_minimum_required(VERSION 3.23)
project(opengl2)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp")

add_executable(opengl2 ${SOURCES})

set_target_properties(opengl2 PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_options(opengl2 PRIVATE "-static-libgcc" "-static-libstdc++" "-Wl,-Bstatic,--whole-archive" "-lwinpthread" "-Wl,--no-whole-archive")

# set includes path.

target_include_directories(opengl2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# link static libs.

target_link_libraries(opengl2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw3.a")
target_link_libraries(opengl2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libglad.a")
target_link_libraries(opengl2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libJolt.dll.a")
target_link_libraries(opengl2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libvsgImGui.dll.a")

# link dynamic libs.

target_link_libraries(opengl2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/OpenAL32.dll")
target_link_libraries(opengl2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/soft_oal.dll")

# copy .dll files after building executeable.

add_custom_command(TARGET opengl2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/OpenAL32.dll"
        $<TARGET_FILE_DIR:opengl2>
)

add_custom_command(TARGET opengl2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/soft_oal.dll"
        $<TARGET_FILE_DIR:opengl2>
)