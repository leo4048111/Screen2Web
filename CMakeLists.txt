cmake_minimum_required(VERSION 3.15)

project(screen2web)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SCREEN2WEB_ROOT_DIR ${CMAKE_SOURCE_DIR})

set(SRC_FILES
${SCREEN2WEB_ROOT_DIR}/src/main.cpp
${SCREEN2WEB_ROOT_DIR}/src/base/screenshotter.cpp
${SCREEN2WEB_ROOT_DIR}/src/window/window.cpp
${SCREEN2WEB_ROOT_DIR}/src/imgui_impl_sdl2.cpp
${SCREEN2WEB_ROOT_DIR}/src/imgui_impl_opengl3.cpp
${SCREEN2WEB_ROOT_DIR}/src/imgui/imgui.cpp
${SCREEN2WEB_ROOT_DIR}/src/imgui/imgui_widgets.cpp
${SCREEN2WEB_ROOT_DIR}/src/imgui/imgui_tables.cpp
${SCREEN2WEB_ROOT_DIR}/src/imgui/imgui_draw.cpp
${SCREEN2WEB_ROOT_DIR}/src/imgui/imgui_demo.cpp
)

# dependencies
find_package(sdl2 REQUIRED)

if (NOT sdl2_FOUND)
    message(FATAL_ERROR "SDL2 not found")
endif()

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(screen2web ${SRC_FILES})

# platform link libs
if (APPLE)
    set(PLATFORM_LINK_LIBS ${SDL2_LIBRARIES} "-framework OpenGL")
else()
    set(PLATFORM_LINK_LIBS ${SDL2_LIBRARIES} OpenGL32)
endif()

target_link_libraries(screen2web ${PLATFORM_LINK_LIBS})
