cmake_minimum_required(VERSION 3.15)

project(RaycastingC)

find_package(raylib REQUIRED)

add_executable(main main.c)

target_link_libraries(main raylib winmm gdi32 opengl32)

target_compile_definitions(main PRIVATE PLATFORM_DESKTOP)