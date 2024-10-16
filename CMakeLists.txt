cmake_minimum_required(VERSION 3.29)

project(gamesdl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O1 -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
# set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections")
# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

include_directories(libs/)

add_executable(main src/main.cpp libs/rlImGui/rlImGui.cpp)
target_precompile_headers(main PRIVATE libs/rlImGui/rlImGui.h src/rtree.hpp)
target_link_libraries(main raylib)
target_link_libraries(main imgui)
