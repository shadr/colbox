cmake_minimum_required(VERSION 3.29)
include(FetchContent)

project(gamesdl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O1 -Wall -Wextra -Wpedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
# set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections")
# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

FetchContent_Declare(box2d GIT_REPOSITORY https://github.com/erincatto/box2d GIT_TAG v3.0.0)
FetchContent_MakeAvailable(box2d)

add_subdirectory(libs/rlImGui)

include_directories(libs/)

add_executable(main src/main.cpp)
target_precompile_headers(main PUBLIC src/pch.hpp)

target_link_libraries(main rlImGui)
target_link_libraries(main raylib)
target_link_libraries(main imgui)
target_link_libraries(main box2d)
