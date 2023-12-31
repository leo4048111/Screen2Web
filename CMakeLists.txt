cmake_minimum_required(VERSION 3.15)

project(screen2web)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SCREEN2WEB_ROOT_DIR ${CMAKE_SOURCE_DIR})
set(SCREEN2WEB_THIRD_PARTY_DIR ${SCREEN2WEB_ROOT_DIR}/third_party)

set(SRC_FILES
${SCREEN2WEB_ROOT_DIR}/src/main.cpp
${SCREEN2WEB_ROOT_DIR}/src/window/window.cpp
${SCREEN2WEB_ROOT_DIR}/src/base/frame.cpp
${SCREEN2WEB_ROOT_DIR}/src/base/httpserver.cpp
${SCREEN2WEB_ROOT_DIR}/src/imgui_impl_sdl2.cpp
${SCREEN2WEB_ROOT_DIR}/src/imgui_impl_opengl3.cpp
${SCREEN2WEB_ROOT_DIR}/src/imgui/imgui.cpp
${SCREEN2WEB_ROOT_DIR}/src/imgui/imgui_widgets.cpp
${SCREEN2WEB_ROOT_DIR}/src/imgui/imgui_tables.cpp
${SCREEN2WEB_ROOT_DIR}/src/imgui/imgui_draw.cpp
${SCREEN2WEB_ROOT_DIR}/src/imgui/imgui_demo.cpp
)


if(APPLE)
    set(SRC_FILES ${SRC_FILES} ${SCREEN2WEB_ROOT_DIR}/src/base/macScreencapturerImpl.cpp)
elseif(WIN32)
    set(SRC_FILES ${SRC_FILES} ${SCREEN2WEB_ROOT_DIR}/src/base/winSDIScreencapturerImpl.cpp)
#   set(SRC_FILES ${SRC_FILES} ${SCREEN2WEB_ROOT_DIR}/src/base/winDXGIScreencapturerImpl.cpp)
# else()
#     set(SRC_FILES ${SRC_FILES} ${SCREEN2WEB_ROOT_DIR}/src/base/linuxScreencapturerImpl.cpp)
endif()

# prebuild dependencies
# SDL2
find_package(sdl2 REQUIRED)

if (NOT sdl2_FOUND)
    message(FATAL_ERROR "SDL2 not found")
endif()

include_directories(${SDL2_INCLUDE_DIRS})

# third party header-only dependencies
# httplib
include_directories(${SCREEN2WEB_THIRD_PARTY_DIR}/cpp-httplib)
# svpng
include_directories(${SCREEN2WEB_THIRD_PARTY_DIR}/svpng)

add_executable(screen2web ${SRC_FILES})

# platform link libs
if (APPLE)
    set(PLATFORM_LINK_LIBS ${SDL2_LIBRARIES} "-framework OpenGL" "-framework CoreGraphics" "-framework ApplicationServices")
elseif(WIN32)
    set(PLATFORM_LINK_LIBS ${SDL2_LIBRARIES} OpenGL32 user32 d3d11 dxgi)
else()
    set(PLATFORM_LINK_LIBS ${SDL2_LIBRARIES} OpenGL32)
endif()

target_link_libraries(screen2web ${PLATFORM_LINK_LIBS})
