#include "Solutions//LoadShader.h"
#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int mode = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        mode = 1 - mode;
        std::cout << "Mode switched to: " << ((mode == 0) ? "Wavy pattern" : "Circular pattern") << std::endl;
    }
}

int main() {
    glfwInit(); //Инициация GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Мажорная
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Минорная
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Установка профайла для которого создается контекст
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE); //Выключение возможности изменения размера окна

    //Создание окна
    GLFWwindow* window = glfwCreateWindow(1000, 1000, "The window", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);

    //Инициация GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    GLfloat vertices[] = {
            // positions                    // colors
            0.9f,  0.9f, 0.0f,         1.0f, 0.0f, 0.0f,   // Верхний правый угол
            0.9f, -0.9f, 0.0f,         0.0f, 1.0f, 0.0f,  // Нижний правый угол
            -0.9f, -0.9f, 0.0f,     0.0f, 0.0f, 1.0f, // Нижний левый угол
            -0.9f,  0.9f, 0.0f,     0.0f, 0.0f, 0.0f  // Верхний левый угол
    };
    GLuint indices[] = {  // Помните, что мы начинаем с 0!
            0, 1, 3,   // Первый треугольник
            1, 2, 3    // Второй треугольник
    };

    GLuint circularFragmentShader = CompileShader("../circular.fs", GL_FRAGMENT_SHADER);
    GLuint wavyFragmentShader = CompileShader("../wavy.fs", GL_FRAGMENT_SHADER);
    GLuint vertexShader = CompileShader("../vertex.vs", GL_VERTEX_SHADER);

    GLuint circularShaderProgram = CompileShaderProgram(vertexShader, circularFragmentShader);
    GLuint wavyShaderProgram = CompileShaderProgram(vertexShader, wavyFragmentShader);

    glUseProgram(circularShaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(wavyFragmentShader);
    glDeleteShader(circularFragmentShader);

    GLuint VBO, VAO, EBO;

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Игровой цикл
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (mode == 0) {
            glUseProgram(wavyShaderProgram);
        } else {
            glUseProgram(circularShaderProgram);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
