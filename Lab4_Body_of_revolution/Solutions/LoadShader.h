#ifndef TEST_LOADSHADER_H
#define TEST_LOADSHADER_H

#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

string LoadShader(const string& path);

GLuint CompileShader(const std::string& path, GLenum shaderType);

GLuint CompileShaderProgram(GLuint vertexShader, GLuint fragmentShader);

#endif //TEST_LOADSHADER_H
