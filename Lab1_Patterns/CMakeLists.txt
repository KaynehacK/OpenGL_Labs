cmake_minimum_required(VERSION 3.30)
project(Test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пути к библиотекам (укажите свои!)
set(GLFW_ROOT "C:/OpenGL/glfw-3.4.bin.WIN64/lib-mingw-w64")
set(GLEW_ROOT "C:/OpenGL/glew-2.2.0/bin/Release/x64")

# Подключаем библиотеки и include'ы
include_directories(
        C:/OpenGL/glew-2.2.0/include
        C:/OpenGL/glfw-3.4.bin.WIN64/include
        shaders
)

link_directories(
        C:/OpenGL/glfw-3.4.bin.WIN64/lib-mingw-w64
        C:/OpenGL/glew-2.2.0/bin/Release/x64
)

add_executable(Test main.cpp
        Solutions/LoadShader.h
        Solutions/LoadShader.cpp)

# Линкуем библиотеки
target_link_libraries(Test
        glfw3
        glew32
        opengl32  # системная библиотека OpenGL на Windows
)
