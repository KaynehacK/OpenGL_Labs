cmake_minimum_required(VERSION 3.30)
project(Lab3_Textures)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_ROOT "C:/OpenGL/glfw-3.4.bin.WIN64/lib-mingw-w64")
set(GLEW_ROOT "C:/OpenGL/glew-2.2.0/bin/Release/x64")

include_directories(
        C:/OpenGL/glew-2.2.0/include
        C:/OpenGL/glfw-3.4.bin.WIN64/include
        C:/OpenGL/glm-master
        shaders
)

link_directories(
        C:/OpenGL/glfw-3.4.bin.WIN64/lib-mingw-w64
        C:/OpenGL/glew-2.2.0/bin/Release/x64
)

add_executable(Lab3_Textures
        Solutions/LoadShader.h
        Solutions/LoadShader.cpp
        Solutions/stb_image.h
        main.cpp
        Solutions/stb_image.cpp
)

target_link_libraries(Lab3_Textures
        glfw3
        glew32
        opengl32  # системная библиотека OpenGL на Windows
)