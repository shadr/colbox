cmake_minimum_required(VERSION 3.29)
include(FetchContent)

project(sgame)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O1 -Wall -Wextra -Wpedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
# set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections")
# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

FetchContent_Declare(box2d GIT_REPOSITORY https://github.com/erincatto/box2d GIT_TAG v3.0.0)
FetchContent_MakeAvailable(box2d)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
set(USE_EXTERNAL_GLFW ON CACHE BOOL "" FORCE)
FetchContent_Declare(raylib GIT_REPOSITORY https://github.com/raysan5/raylib GIT_TAG 5.0)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(glm GIT_REPOSITORY	https://github.com/g-truc/glm.git GIT_TAG 1.0.1)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
  imgui
  # URL "https://github.com/ocornut/imgui/archive/v1.91.3.zip"
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.91.3
)
FetchContent_MakeAvailable(imgui)
FetchContent_GetProperties(imgui)
if (NOT imgui_POPULATED)
	FetchContent_Populate(imgui)
	add_subdirectory(${imgui_SOURCE_DIR})
endif()

# add_subdirectory(libs/rlImGui)

include_directories(libs/)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/libs/*.cpp")

add_executable(main)
target_sources(main PRIVATE ${PROJECT_SOURCES})
# target_precompile_headers(main PUBLIC src/pch.hpp)

target_include_directories(main PRIVATE ${imgui_SOURCE_DIR})

add_library(imgui 
	${imgui_SOURCE_DIR}/imgui.cpp
	${imgui_SOURCE_DIR}/imgui_draw.cpp
	${imgui_SOURCE_DIR}/imgui_widgets.cpp
	${imgui_SOURCE_DIR}/imgui_demo.cpp
	${imgui_SOURCE_DIR}/imgui_tables.cpp
	# ${imgui_SOURCE_DIR}/examples/imgui_impl_glfw.cpp
	# ${imgui_SOURCE_DIR}/examples/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PRIVATE ${imgui_SOURCE_DIR})

target_link_libraries(main raylib)
target_link_libraries(main imgui)
target_link_libraries(main box2d)
target_link_libraries(main glm::glm)

if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an .html file.
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
	set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Os")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -O3 -Wall -s FORCE_FILESYSTEM=1 -s MINIFY_HTML=0 -s ASYNCIFY -s ALLOW_MEMORY_GROWTH")
	set(CMAKE_EXECUTABLE_SUFFIX ".html") # This line is used to set your executable to build with the emscripten html template so that you can directly open it.
    set_target_properties(main PROPERTIES SUFFIX ".html")
	# target_compile_definitions(main PUBLIC ASSETS_PATH="/assets/") # Set the asset path macro in release mode to a relative path that assumes the assets folder is in the same directory as the game executable
endif()
