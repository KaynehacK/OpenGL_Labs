#ifndef TEST_LOADSHADER_H
#define TEST_LOADSHADER_H

#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include "stb_image.h"

using namespace std;

string LoadShader(const string& path);

GLuint CompileShader(const std::string& path, GLenum shaderType);

GLuint CompileShaderProgram(GLuint vertexShader, GLuint fragmentShader);

void GenTexture(GLuint &texture, const char* path, GLint internalFormat, GLenum format);

#endif //TEST_LOADSHADER_H
