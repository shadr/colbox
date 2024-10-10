cmake_minimum_required(VERSION 3.29)
project(gamesdl)
set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_COMPILER clang++)


find_package(raylib REQUIRED)
find_package(imgui REQUIRED)

add_executable(main main.cpp rlImGui/rlImGui.cpp)
target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(main raylib)
target_link_libraries(main imgui)
