#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <memory>

#include "Shader.h"
#include "Camera.h"
#include "PointSet.h"
#include "Curve.h"
#include "RevolutionSurface.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void processInput(GLFWwindow *window);
glm::vec3 screenToWorldCoordinates(double xpos, double ypos, int screenWidth, int screenHeight);

unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 0.5f, 3.0f));
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

enum class AppMode {
    INPUT_POINTS,
    VIEW_SURFACE
};
AppMode currentMode = AppMode::INPUT_POINTS;
bool modeChanged = false;

std::unique_ptr<PointSet> pointSet;
std::unique_ptr<Curve> curve;
std::unique_ptr<RevolutionSurface> revolutionSurface;

std::unique_ptr<Shader> pointShader;
std::unique_ptr<Shader> curveShader;
std::unique_ptr<Shader> surfaceShader;

const int SURFACE_SEGMENTS = 32;
const char ROTATION_AXIS = 'X';
float surfaceRotationAngleX = 0.0f;
float surfaceRotationAngleY = 0.0f;
const float ROTATION_SPEED = 50.0f;
glm::vec3 translation(0.0f, 0.5f, 0.0f);

bool mouseRotationEngaged = false;
bool mouseRotationStarted = true;
float startCursorPosX = 0.0f;
float startCursorPosY = 0.0f;

std::string loadShaderFromFile(const std::string& filePath) {
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        shaderFile.open(filePath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        return shaderStream.str();
    } catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        std::cerr << "File path: " << filePath << std::endl;
        return "";
    }
}

void processInput(GLFWwindow* window) {
    if (currentMode == AppMode::VIEW_SURFACE) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            surfaceRotationAngleY -= ROTATION_SPEED * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            surfaceRotationAngleY += ROTATION_SPEED * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            surfaceRotationAngleX -= ROTATION_SPEED * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            surfaceRotationAngleX += ROTATION_SPEED * deltaTime;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    if (currentMode == AppMode::VIEW_SURFACE) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        /*if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);*/

        if (mouseRotationEngaged) {
            if (mouseRotationStarted) {
                startCursorPosX = xpos;
                startCursorPosY = ypos;
                mouseRotationStarted = false;
            }

            surfaceRotationAngleX += ROTATION_SPEED * (ypos - startCursorPosY) * 0.005f;
            surfaceRotationAngleY += ROTATION_SPEED * (xpos - startCursorPosX) * 0.005f;

            startCursorPosX = xpos;
            startCursorPosY = ypos;
        }
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (currentMode == AppMode::VIEW_SURFACE) {
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (currentMode == AppMode::INPUT_POINTS && action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            glm::vec3 worldPos = screenToWorldCoordinates(xpos, ypos, SCR_WIDTH, SCR_HEIGHT);

            pointSet->addPoint(worldPos.x, worldPos.y);
            pointSet->updateBuffers();

            if (pointSet->getNumPoints() >= 2) {
                curve->generateCurve(pointSet->getPoints(), 10);
                curve->updateBuffers();
            }
        } else if (currentMode == AppMode::VIEW_SURFACE) {
            if (action == GLFW_PRESS) {
                mouseRotationEngaged = true;
                mouseRotationStarted = true;
            } else if (action == GLFW_RELEASE) {
                mouseRotationEngaged = false;
            }
        }
    }

}

