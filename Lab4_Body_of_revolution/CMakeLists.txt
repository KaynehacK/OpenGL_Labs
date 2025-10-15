cmake_minimum_required(VERSION 3.30)
project(Lab4_Body_of_revolution)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_ROOT "C:/OpenGL/glfw-3.4.bin.WIN64/lib-mingw-w64")
set(GLEW_ROOT "C:/OpenGL/glew-2.2.0/bin/Release/x64")

include_directories(
        C:/OpenGL/glew-2.2.0/include
        C:/OpenGL/glfw-3.4.bin.WIN64/include
        C:/OpenGL/glm-master
        shaders
        include
)

link_directories(
        C:/OpenGL/glfw-3.4.bin.WIN64/lib-mingw-w64
        C:/OpenGL/glew-2.2.0/bin/Release/x64
)

add_executable(Lab4_Body_of_revolution
        Solutions/LoadShader.h
        Solutions/LoadShader.cpp
        src/main.cpp
        src/Shader.cpp
        src/Camera.cpp
        src/PointSet.cpp
        src/Curve.cpp
        src/RevolutionSurface.cpp
)

target_include_directories(Lab4_Body_of_revolution PUBLIC include)

target_link_libraries(Lab4_Body_of_revolution
        glfw3
        glew32
        opengl32  # системная библиотека OpenGL на Windows
)