cmake_minimum_required(VERSION 3.29)

project(rlImGui)

add_library(rlImGui STATIC rlImGui.cpp)
# target_include_directories(rlImGui PUBLIC <includedirs>)
set_target_properties(rlImGui PROPERTIES LINKER_LANGUAGE CXX)