glm::vec3 screenToWorldCoordinates(double xpos, double ypos, int screenWidth, int screenHeight) {
    float ndcX = (static_cast<float>(xpos) / screenWidth) * 2.0f - 1.0f;
    float ndcY = 1.0f - (static_cast<float>(ypos) / screenHeight) * 2.0f;

    float aspectRatio = static_cast<float>(screenWidth) / screenHeight;
    
    return glm::vec3(ndcX * aspectRatio, ndcY, 0.0f);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, true);
        if (key == GLFW_KEY_P) {
            if (currentMode == AppMode::INPUT_POINTS) {
                if (pointSet->getNumPoints() >= 2) {
                    currentMode = AppMode::VIEW_SURFACE;
                    modeChanged = true;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    firstMouse = true;
                    std::cout << "Switched to VIEW_SURFACE mode." << std::endl;

                    //pointSet->clearPoints();
                    //curve->clearCurve();
                } else {
                    std::cout << "Add at least 2 points to generate a surface." << std::endl;
                }
            }
        }
        if (key == GLFW_KEY_C && currentMode == AppMode::INPUT_POINTS) {
            pointSet->clearPoints();
            pointSet->updateBuffers();
            curve->clearCurve();
            curve->updateBuffers();
            std::cout << "Cleared all points." << std::endl;
        }
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Lab 4 - Revolution Surface", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    try {
        pointShader = std::make_unique<Shader>("../shaders/point.vert", "../shaders/point.frag");
        curveShader = std::make_unique<Shader>("../shaders/curve.vert", "../shaders/curve.frag");
        surfaceShader = std::make_unique<Shader>("../shaders/surface.vert", "../shaders/surface.frag");
    } catch (const std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        glfwTerminate();
        return -1;
    } catch (const std::runtime_error& e) {
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED: " << e.what() << std::endl;
        glfwTerminate();
        return -1;
    }

    pointSet = std::make_unique<PointSet>();
    curve = std::make_unique<Curve>();
    revolutionSurface = std::make_unique<RevolutionSurface>();

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (currentMode == AppMode::INPUT_POINTS) {
            float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT;
            glm::mat4 ortho_projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
            glm::mat4 identity_view = glm::mat4(1.0f);
            glm::mat4 model = glm::mat4(1.0f);

            pointShader->Use();
            pointShader->setMat4("projection", ortho_projection);
            pointShader->setMat4("view", identity_view);
            pointShader->setMat4("model", model);
            pointShader->setVec3("pointColor", 1.0f, 1.0f, 0.0f); 
            pointSet->Draw(*pointShader);

            if (pointSet->getNumPoints() >= 2) {
                curveShader->Use();
                curveShader->setMat4("projection", ortho_projection);
                curveShader->setMat4("view", identity_view);
                curveShader->setMat4("model", model);
                curveShader->setVec3("curveColor", 0.0f, 1.0f, 0.0f); 
                curve->Draw(*curveShader);
            }
        } else {
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();

            if (modeChanged) {
                if (pointSet->getNumPoints() >= 2) {
                    if (!curve->curvePoints.empty()){
                         revolutionSurface->generateSurface(curve->curvePoints, SURFACE_SEGMENTS, ROTATION_AXIS);
                    } else if (!pointSet->getPoints().empty()) {
                        revolutionSurface->generateSurface(pointSet->getPoints(), SURFACE_SEGMENTS, ROTATION_AXIS);
                    }
                    
                    if(!revolutionSurface->vertices.empty()){
                        revolutionSurface->setupBuffers();
                    }
                }
                modeChanged = false;
            }

            if (!revolutionSurface->vertices.empty()) {
                surfaceShader->Use();
                surfaceShader->setMat4("projection", projection);
                surfaceShader->setMat4("view", view);

                glm::mat4 surfaceModel = glm::mat4(1.0f);
                surfaceModel = glm::translate(surfaceModel, translation);
                surfaceModel = glm::rotate(surfaceModel, glm::radians(surfaceRotationAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
                surfaceModel = glm::rotate(surfaceModel, glm::radians(surfaceRotationAngleY), glm::vec3(0.0f, 1.0f, 0.0f));

                surfaceShader->setMat4("model", surfaceModel);
                
                surfaceShader->setVec3("lightPos", 1.0f, 2.0f, 2.0f);
                surfaceShader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
                surfaceShader->setVec3("objectColor", 0.5f, 0.7f, 0.8f);
                surfaceShader->setVec3("viewPos", camera.Position);

                revolutionSurface->Draw(*surfaceShader, surfaceModel);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
