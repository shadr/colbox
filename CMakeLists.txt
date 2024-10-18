cmake_minimum_required(VERSION 3.29)
include(FetchContent)

set(IMGUI_VERSION v1.91.3)
set(RAYLIB_VERSION 5.0)
set(GLM_VERSION 1.0.1)
set(BOX2D_VERSION v3.0.0)
set(ENTT_VERSION v3.13.2)

set(FETCHCONTENT_QUIET FALSE)

project(sgame)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

FetchContent_Declare(box2d URL https://github.com/erincatto/box2d/archive/refs/tags/${BOX2D_VERSION}.tar.gz GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(box2d)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
set(USE_EXTERNAL_GLFW ON CACHE BOOL "" FORCE)
FetchContent_Declare(raylib URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(glm URL https://github.com/g-truc/glm/archive/refs/tags/${GLM_VERSION}.tar.gz GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(entt URL https://github.com/skypjack/entt/archive/refs/tags/${ENTT_VERSION}.tar.gz GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(entt)

FetchContent_Declare(
  imgui
  URL https://github.com/ocornut/imgui/archive/refs/tags/${IMGUI_VERSION}.tar.gz 
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(imgui)
FetchContent_GetProperties(imgui)
if (NOT imgui_POPULATED)
	FetchContent_Populate(imgui)
	add_subdirectory(${imgui_SOURCE_DIR})
endif()
add_library(imgui 
	${imgui_SOURCE_DIR}/imgui.cpp
	${imgui_SOURCE_DIR}/imgui_draw.cpp
	${imgui_SOURCE_DIR}/imgui_widgets.cpp
	${imgui_SOURCE_DIR}/imgui_demo.cpp
	${imgui_SOURCE_DIR}/imgui_tables.cpp
)
target_include_directories(imgui PRIVATE ${imgui_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

add_executable(${PROJECT_NAME})
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/libs/*.cpp")
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_precompile_headers(${PROJECT_NAME} PUBLIC src/pch.hpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${imgui_SOURCE_DIR} libs/)

target_link_libraries(${PROJECT_NAME} raylib imgui box2d glm::glm EnTT)

if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an .html file.
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
	set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Os")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -O3 -Wall -s FORCE_FILESYSTEM=1 -s MINIFY_HTML=0 -s ASYNCIFY -s ALLOW_MEMORY_GROWTH")
	set(CMAKE_EXECUTABLE_SUFFIX ".html") # This line is used to set your executable to build with the emscripten html template so that you can directly open it.
	set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
	# target_compile_definitions(main PUBLIC ASSETS_PATH="/assets/") # Set the asset path macro in release mode to a relative path that assumes the assets folder is in the same directory as the game executable
endif()
