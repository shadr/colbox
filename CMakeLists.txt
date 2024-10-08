cmake_minimum_required(VERSION 3.29)
project(gamesdl)
set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_CXX_FLAGS "-O2")
set(CMAKE_CXX_COMPILER clang++)


find_package(raylib REQUIRED)
find_package(imgui REQUIRED)

add_executable(main main.cpp rlImGui/rlImGui.cpp)
target_link_libraries(main raylib)
target_link_libraries(main imgui)
