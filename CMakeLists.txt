cmake_minimum_required(VERSION 3.29)

project(gamesdl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O1 -Wall -Wextra -Wpedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
# set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections")
# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

add_subdirectory(libs/rlImGui)

include_directories(libs/)

add_executable(main src/main.cpp)
target_precompile_headers(main PUBLIC src/rtree.hpp)
target_link_libraries(main rlImGui)
target_link_libraries(main raylib)
target_link_libraries(main imgui)
