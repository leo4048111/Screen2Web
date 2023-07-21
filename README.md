# Screen2Web
A live screen sharing server implemented with cpp-httplib and ImGui, enabling sharing screen across LAN with some simple clicks.

## Download this project

`git clone --recurse-submodules https://github.com/leo4048111/Screen2Web.git`

## Build prerequisites

+ Installed OpenGL and SDL2 libraries
+ Cmake

## Platforms

+ **Windows**
+ **MacOS** (Developing)
+ **Linux** (Developing)

## Usage

+ Build this project
+ Run binary, a streaming server will start on port 80
+ Select the window to share
+ Use your favorite browser, open url `<server_ip>:80`
+ Shared window should appear on web page
