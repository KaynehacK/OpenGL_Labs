#include "LoadShader.h"

string LoadShader(const string& path) {
    ifstream file;
    try {
        file.open(path);
        stringstream stringStream;
        stringStream << file.rdbuf();
        file.close();
        return stringStream.str();
    } catch(std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        std::cerr << "File path: " << path << std::endl;
        return "";
    }
}

GLuint CompileShader(const std::string& path, GLenum shaderType) {
    std::string shaderCode = LoadShader(path);
    if (shaderCode.empty()) {
        return 0; // ошибка при загрузке файла
    }

    const char* shaderSource = shaderCode.c_str();

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    // Проверка на ошибки компиляции
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);

        std::cerr << "ERROR::SHADER::COMPILATION_FAILED (" << path << "):\n"
                  << infoLog << std::endl;

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint CompileShaderProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    GLchar infoLog[512];

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog
                  << std::endl;
    }
    return shaderProgram;
}