#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>

// Для матриц и векторов подключим GLM (готовая библиотека)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Solutions/LoadShader.h"

// ---------------- ПАРАМЕТРЫ СЕТКИ ----------------
const int GRID_SIZE = 128;    // Кол-во делений по каждой оси (>= 32)
const float STEP = 0.1f;     // Шаг сетки
const float SCALE = 0.4f;    // Масштаб поверхности

// ---------------- ПАРАМЕТРЫ ТРАНСФОРМАЦИЙ ----------------
float rotationAngleX = 0.0f;
float rotationAngleY = 0.0f;
float scaleFactor = 1.0f;
glm::vec3 translation(0.0f, 0.0f, -3.0f);

// ---------------- ТЕКСТУРЫ ---------------
const char* path1 = "../tex1.png";
const char* path2 = "../drip.png";

// ---------------- ПЕРЕМЕННЫЕ ДЛЯ ВВОДА ---------------
int inputDirection = 0;
bool mouseRotationEngaged = false;
bool mouseRotationStarted = true;
float startCursorPosX = 0.0f;
float startCursorPosY = 0.0f;
const float arrowsRotationSpeed = 0.03f;
float mixValue = 0.5f;

// ---------------- ВВОД С КЛАВИАТУРЫ -----------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        inputDirection += 1;
    }
    if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
        inputDirection -= 1;
    }

    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        inputDirection += 2;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
        inputDirection -= 2;
    }

    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        inputDirection += 4;
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) {
        inputDirection -= 4;
    }

    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        inputDirection += 8;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) {
        inputDirection -= 8;
    }
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        scaleFactor += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        scaleFactor -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
        mixValue -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
        mixValue += 0.01f;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouseRotationEngaged = true;
            mouseRotationStarted = true;
        } else if (action == GLFW_RELEASE) {
            mouseRotationEngaged = false;
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (mouseRotationEngaged) {
        if (mouseRotationStarted) {
            startCursorPosX = xpos;
            startCursorPosY = ypos;
            mouseRotationStarted = false;
        }

        rotationAngleX += (ypos - startCursorPosY) * 0.005f;
        rotationAngleY += (xpos - startCursorPosX) * 0.005f;

        startCursorPosX = xpos;
        startCursorPosY = ypos;
    }
}

void ArrowsInput(int inputDirection, float &rotationAngleX, float &rotationAngleY) {
    switch (inputDirection) {
        case 1: rotationAngleX -= arrowsRotationSpeed;
            break;
        case 2: rotationAngleX += arrowsRotationSpeed;
            break;
        case 4: rotationAngleY -= arrowsRotationSpeed;
            break;
        case 5: rotationAngleX -= arrowsRotationSpeed;
            rotationAngleY -= arrowsRotationSpeed;
            break;
        case 6: rotationAngleX += arrowsRotationSpeed;
            rotationAngleY -= arrowsRotationSpeed;
            break;
        case 7: rotationAngleY -= arrowsRotationSpeed;
            break;
        case 8: rotationAngleY += arrowsRotationSpeed;
            break;
        case 9: rotationAngleX -= arrowsRotationSpeed;
            rotationAngleY += arrowsRotationSpeed;
            break;
        case 10: rotationAngleX += arrowsRotationSpeed;
            rotationAngleY += arrowsRotationSpeed;
            break;
        case 11: rotationAngleY += arrowsRotationSpeed;
            break;
        case 13: rotationAngleX -= arrowsRotationSpeed;
            break;
        case 14: rotationAngleX += arrowsRotationSpeed;
            break;
    }
}

// ---------------- АНАЛИТИЧЕСКАЯ ФУНКЦИЯ ----------------
float surfaceFunc(float x, float y) {
    return 0.6f * sin(y * 2 + sin(x * 2) * 3); // Функция 0
    //return x*x / 6 + y*y / 6; // Функция 1
    //return 0.5f * sin( x*x + y*y); // Функция 2
}

