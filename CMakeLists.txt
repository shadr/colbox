cmake_minimum_required(VERSION 3.29)
include(FetchContent)

project(gamesdl)

set(CMAKE_GENERATOR "Unix Makefiles")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O0 -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
# set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections")
# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

include_directories(libs/)

FetchContent_Declare(flecs GIT_REPOSITORY https://github.com/SanderMertens/flecs GIT_TAG v4.0.2)
FetchContent_MakeAvailable(flecs)

add_executable(main src/main.cpp libs/rlImGui/rlImGui.cpp)
target_link_libraries(main raylib)
target_link_libraries(main imgui)
target_link_libraries(main flecs::flecs)
target_link_libraries(main glm)