// ---------------- ГРАДИЕНТ ДЛЯ НОРМАЛЕЙ ----------------
glm::vec3 calcNormal(float x, float y) {
    // частные производные
    float dx = ( 18.0f * cosf(2.0f * x ) * cosf ( 3.0f * sinf(2.0f * x) + 2.0f * y ) )/5.0f; //Функция 0
    float dy = (6.0f * cosf( 2.0f * y + 3.0f * sinf( 2.0f * x ) ) )/6.0f;
    //float dx = x / 3; //Функция 1
    //float dy = y / 3;
//    float dx = 0.5f * 2 * x * cos( x*x + y*y); //Функция 2
//    float dy = 0.5f * 2 * y * cos( x*x + y*y);

    glm::vec3 n(-dx, -dy, 1.0f);
    return glm::normalize(n);
}

// ---------------- ГЕНЕРАЦИЯ ВЕРШИН И ИНДЕКСОВ ----------------
void generateSurface(std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    for (int i = 0; i <= GRID_SIZE; i++) {
        for (int j = 0; j <= GRID_SIZE; j++) {
            float x = (i - GRID_SIZE / 2) * STEP;
            float y = (j - GRID_SIZE / 2) * STEP;
            float z = surfaceFunc(x, y);
            glm::vec3 n = calcNormal(x, y);
            float tx = (1.0f/GRID_SIZE) * i;
            float ty = 1.0f - (1.0f/GRID_SIZE) * j;

            // Позиция (x, y, z)
            vertices.push_back(x * SCALE);
            vertices.push_back(y * SCALE);
            vertices.push_back(z * SCALE);

            // Нормаль (nx, ny, nz)
            vertices.push_back(n.x);
            vertices.push_back(n.y);
            vertices.push_back(n.z);

            // Координаты текстуры (s, t)
            vertices.push_back(tx);
            vertices.push_back(ty);

        }
    }

    // Индексы (треугольники)
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int row1 = i * (GRID_SIZE + 1);
            int row2 = (i + 1) * (GRID_SIZE + 1);

            indices.push_back(row1 + j);
            indices.push_back(row2 + j);
            indices.push_back(row1 + j + 1);

            indices.push_back(row1 + j + 1);
            indices.push_back(row2 + j);
            indices.push_back(row2 + j + 1);
        }
    }
}

// ---------------- MAIN ----------------
int main() {
    if (!glfwInit()) {
        std::cerr << "Ошибка инициализации GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1000, 800, "Surface with Blinn-Phong", nullptr, nullptr);
    if (!window) {
        std::cerr << "Ошибка создания окна" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Ошибка инициализации GLEW" << std::endl;
        return -1;
    }

    // --- Генерация данных поверхности ---
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    generateSurface(vertices, indices);

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Атрибуты: позиция (3 float) + нормаль (3 float)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // --- Компиляция шейдеров ---
    GLuint vertexShader = CompileShader("../vertex_shader.glsl", GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader("../fragment_shader.glsl", GL_FRAGMENT_SHADER);
    GLuint shaderProgram = CompileShaderProgram(vertexShader, fragmentShader);

    glEnable(GL_DEPTH_TEST);

    // --- Генерация текстуры ---
    GLuint texture1, texture2;
    GenTexture(texture1, path1, GL_RGB, GL_RGBA);
    GenTexture(texture2, path2, GL_RGBA, GL_RGBA);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);

    // --- Цикл отрисовки ---
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        ArrowsInput(inputDirection, rotationAngleX, rotationAngleY);
        // Матрицы
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, translation);
        model = glm::rotate(model, rotationAngleX, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotationAngleY, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scaleFactor));

        glm::mat4 view = glm::lookAt(
                glm::vec3(0.0f, 0.5f, 3.0f),  // позиция камеры
                glm::vec3(0.0f, 0.0f, 0.0f),  // куда смотрим
                glm::vec3(0.0f, 1.0f, 0.0f)   // вектор "вверх"
        );

        glm::mat4 projection = glm::perspective(
                glm::radians(45.0f), 1000.0f / 800.0f, 0.1f, 100.0f
        );

        // Передача uniform-переменных
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 2.0f, 2.0f, 2.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), 0.0f, 1.5f, 3.0f);

        glUniform1f(glGetUniformLocation(shaderProgram, "mixValue"), mixValue);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
